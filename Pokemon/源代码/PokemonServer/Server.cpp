#include "server.h"
#include <conio.h>
#pragma warning(disable:4996)

std::vector<Pokemon*> pokemonLibrary;
std::vector<Pokemon*> individualPokemon;//���屦���Σ������û��ĺ�ϵͳ���ɵ�

Server::Server()
{
	userFileLoc = "./userData.dat";
	isRunning = false;

	listenFd = INVALID_SOCKET;
	connectFd = INVALID_SOCKET;
}

Server::~Server()
{
	//ɾ�����з���˵�
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

	//�����Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ,Ҫ��Ҫ��ѭ���жϣ�����
	WSACleanup(); 
}

//��ʼ�����������������У���2�����֣���ʼ���û������ļ�����ʼ��socket
bool Server::InitServer()
{
	//��ʼ���û������ļ�����һ�δ򿪷�����û�������ļ���Ҫ����һ��
	std::ofstream userDataFile;
	userDataFile.open(userFileLoc, std::ios::app | std::ios::out);//�ļ������ھʹ���һ��������Ҳ����ɾ��ԭ������
	if (userDataFile.is_open())
	{
		std::cout << "��/�����û������ļ��ɹ�!" << std::endl;
		userDataFile.close();
	}
	else
	{
		std::cout << "��/�����û������ļ�ʧ��..." << std::endl;
		return false;
	}
	//�����ļ����ڴ棨����˳���ܱ䣩
	LoadSpeciesPokemon();
	LoadIndividualPKM();
	LoadUsersData();

	//��ʼ��socket
	//1.�����׽��ֿ�
	WORD versionRequested = MAKEWORD(2, 2);//�汾��
	WSADATA wsaData;
	int err = WSAStartup(versionRequested, &wsaData);//ʹ��Socket֮ǰ�������,�����׽��ֿⷵ����Ϣ��err
	if (err != 0)
	{
		std::cout << "�����׽��ֿ�ʧ��..." << std::endl;
		return false;
	}
	else
	{
		std::cout << "�����׽��ֿ�ɹ���" << std::endl;
	}
	//2.����׽��ֿ�汾��
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wHighVersion) != 2) 
	{
		std::cout << "�׽��ֿ�汾�Ų���..." << std::endl;
		WSACleanup();
		return false;
	}
	else 
	{
		std::cout << "�׽��ֿ�汾��ȷ��" << std::endl;
	}

	//3.�����׽��֣���ַ��ΪInernet�����ַ��socket�������ݸ�ʽΪ����Э����TCP
	listenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�˴�Ϊ�����׽���
	if (listenFd == INVALID_SOCKET)
	{
		std::cout << "�����׽���ʧ��..." << std::endl;
		closesocket(listenFd);
		WSACleanup();
		return false;
	}
	else
	{
		std::cout << "�����׽��ֳɹ���" << std::endl;
	}

	//4.��ip��ַ�Ͷ˿ڹ����׽��ֽṹ�����׽���
	SOCKADDR_IN serverAddr;//����server���ͺͽ������ݰ��ĵ�ַ
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);//�����ֽ���,�˿ںŹ̶���SERVER_PORT��8000��
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//�����ֽ���ϵͳ�Զ����뱾��IP��ַ
	
	err = bind(listenFd, (SOCKADDR*)&serverAddr, sizeof(serverAddr));//���׽��֣��˴���Ϊ�����׽���
	if (err == SOCKET_ERROR)
	{
		std::cout << "�׽��ְ�ʧ��..." << std::endl;
		closesocket(listenFd);
		WSACleanup();
		return false;
	}
	else
	{
		std::cout << "�׽��ְ󶨳ɹ���" << std::endl;
	}

	//5.����listenFdΪ�����׽��֣�listen����ִ��֮ǰ���׽��ֶ��������׽��֣����������ģʽ�ȴ���������
	err = listen(listenFd, MAX_QUEUE_LEN);//תΪ�����׽���
	if (err == SOCKET_ERROR)
	{
		std::cout << "ת���������׽���ʧ��..." << std::endl;
		closesocket(listenFd);
		WSACleanup();
		return false;
	}
	else 
	{
		isRunning = true;
		std::cout << "ת���������׽��ֳɹ���" << std::endl;
		std::cout << "��������" << SERVER_PORT << "�Ŷ˿�������...�����������ֹͣ����������..." << std::endl;
	}

	return true;
}

