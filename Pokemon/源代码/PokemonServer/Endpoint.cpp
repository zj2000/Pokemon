#include "Endpoint.h"
#include "server.h"
#include <chrono>
#pragma warning(disable:4996)

Endpoint::Endpoint(Server* server, int id)
{
	this->server = server;
	userId = id;
	endpointPort = 0;
	isRunning = false;
	isUserOnline = false;
	isWaitingConnect = false;
}

//初始化成功返回端口号，失败返回-1
int Endpoint::InitEndpoint()
{
	//初始化socket(不需要加载套接字库了)
	//1.创建套接字，地址族为Inernet网络地址，socket接受数据格式为流，协议是TCP
	listenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//此处为主动套接字
	if (listenFd == INVALID_SOCKET)
	{
		std::cout << userId << "号用户对应的服务端点套接字创建失败..." << std::endl;
		closesocket(listenFd);
		return -1;
	}
	else
		;

	//2.将ip地址和端口关联套接字结构，绑定套接字
	SOCKADDR_IN endpointAddr;//定义server发送和接收数据包的地址
	endpointAddr.sin_family = AF_INET;
	endpointAddr.sin_port = htons(0);//系统随机选择一个未被使用的端口号,bind之后若要获得此端口须使用getsockname函数
	endpointAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//系统自动填入本机IP地址
	
	int endpointAddrLen = sizeof(endpointAddr);
	int err = bind(listenFd, (SOCKADDR*)&endpointAddr, endpointAddrLen);//绑定套接字，此处仍为主动套接字
	if (err == SOCKET_ERROR)
	{
		std::cout << userId << "号用户对应的服务端点套接字绑定失败..." << std::endl;
		closesocket(listenFd);
		return -1;
	}
	else
		;
	//以端口号为0调用bind后，使用getsockname返回内核分配的本地端口号
	err = getsockname(listenFd, (SOCKADDR*)&endpointAddr, &endpointAddrLen);
	if (err == SOCKET_ERROR)
	{
		std::cout << userId << "号用户对应的服务端点套接字端口号获取失败..." << std::endl;
		closesocket(listenFd);
		return -1;
	}
	else
		;
	endpointPort = ntohs(endpointAddr.sin_port);//要转换为主机序

	//3.listenFd为被动套接字，进入监听模式等待连接请求
	err = listen(listenFd, MAX_QUEUE_LEN);//转为被动套接字
	if (err == SOCKET_ERROR)
	{
		std::cout << userId << "号用户对应的服务端点套接字转换至监听套接字失败..." << std::endl;
		closesocket(listenFd);
		return -1;
	}
	else
	{
		std::cout << userId << "号用户对应的服务端点在" << endpointPort << "号端口上运行" << std::endl;
		isRunning = true;//标记为正在运行
	}

	return endpointPort;
}

//运行服务端点，执行线程
void Endpoint::Run()
{
	while (isRunning)//如果服务端点正在运行就一直执行
	{
		isWaitingConnect = true;//开始等待用户连接
		isUserOnline = false;	//用户初始化为不在线

		std::thread timerThread(&Endpoint::WaitingTimer, this);
		std::thread communicateThread(&Endpoint::Communicate, this);
		timerThread.join();
		communicateThread.join();
	}
}

void Endpoint::WaitingTimer()
{
	std::unique_lock<std::mutex>lock(endpointMutex);

	//wait_for函数：返回值为用户的在线情况
	//在用户不在线时阻塞，阻塞后即使notify了，也只在用户在线时线程才会被唤醒
	
	//如果等待用户连接超过5分钟，用户仍不在线
	if (conditionVariable.wait_for(lock, std::chrono::minutes(5), [this] { return isUserOnline; }) == false)
	{
		//标记本段点不再运行，关闭套接字
		isRunning = false;
		closesocket(listenFd);
	}
	else//用户连接在线
	{
		std::cout << endpointPort << "号端口与用户上线登录" << std::endl;
	}

	server->SetUserIsOnline(userId, isUserOnline);
	isWaitingConnect = false;//无论是以上哪种情况都不再等待连接
}

