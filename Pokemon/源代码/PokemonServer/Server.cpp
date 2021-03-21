#include "server.h"
#include <conio.h>
#pragma warning(disable:4996)

std::vector<Pokemon*> pokemonLibrary;
std::vector<Pokemon*> individualPokemon;//个体宝可梦，包括用户的和系统生成的

Server::Server()
{
	userFileLoc = "./userData.dat";
	isRunning = false;

	listenFd = INVALID_SOCKET;
	connectFd = INVALID_SOCKET;
}

Server::~Server()
{
	//删除所有服务端点
	serverMutex.lock();
	int size = serverEndpoints.size();
	for (int i = size - 1; i >= 0; i--)
	{
		delete serverEndpoints[i];
		serverEndpoints[i] = nullptr;
		serverEndpoints.pop_back();
	}
	serverMutex.unlock();

	closesocket(listenFd);
	closesocket(connectFd);

	//解除与Socket库的绑定并且释放Socket库所占用的系统资源,要不要用循环判断？？？
	WSACleanup(); 
}

//初始化并启动服务器运行，共2个部分：初始化用户数据文件，初始化socket
bool Server::InitServer()
{
	//初始化用户数据文件，第一次打开服务器没有数据文件需要创建一个
	std::ofstream userDataFile;
	userDataFile.open(userFileLoc, std::ios::app | std::ios::out);//文件不存在就创建一个，存在也不会删除原先内容
	if (userDataFile.is_open())
	{
		std::cout << "打开/创建用户数据文件成功!" << std::endl;
		userDataFile.close();
	}
	else
	{
		std::cout << "打开/创建用户数据文件失败..." << std::endl;
		return false;
	}
	//加载文件到内存（以下顺序不能变）
	LoadSpeciesPokemon();
	LoadIndividualPKM();
	LoadUsersData();

	//初始化socket
	//1.加载套接字库
	WORD versionRequested = MAKEWORD(2, 2);//版本号
	WSADATA wsaData;
	int err = WSAStartup(versionRequested, &wsaData);//使用Socket之前必须调用,加载套接字库返回信息至err
	if (err != 0)
	{
		std::cout << "加载套接字库失败..." << std::endl;
		return false;
	}
	else
	{
		std::cout << "加载套接字库成功！" << std::endl;
	}
	//2.检测套接字库版本号
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wHighVersion) != 2) 
	{
		std::cout << "套接字库版本号不符..." << std::endl;
		WSACleanup();
		return false;
	}
	else 
	{
		std::cout << "套接字库版本正确！" << std::endl;
	}

	//3.创建套接字，地址族为Inernet网络地址，socket接受数据格式为流，协议是TCP
	listenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//此处为主动套接字
	if (listenFd == INVALID_SOCKET)
	{
		std::cout << "创建套接字失败..." << std::endl;
		closesocket(listenFd);
		WSACleanup();
		return false;
	}
	else
	{
		std::cout << "创建套接字成功！" << std::endl;
	}

	//4.将ip地址和端口关联套接字结构，绑定套接字
	SOCKADDR_IN serverAddr;//定义server发送和接收数据包的地址
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);//网络字节序,端口号固定是SERVER_PORT（8000）
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//网络字节序，系统自动填入本机IP地址
	
	err = bind(listenFd, (SOCKADDR*)&serverAddr, sizeof(serverAddr));//绑定套接字，此处仍为主动套接字
	if (err == SOCKET_ERROR)
	{
		std::cout << "套接字绑定失败..." << std::endl;
		closesocket(listenFd);
		WSACleanup();
		return false;
	}
	else
	{
		std::cout << "套接字绑定成功！" << std::endl;
	}

	//5.设置listenFd为被动套接字（listen函数执行之前的套接字都是主动套接字），进入监听模式等待连接请求
	err = listen(listenFd, MAX_QUEUE_LEN);//转为被动套接字
	if (err == SOCKET_ERROR)
	{
		std::cout << "转换至监听套接字失败..." << std::endl;
		closesocket(listenFd);
		WSACleanup();
		return false;
	}
	else 
	{
		isRunning = true;
		std::cout << "转换至监听套接字成功！" << std::endl;
		std::cout << "服务器在" << SERVER_PORT << "号端口上运行...按下任意键可停止服务器运行..." << std::endl;
	}

	return true;
}