//���з�������ִ���߳��Լ����߳�ִ����Ϻ��������Ӷ˵�,���رշ�����
void Server::Run()
{
	//ִ���߳�
	std::thread communicateThread(&Server::Communicate, this);
	std::thread endThread(&Server::End, this);
	communicateThread.join();
	endThread.join();

	//�߳�ִ�н����������������Ӷ˵�
	serverMutex.lock();
	int size = static_cast<int>(serverEndpoints.size());
	for (int i = size - 1; i >= 0; i--)
	{
		delete serverEndpoints[i];
		serverEndpoints[i] = nullptr;
		serverEndpoints.pop_back();
	}
	serverMutex.unlock();

	closesocket(listenFd);//�ر��׽���
	//closesocket(connectFd);
	WSACleanup();//�ͷ�dll��Դ
	std::cout << std::endl << "��������ֹͣ����...";

}

void Server::Communicate()
{
	while (isRunning)
	{
		//���������������´���һ���׽������ںͿͻ���֮��ͨ������(listenFd������������)
		SOCKADDR_IN clientAddr;
		int clientAddrLen = sizeof(clientAddr);
		connectFd = accept(listenFd, (SOCKADDR*)&clientAddr, &clientAddrLen);//����ֱ���ȴ����յ��пͻ�������
		if (connectFd == INVALID_SOCKET)//����ʧ��
		{
			if (isRunning)
			{
				std::cout << "���ӵ��ͻ���ʧ��..." << std::endl;
			}
			else
				;

			closesocket(connectFd);
		}
		else//���ӳɹ�
		{
			std::cout << "���ӵ��ͻ��˳ɹ����ͻ��˵�ַΪ����"
						<< inet_ntoa(clientAddr.sin_addr) <<" : "
						<< clientAddr.sin_port << ")"<< std::endl;

			//��ʼ�շ�����
			std::string sendBufString = "";
			memset(socketBuffer, 0, MAX_BUF_LEN);
			int recvBuf = recv(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ﲻ�ó�������
			if (recvBuf == 0 || recvBuf == SOCKET_ERROR)
			{
				std::cout << "�����������ݽ���ʧ��..." << std::endl;

				sendBufString = "RECV_ERROR";
				strcpy(socketBuffer, sendBufString.c_str());
				send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���

			}
			else
			{
				std::vector<std::string> recvRequest = SplitBuf();

				if (recvRequest.size() < 3)//����С��3�Ŀ϶�����Ч������
				{
					std::cout << "��Ч������" << std::endl;

					sendBufString = "RECV_ERROR";
					strcpy(socketBuffer, sendBufString.c_str());
					send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���
				}
				else if (recvRequest[0] == "SIGN_UP")//�յ���������ע��
				{
					std::cout << "��ʼ����ע������" << std::endl;
					SignUp(recvRequest[1], recvRequest[2]);
				}
				else if (recvRequest[0] == "LOG_IN")//�յ��������ǵ�¼
				{
					std::cout << "��ʼ�����¼����" << std::endl;
					LogIn(recvRequest[1], recvRequest[2]);
				}
				else//ʣ���Ҳ����Ч������
				{
					std::cout << "��Ч������" << std::endl;
					sendBufString = "RECV_ERROR";
					strcpy(socketBuffer, sendBufString.c_str());
					send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���
				}

				closesocket(connectFd);//����һ�ξͿ��ԶϿ�������

			}//end of else

		}//end of else(���ӳɹ�)
	}//end of while(isRunning)
	
}

void Server::End()
{
	_getch();//��������˳�������������ֱ�����ַ�����ʱ�Ż᷵��
	
	isRunning = false;//��־������ֹͣ����

	closesocket(listenFd);//ֹͣ����
}

void Server::ManageEndpoint(Endpoint* endpoint)
{
	endpoint->Run();//�ø÷���˵㿪ʼ���У�����ʱ�˴����������н����Ż�����ִ�д���

	//�÷���˵����н�������ʱ�ӷ�����������
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

//�ָ���յ������ݣ������Կո�' '���ָ���
std::vector<std::string> Server::SplitBuf() const
{
	std::vector<std::string> result;
	std::string splitData = "";//�ݴ�õ����ַ���

	int curHandleLoc = 0;//��ǰ�������ַ����ַ����е�λ��
	while (socketBuffer[curHandleLoc] != '\0')//����ַ���û�д������
	{
		if (socketBuffer[curHandleLoc] != ' ')//�����ǰ������ַ����Ƿָ���' '�������ƴ��
		{
			splitData += socketBuffer[curHandleLoc];
			curHandleLoc++;
		}
		else if (curHandleLoc != 0)//����������Ƿָ����ҵ��Ҳ��Ǵ����һ��λ��
		{
			curHandleLoc++;//�������ַ�

			result.push_back(splitData);//���浱ǰ�õ������ݶ�

			splitData = "";//��ʼ��
		}
		else
		{
			curHandleLoc++;//�������ַ�
		}
	}
	result.push_back(splitData);//���һ�����ݶβ����Էָ�����β�ģ���ѭ������û���浽�����

	return result;
}

//ע��,ע��ɹ������û�����ר�Ÿ�����û��ķ���˿ںź͸��û���ID��ʧ�����͵Ķ˿ں�Ϊ0
void Server::SignUp(std::string userName, std::string password)
{
	std::string sendBufString = "";

	//�����û����û����Ƿ�ע���,ûע���������ע�ᣬ����ܾ�
	int userID = CheckExistence(userName);
	if (userID == NOT_EXIST)
	{
		//����ע��
		userID = userList.size();//����ID��
		sendBufString = "ALLOW_SIGNUP";
		std::cout << "�û�ע��ɹ�" << std::endl;

		//��Ӹ��û����û�����
		User* newUser = new User(userID, userName, password);
		userList.push_back(newUser);

		//�����û�������ֻ��ͬ����ı�����
		int first = -1, second = -1, third = -1;//��õĵ�һֻ�͵ڶ�ֻ�����ε�����ID
		serverMutex.lock();
		first = newUser->GainPokemon(first, second, 1);
		second = newUser->GainPokemon(first, second, 1);
		third = newUser->GainPokemon(first, second, 1);
		serverMutex.unlock();

		//��Ӹ��û����û��ļ���
		serverMutex.lock();
		newUser->AppendUserFile(userFileLoc);
		serverMutex.unlock();
	}
	else
	{
		//�ܾ�ע������
		sendBufString = "EXIST";
		std::cout << "�û�ע��ʧ��" << std::endl;
	}
	
	strcpy(socketBuffer, sendBufString.c_str());
	send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���
}

//��¼����½�ɹ���...
void Server::LogIn(std::string userName, std::string password)
{
	std::string sendBufString = "";
	//�����û����û����Ƿ�ע���
	int userID = CheckExistence(userName);
	//ûע������޷���¼
	if (userID == NOT_EXIST)
	{
		sendBufString = "NOT_EXIST";
	}
	else
	{
		//��ע�������������û����Ƿ�ƥ��
		if (userList[userID]->GetPassword() != password)
		{
			//���벻��ȷ
			sendBufString = "ERROR";
		}
		else
		{
			//������ȷ
			
			serverMutex.lock();
			//���������������û����ӵĶ˵㣬���Ƿ���ƥ����û�
			int size = serverEndpoints.size();
			bool isUserMatch = false;//����û��Ƿ���ƥ��Ķ˵�
			for (int i = 0; i < size && !isUserMatch; i++)
			{
				if (serverEndpoints[i]->GetUserID() == userID)//����ƥ����������û��Ѿ��п���ͨ�ŵĶ˵�
				{
					isUserMatch = true;
					//�����û��Ƿ��Ѿ���½
					if (serverEndpoints[i]->GetIsOnline())
					{
						sendBufString = "ALREADY_ONLINE";
					}
					else
					{
						//δ��¼�����ض˿�����û���Ϣ�������û���¼
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

			//û��ƥ���Ҫ�����û�����һ��ͨ�Ŷ˵�
			if (isUserMatch == false)
			{
				Endpoint* newEndpoint = new Endpoint(this, userID);
				int endpointPort = newEndpoint->InitEndpoint();//��ʼ������socket��һЩ���̣��ɹ����ض˿ںţ�ʧ�ܷ���-1
				if (endpointPort == -1)
				{
					//��ʼ��ʧ��,�ͷ�
					delete newEndpoint;
					newEndpoint = nullptr;
					sendBufString = "SERVER_ERROR";
				}
				else
				{
					//��ʼ���ɹ��������û����߲���Ӷ˿ڵ�������
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

		}//end of else(������ȷ)

	}// end of else���û����ڣ�

	strcpy(socketBuffer, sendBufString.c_str());
	send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���
}


//�����û������ĳ�û��Ƿ���ڣ����ڷ��ظ��û���ID�������ڷ���NOT_EXIST(-1).
int Server::CheckExistence(std::string userName) const
{
	int userID = NOT_EXIST;
	bool isFound = false;
	int size = userList.size();
	for (int i = 0; (i < size) && (!isFound); i++)
	{
		if (userName == userList[i]->GetName())//����û�����ͬ����ڣ��û�����Ψһ��
		{
			isFound = true;
			userID = userList[i]->GetID();
		}
		else
			;
	}
	return userID;
}

//���û����ݴ��ļ��������������ڴ���
void Server::LoadUsersData()
{
	/*
		�û��ļ���¼���ݼ������������£�
		 ID	| �û���	|  ����	| ��ʤ��	| ʧ����	| ӵ�б�������n	|������������	| ����������ID(n��)����	|
		 int| string| string|  int	|  int	|	  int		|    int	|		int			|
	*/

	std::ifstream userDataFile;
	userDataFile.open(userFileLoc, std::ios::in);
	if (userDataFile.is_open())
	{
		while (userDataFile.peek() != EOF)
		{
			int userID, wins, loses, pokemonNum,fullLevelSum;//�û�id����ʤ����ʧ������ӵ�б�������
			std::string userName, userPassword;//�û���������

			userDataFile >> userID >> userName >> userPassword >> wins >> loses >> pokemonNum >> fullLevelSum;//����ǰ��������

			//�����û����󲢼����û�����
			User* curUser = new User(userID, userName, userPassword, wins, loses, fullLevelSum);
			userList.push_back(curUser);

			//����ӵ�б��������Լ�������ID����ӵ�еı�����
			for (int i = 1; i <= pokemonNum; i++)
			{
				int pokemonID;
				userDataFile >> pokemonID;

				//�Ӹ��屦���α��ж�ȡ���������ݣ����ӵ����û�������
				Pokemon* myPkm = individualPokemon[pokemonID];
				curUser->AddPokemonToBags(myPkm);
			}//end of for
			
		}//end of while
		
	}
	else
		;
}

//�������屦�����ļ����ڴ�
void Server::LoadSpeciesPokemon()const
{
	/*
		�ļ���¼���ݼ������������£�
		 ����ID	|  ����	| ����	| ������	| ��������ֵ	| ��������ֵ	| ����ֵ	| ����ֵ	| ����ֵ	| �ع�ֵ	| �ط�ֵ	| �ٶ�ֵ	| ��ʽID��4����
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
			for (int i = 0; i < 6; i++)// ����ֵ	| ����ֵ	| ����ֵ	| �ع�ֵ	| �ط�ֵ	| �ٶ�ֵ
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

//���ظ��屦�����ļ�������
void Server::LoadIndividualPKM()const
{
	/*
		�ļ���¼���ݼ������������£�
		 ����ID	| ����ID	|  �ȼ�	| ����ֵ	| ����ֵ	| ����ֵ	| ����ֵ	| �ع�ֵ	| �ط�ֵ	| �ٶ�ֵ	|(��6��Ϊattr[6])
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

//����ĳid�û�����ϸ��Ϣ
std::string Server::GetUserInfo(int id) const
{
	/*
		�����û���Ϣ��¼���ݼ������������£������ȫתΪstring����,�м��ÿո�����
		  �û���	| �û�ID	| �û���	|  ����	| ��ʤ��	| ʧ����	| ӵ�б�������n	|������������	| ����������ID(n��)����	|
		    int	|  int	| string|  bool	|  int	|  int	|	  int		|    int	|		int			|
	*/

	std::string info = "";
	info = "USER_INFO";
	info += ' ' + std::to_string(userList.size()) + ' ' + std::to_string(id);

	std::string leftInfo = userList[id]->GetUserInfo(false);//ʣ�ಿ�ֵ���Ϣ
	info += ' ' + leftInfo;

	return info;
}

//���ش����ĸ��屦������Ϣ
std::string Server::CreateBattlePkm(int species, int lv, bool isUpgrateBattle) const
{
	Pokemon* battlePokemon = new Pokemon(*pokemonLibrary[species]);

	int individualID = individualPokemon.size();
	battlePokemon->InitIndividual(individualID, lv);
	battlePokemon->UpdateAttribute(lv);

	std::string info = battlePokemon->GetPokemonInfo();

	if (!isUpgrateBattle)//����Ǿ�����
	{
		//������屦���α���
		individualPokemon.push_back(battlePokemon);

		//д�뵽���屦�����ļ���
		std::ofstream individualFile;
		individualFile.open(POKEMON_INDIVIDUAL_LOC, std::ios::app | std::ios::out);
		if (individualFile.is_open())
		{
			individualFile << '\n' << info;
		}
		else;
	}
	else//��������������û������л�����Ӹñ����Σ��ñ����ο����ͷ���
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

	if (isAdd)//ʤ����ñ�����
	{
		Pokemon* winPokemon = individualPokemon[pkmid];
		userList[userid]->AddPokemonToBags(winPokemon);

		sendBufString = "DONE";
	}
	else//ʧ��ʧȥ������
	{
		int leftPkmSum = userList[userid]->LosePokemon(pkmid);
		if (leftPkmSum == 0)//û�б������ˣ��������һ��������
		{
			serverMutex.lock();//�漰����д�ļ�
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