void Endpoint::Communicate()
{
	//接受连接请求并重新创建一个套接字用于和客户机之间通信连接(listenFd继续监听工作)
	SOCKADDR_IN clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	connectFd = accept(listenFd, (SOCKADDR*)&clientAddr, &clientAddrLen);//阻塞直到等待接收到有客户端请求

	//此时已经收到用户的连接请求(接受失败也没关系，得先唤醒WaitingTimer线程)
	while (isWaitingConnect)
	{
		std::unique_lock<std::mutex>lock(endpointMutex);
		isUserOnline = true;//标记用户上线

		lock.unlock();//先解锁，后续要唤醒另一线程
		conditionVariable.notify_one();
		lock.lock();//重新上锁
	}
	//判断请求是否接收成功
	if (connectFd == INVALID_SOCKET)
	{
		return;
	}
	else
		;
	std::cout << endpointPort << "号端口与用户连接成功，可以开始接收请求" << std::endl;

	//接受请求成功，开始收发数据
	int recvBuf = recv(connectFd, socketBuffer, MAX_BUF_LEN, 0);//==0连接关闭；<0出错; >0收到的数据长度
	while (recvBuf != 0 && recvBuf != SOCKET_ERROR && isRunning)//用while持续接收不断开连接直到有情况
	{
		std::string sendBufString = "";

		std::vector<std::string> recvRequest = SplitBuf();
		int size = recvRequest.size();
		if (recvRequest[0] == "LOG_OUT")//收到的请求是退出登录,不需要回送信息
		{
			//停止运行本服务端点
			isRunning = false;
		}
		else if (recvRequest[0] == "SEE_OTHER_USERS" && size >= 2)//查看其它用户
		{
			//查看其他用户有两种：
			//一种是查看全部用户,返回的是用户总数和id为0的详细信息
			if (recvRequest[1] == "ALL")
			{
				sendBufString = server->GetUserInfo(0);
			}
			else//另一种是查看指定ID的用户的详细信息，返回的是id为指定ID的详细信息
			{
				if (recvRequest[1] == "ONE" && size >= 3)
				{
					int id = std::stoi(recvRequest[2]);
					sendBufString = server->GetUserInfo(id);
				}
				else
				{
					sendBufString = "RECV_ERROR";
				}
			}
			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端

		}
		else if (recvRequest[0] == "BATTLE" && size >= 4)//用户请求战斗
		{
			//接收用户选择的种族宝可梦标号和等级
			//生成一只个体宝可梦，写入个体宝可梦文件，返回给用户个体信息
			int pkmID = std::stoi(recvRequest[1]);
			int pkmLevel = std::stoi(recvRequest[2]);
			bool isUpgrateBattle = std::stoi(recvRequest[3]);

			sendBufString = "BATTLE_PKM";
			sendBufString += ' ' + server->CreateBattlePkm(pkmID, pkmLevel,isUpgrateBattle);

			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端

		}
		else if (recvRequest[0] == "LOSE_PKM" && size >= 2)
		{
			//接收用户选择要失去的个体宝可梦标号，修改用户文件中拥有宝可梦的信息（pokemonsum--）

			server->SetUserTimes(userId, false);

			int pkmID = std::stoi(recvRequest[1]);
			sendBufString = server->UpdateUser(userId,pkmID, false);
			server->RewriteUserFile();

			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端
		}
		else if (recvRequest[0] == "LOSE")
		{
			server->SetUserTimes(userId, false);
			server->RewriteUserFile();

			sendBufString = "DONE";
			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端
	
		}
		else if (recvRequest[0] == "WIN_PKM" && size >= 11)//可以获得新宝可梦的获胜
		{
			/*
				记录内容及数据类型如下：
				获得的宝可梦ID|胜利个体ID	|  等级	| 经验值	| 生命值	| 攻击值	| 防御值	| 特攻值	| 特防值	| 速度值	|
				  int		 |	int		|  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|
			*/

			server->SetUserTimes(userId, true);

			//接收用户获得的个体宝可梦标号，修改用户文件中拥有宝可梦的信息（pokemonsum++）：先修改表，然后重写文件
			int pkmID = std::stoi(recvRequest[1]);
			sendBufString = server->UpdateUser(userId,pkmID, true);
			server->RewriteUserFile();

			//更新个体宝可梦表中的信息并修改文件(exp/level...)
			int attr[8];
			for (int i = 0; i < 8; i++)
			{
				attr[i] = std::stoi(recvRequest[i + 3]);
			}

			int updatePkmID = std::stoi(recvRequest[2]);
			Pokemon* updatePkm = individualPokemon[updatePkmID];
			updatePkm->SetAttr(attr);
			server->RewritePkmFile();

			//发送反馈信息给客户端
			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);

		}
		else if (recvRequest[0] == "WIN" && size >= 10)//无获得新宝可梦的获胜
		{
			//更新个体宝可梦表中的信息并修改文件
			/*
				记录内容及数据类型如下：
				个体ID	|  等级	| 经验值	| 生命值	| 攻击值	| 防御值	| 特攻值	| 特防值	| 速度值	|
				int		|  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|
			*/
			server->SetUserTimes(userId, true);
			
			int attr[8];
			for (int i = 0; i < 8; i++)
			{
				attr[i] = std::stoi(recvRequest[i + 2]);
			}

			int pkmID = std::stoi(recvRequest[1]);
			Pokemon* updatePkm = individualPokemon[pkmID];
			updatePkm->SetAttr(attr);
			server->RewritePkmFile();

			sendBufString = "DONE";
			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端
		}
		else
		{
			//其余情况皆为无效请求
			sendBufString = "RECV_ERROR";
			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给客户端
		}

		//如果服务端点还在运行（客户端没有要求登出），就接收下一轮信息
		if (isRunning)
		{
			recvBuf = recv(connectFd, socketBuffer, MAX_BUF_LEN, 0);
		}
		else
			;
	}//end of while

	//标记用户下线
	server->SetUserIsOnline(userId, false);
	
	std::cout << endpointPort << "号端口用户退出登录";

	//？？？？
}

//分割接收到的数据，数据以空格' '做分隔符
std::vector<std::string> Endpoint::SplitBuf() const
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

int Endpoint::GetUserID() const
{
	return userId;
}

bool Endpoint::GetIsOnline() const
{
	return isUserOnline;
}

int Endpoint::GetPort() const
{
	return endpointPort;
}