//运行服务器：执行线程以及在线程执行完毕后销毁连接端点,并关闭服务器
void Server::Run()
{
	//执行线程
	std::thread communicateThread(&Server::Communicate, this);
	std::thread endThread(&Server::End, this);
	communicateThread.join();
	endThread.join();

	//线程执行结束，销毁所有连接端点
	serverMutex.lock();
	int size = static_cast<int>(serverEndpoints.size());
	for (int i = size - 1; i >= 0; i--)
	{
		delete serverEndpoints[i];
		serverEndpoints[i] = nullptr;
		serverEndpoints.pop_back();
	}
	serverMutex.unlock();

	closesocket(listenFd);//关闭套接字
	//closesocket(connectFd);
	WSACleanup();//释放dll资源
	std::cout << std::endl << "服务器已停止运行...";

}

void Server::Communicate()
{
	while (isRunning)
	{
		//接受连接请求并重新创建一个套接字用于和客户机之间通信连接(listenFd继续监听工作)
		SOCKADDR_IN clientAddr;
		int clientAddrLen = sizeof(clientAddr);
		connectFd = accept(listenFd, (SOCKADDR*)&clientAddr, &clientAddrLen);//阻塞直到等待接收到有客户端请求
		if (connectFd == INVALID_SOCKET)//连接失败
		{
			if (isRunning)
			{
				std::cout << "连接到客户端失败..." << std::endl;
			}
			else
				;

			closesocket(connectFd);
		}
		else//连接成功
		{
			std::cout << "连接到客户端成功，客户端地址为：（"
						<< inet_ntoa(clientAddr.sin_addr) <<" : "
						<< clientAddr.sin_port << ")"<< std::endl;

			//开始收发数据
			std::string sendBufString = "";
			memset(socketBuffer, 0, MAX_BUF_LEN);
			int recvBuf = recv(connectFd, socketBuffer, MAX_BUF_LEN, 0);//这里不用持续接收
			if (recvBuf == 0 || recvBuf == SOCKET_ERROR)
			{
				std::cout << "服务器端数据接受失败..." << std::endl;

				sendBufString = "RECV_ERROR";
				strcpy(socketBuffer, sendBufString.c_str());
				send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端

			}
			else
			{
				std::vector<std::string> recvRequest = SplitBuf();

				if (recvRequest.size() < 3)//长度小于3的肯定是无效的请求
				{
					std::cout << "无效的请求" << std::endl;

					sendBufString = "RECV_ERROR";
					strcpy(socketBuffer, sendBufString.c_str());
					send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端
				}
				else if (recvRequest[0] == "SIGN_UP")//收到的请求是注册
				{
					std::cout << "开始处理注册请求" << std::endl;
					SignUp(recvRequest[1], recvRequest[2]);
				}
				else if (recvRequest[0] == "LOG_IN")//收到的请求是登录
				{
					std::cout << "开始处理登录请求" << std::endl;
					LogIn(recvRequest[1], recvRequest[2]);
				}
				else//剩余的也是无效的请求
				{
					std::cout << "无效的请求" << std::endl;
					sendBufString = "RECV_ERROR";
					strcpy(socketBuffer, sendBufString.c_str());
					send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端
				}

				closesocket(connectFd);//接收一次就可以断开连接了

			}//end of else

		}//end of else(连接成功)
	}//end of while(isRunning)
	
}

void Server::End()
{
	_getch();//按任意键退出，阻塞函数，直到有字符输入时才会返回
	
	isRunning = false;//标志服务器停止运行

	closesocket(listenFd);//停止监听
}

void Server::ManageEndpoint(Endpoint* endpoint)
{
	endpoint->Run();//让该服务端点开始运行，运行时此处阻塞，运行结束才会往下执行代码

	//该服务端点运行结束，及时从服务器中销毁
	serverMutex.lock();
	int size = serverEndpoints.size();
	bool isFound = false;
	for (int i = 0; i < size && !isFound; i++)
	{
		if (serverEndpoints[i] == endpoint)
		{
			isFound = true;
			serverEndpoints.erase(serverEndpoints.begin() + i);
			delete endpoint;
			endpoint = nullptr;
		}
		else
			;
	}
	serverMutex.unlock();
}

//分割接收到的数据，数据以空格' '做分隔符
std::vector<std::string> Server::SplitBuf() const
{
	std::vector<std::string> result;
	std::string splitData = "";//暂存得到的字符串

	int curHandleLoc = 0;//当前处理到的字符在字符串中的位置
	while (socketBuffer[curHandleLoc] != '\0')//如果字符串没有处理结束
	{
		if (socketBuffer[curHandleLoc] != ' ')//如果当前处理的字符不是分隔符' '，则继续拼接
		{
			splitData += socketBuffer[curHandleLoc];
			curHandleLoc++;
		}
		else if (curHandleLoc != 0)//如果处理到的是分隔符且当且不是处理第一个位置
		{
			curHandleLoc++;//跳过该字符

			result.push_back(splitData);//保存当前得到的数据段

			splitData = "";//初始化
		}
		else
		{
			curHandleLoc++;//跳过该字符
		}
	}
	result.push_back(splitData);//最后一个数据段不是以分隔符结尾的，在循环体中没保存到结果中

	return result;
}

//注册,注册成功则向用户发送专门负责该用户的服务端口号和该用户的ID，失败则发送的端口号为0
void Server::SignUp(std::string userName, std::string password)
{
	std::string sendBufString = "";

	//检查该用户的用户名是否被注册过,没注册过则允许注册，否则拒绝
	int userID = CheckExistence(userName);
	if (userID == NOT_EXIST)
	{
		//允许注册
		userID = userList.size();//分配ID号
		sendBufString = "ALLOW_SIGNUP";
		std::cout << "用户注册成功" << std::endl;

		//添加该用户到用户表中
		User* newUser = new User(userID, userName, password);
		userList.push_back(newUser);

		//给该用户分配三只不同种族的宝可梦
		int first = -1, second = -1, third = -1;//获得的第一只和第二只宝可梦的种族ID
		serverMutex.lock();
		first = newUser->GainPokemon(first, second, 1);
		second = newUser->GainPokemon(first, second, 1);
		third = newUser->GainPokemon(first, second, 1);
		serverMutex.unlock();

		//添加该用户到用户文件中
		serverMutex.lock();
		newUser->AppendUserFile(userFileLoc);
		serverMutex.unlock();
	}
	else
	{
		//拒绝注册请求
		sendBufString = "EXIST";
		std::cout << "用户注册失败" << std::endl;
	}
	
	strcpy(socketBuffer, sendBufString.c_str());
	send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端
}

//登录，登陆成功则...
void Server::LogIn(std::string userName, std::string password)
{
	std::string sendBufString = "";
	//检查该用户的用户名是否被注册过
	int userID = CheckExistence(userName);
	//没注册过则无法登录
	if (userID == NOT_EXIST)
	{
		sendBufString = "NOT_EXIST";
	}
	else
	{
		//若注册过则检查密码和用户名是否匹配
		if (userList[userID]->GetPassword() != password)
		{
			//密码不正确
			sendBufString = "ERROR";
		}
		else
		{
			//密码正确
			
			serverMutex.lock();
			//遍历服务器在与用户连接的端点，看是否有匹配的用户
			int size = serverEndpoints.size();
			bool isUserMatch = false;//标记用户是否有匹配的端点
			for (int i = 0; i < size && !isUserMatch; i++)
			{
				if (serverEndpoints[i]->GetUserID() == userID)//若有匹配的则代表该用户已经有可以通信的端点
				{
					isUserMatch = true;
					//检查该用户是否已经登陆
					if (serverEndpoints[i]->GetIsOnline())
					{
						sendBufString = "ALREADY_ONLINE";
					}
					else
					{
						//未登录，返回端口与该用户信息，允许用户登录
						int endpointPort = serverEndpoints[i]->GetPort();
						sendBufString = "ALLOWED";
						sendBufString += ' ' + std::to_string(endpointPort) + ' '
										+ std::to_string(userID) + ' ' + userList[userID]->GetUserInfo(false);
					}

				}
				else
					;
			}//end of for
			serverMutex.unlock();

			//没有匹配的要给该用户分配一个通信端点
			if (isUserMatch == false)
			{
				Endpoint* newEndpoint = new Endpoint(this, userID);
				int endpointPort = newEndpoint->InitEndpoint();//初始化并做socket的一些流程，成功返回端口号，失败返回-1
				if (endpointPort == -1)
				{
					//初始化失败,释放
					delete newEndpoint;
					newEndpoint = nullptr;
					sendBufString = "SERVER_ERROR";
				}
				else
				{
					//初始化成功，设置用户在线并添加端口到服务器
					std::lock_guard<std::mutex> lock(serverMutex);

					serverEndpoints.push_back(newEndpoint);
					sendBufString = "ALLOWED";
					sendBufString += ' ' + std::to_string(endpointPort) + ' '
									+ std::to_string(userID) + ' ' + userList[userID]->GetUserInfo(false);

					std::thread manageThread(&Server::ManageEndpoint, this, newEndpoint);
					manageThread.detach();
				}
			}
			else
				;

		}//end of else(密码正确)

	}// end of else（用户存在）

	strcpy(socketBuffer, sendBufString.c_str());
	send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端
}


//遍历用户表，检查某用户是否存在，存在返回该用户的ID，不存在返回NOT_EXIST(-1).
int Server::CheckExistence(std::string userName) const
{
	int userID = NOT_EXIST;
	bool isFound = false;
	int size = userList.size();
	for (int i = 0; (i < size) && (!isFound); i++)
	{
		if (userName == userList[i]->GetName())//如果用户名相同则存在，用户名是唯一的
		{
			isFound = true;
			userID = userList[i]->GetID();
		}
		else
			;
	}
	return userID;
}

//将用户数据从文件拷贝到服务器内存里
void Server::LoadUsersData()
{
	/*
		用户文件记录内容及数据类型如下：
		 ID	| 用户名	|  密码	| 获胜数	| 失败数	| 拥有宝可梦数n	|满级宝可梦数	| ……宝可梦ID(n个)……	|
		 int| string| string|  int	|  int	|	  int		|    int	|		int			|
	*/

	std::ifstream userDataFile;
	userDataFile.open(userFileLoc, std::ios::in);
	if (userDataFile.is_open())
	{
		while (userDataFile.peek() != EOF)
		{
			int userID, wins, loses, pokemonNum,fullLevelSum;//用户id，获胜数，失败数，拥有宝可梦数
			std::string userName, userPassword;//用户名，密码

			userDataFile >> userID >> userName >> userPassword >> wins >> loses >> pokemonNum >> fullLevelSum;//读入前六项内容

			//创建用户对象并加入用户表中
			User* curUser = new User(userID, userName, userPassword, wins, loses, fullLevelSum);
			userList.push_back(curUser);

			//根据拥有宝可梦数以及宝可梦ID载入拥有的宝可梦
			for (int i = 1; i <= pokemonNum; i++)
			{
				int pokemonID;
				userDataFile >> pokemonID;

				//从个体宝可梦表中读取宝可梦数据，并加到该用户背包中
				Pokemon* myPkm = individualPokemon[pokemonID];
				curUser->AddPokemonToBags(myPkm);
			}//end of for
			
		}//end of while
		
	}
	else
		;
}

//加载种族宝可梦文件到内存
void Server::LoadSpeciesPokemon()const
{
	/*
		文件记录内容及数据类型如下：
		 种族ID	|  名字	| 类型	| 主属性	| 基础经验值	| 属性提升值	| 生命值	| 攻击值	| 防御值	| 特攻值	| 特防值	| 速度值	| 招式ID（4个）
		  int	|string	|string	|string	|    int	|    int	|  int	|  int	|  int	|  int	|  int	|  int	|	int
	*/

	std::ifstream speciesFile;
	speciesFile.open(POKEMON_SPECIES_LOC, std::ios::in);
	if (speciesFile.is_open())
	{
		while (speciesFile.peek() != EOF)
		{
			int id, base, grow;
			int attr[6], move[4];
			std::string name, type, mainA;

			speciesFile >> id >> name >> type >> mainA >> base >> grow;
			for (int i = 0; i < 6; i++)// 生命值	| 攻击值	| 防御值	| 特攻值	| 特防值	| 速度值
			{
				speciesFile >> attr[i];
			}
			for (int i = 0; i < 4; i++)
			{
				speciesFile >> move[i];
			}

			Pokemon* curSpecies = new Pokemon(id, name, type, mainA, base, grow, attr, move);
			pokemonLibrary.push_back(curSpecies);
		}//end of while
	}//end of if
	else
		;
}

//加载个体宝可梦文件到本地
void Server::LoadIndividualPKM()const
{
	/*
		文件记录内容及数据类型如下：
		 个体ID	| 种族ID	|  等级	| 经验值	| 生命值	| 攻击值	| 防御值	| 特攻值	| 特防值	| 速度值	|(后6项为attr[6])
		  int	| int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|
	*/
	std::ifstream individualFile;
	individualFile.open(POKEMON_INDIVIDUAL_LOC, std::ios::in);
	if (individualFile.is_open())
	{
		while (individualFile.peek() != EOF)
		{
			int myID, speciesID, level, EXP, attr[6];

			individualFile >> myID >> speciesID >> level >> EXP;
			for (int i = 0; i < 6; i++)
			{
				individualFile >> attr[i];
			}

			Pokemon* individualPkm = new Pokemon(myID, speciesID, level, EXP, attr);
			individualPokemon.push_back(individualPkm);
		}
	}
	else
		;
}

void Server::SetUserIsOnline(int id, bool isonline)
{
	userList[id]->SetIsOnline(isonline);
	RewriteUserFile();
}

//返回某id用户的详细信息
std::string Server::GetUserInfo(int id) const
{
	/*
		返回用户信息记录内容及数据类型如下：（最后全转为string返回,中间用空格间隔）
		  用户数	| 用户ID	| 用户名	|  在线	| 获胜数	| 失败数	| 拥有宝可梦数n	|满级宝可梦数	| ……宝可梦ID(n个)……	|
		    int	|  int	| string|  bool	|  int	|  int	|	  int		|    int	|		int			|
	*/

	std::string info = "";
	info = "USER_INFO";
	info += ' ' + std::to_string(userList.size()) + ' ' + std::to_string(id);

	std::string leftInfo = userList[id]->GetUserInfo(false);//剩余部分的信息
	info += ' ' + leftInfo;

	return info;
}

//返回创建的个体宝可梦信息
std::string Server::CreateBattlePkm(int species, int lv, bool isUpgrateBattle) const
{
	Pokemon* battlePokemon = new Pokemon(*pokemonLibrary[species]);

	int individualID = individualPokemon.size();
	battlePokemon->InitIndividual(individualID, lv);
	battlePokemon->UpdateAttribute(lv);

	std::string info = battlePokemon->GetPokemonInfo();

	if (!isUpgrateBattle)//如果是决斗赛
	{
		//加入个体宝可梦表中
		individualPokemon.push_back(battlePokemon);

		//写入到个体宝可梦文件中
		std::ofstream individualFile;
		individualFile.open(POKEMON_INDIVIDUAL_LOC, std::ios::app | std::ios::out);
		if (individualFile.is_open())
		{
			individualFile << '\n' << info;
		}
		else;
	}
	else//如果是升级赛，用户不会有机会添加该宝可梦，该宝可梦可以释放了
	{
		delete battlePokemon;
		battlePokemon = nullptr;
	}

	return info;
}

void Server::RewriteUserFile()
{
	serverMutex.lock();

	std::ofstream userFile;
	userFile.open(userFileLoc, std::ios::out);

	int size = userList.size();
	for (int i = 0; i < size; i++)
	{
		if (i != 0)
		{
			userFile << '\n';
		}
		else
			;

		std::string info = std::to_string(i) + ' ' + userList[i]->GetUserInfo(true);
		userFile << info;
	}

	serverMutex.unlock();
}

void Server::RewritePkmFile()
{
	serverMutex.lock();

	std::ofstream pkmFile;
	pkmFile.open(POKEMON_INDIVIDUAL_LOC, std::ios::out);

	int size = individualPokemon.size();
	for (int i = 0; i < size; i++)
	{
		if (i != 0)
		{
			pkmFile << '\n';
		}
		else
			;

		std::string info = individualPokemon[i]->GetPokemonInfo();
		pkmFile << info;
	}

	serverMutex.unlock();
}

void Server::SetUserTimes(int userID, bool isWin)
{
	userList[userID]->SetBattleTimes(isWin);
}

std::string Server::UpdateUser(int userid, int pkmid, bool isAdd)
{
	std::string sendBufString = "";

	if (isAdd)//胜利获得宝可梦
	{
		Pokemon* winPokemon = individualPokemon[pkmid];
		userList[userid]->AddPokemonToBags(winPokemon);

		sendBufString = "DONE";
	}
	else//失败失去宝可梦
	{
		int leftPkmSum = userList[userid]->LosePokemon(pkmid);
		if (leftPkmSum == 0)//没有宝可梦了，随机发放一个初级的
		{
			serverMutex.lock();//涉及到改写文件
			userList[userid]->GainPokemon(-1, -1, 1);
			serverMutex.unlock();

			std::string info = individualPokemon.back()->GetPokemonInfo();

			sendBufString = "NEW_PKM";
			sendBufString += ' ' + info;
		}
		else
		{
			sendBufString = "DONE";
		}
	}

	return sendBufString;

}