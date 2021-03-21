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

//��ʼ���ɹ����ض˿ںţ�ʧ�ܷ���-1
int Endpoint::InitEndpoint()
{
	//��ʼ��socket(����Ҫ�����׽��ֿ���)
	//1.�����׽��֣���ַ��ΪInernet�����ַ��socket�������ݸ�ʽΪ����Э����TCP
	listenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�˴�Ϊ�����׽���
	if (listenFd == INVALID_SOCKET)
	{
		std::cout << userId << "���û���Ӧ�ķ���˵��׽��ִ���ʧ��..." << std::endl;
		closesocket(listenFd);
		return -1;
	}
	else
		;

	//2.��ip��ַ�Ͷ˿ڹ����׽��ֽṹ�����׽���
	SOCKADDR_IN endpointAddr;//����server���ͺͽ������ݰ��ĵ�ַ
	endpointAddr.sin_family = AF_INET;
	endpointAddr.sin_port = htons(0);//ϵͳ���ѡ��һ��δ��ʹ�õĶ˿ں�,bind֮����Ҫ��ô˶˿���ʹ��getsockname����
	endpointAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//ϵͳ�Զ����뱾��IP��ַ
	
	int endpointAddrLen = sizeof(endpointAddr);
	int err = bind(listenFd, (SOCKADDR*)&endpointAddr, endpointAddrLen);//���׽��֣��˴���Ϊ�����׽���
	if (err == SOCKET_ERROR)
	{
		std::cout << userId << "���û���Ӧ�ķ���˵��׽��ְ�ʧ��..." << std::endl;
		closesocket(listenFd);
		return -1;
	}
	else
		;
	//�Զ˿ں�Ϊ0����bind��ʹ��getsockname�����ں˷���ı��ض˿ں�
	err = getsockname(listenFd, (SOCKADDR*)&endpointAddr, &endpointAddrLen);
	if (err == SOCKET_ERROR)
	{
		std::cout << userId << "���û���Ӧ�ķ���˵��׽��ֶ˿ںŻ�ȡʧ��..." << std::endl;
		closesocket(listenFd);
		return -1;
	}
	else
		;
	endpointPort = ntohs(endpointAddr.sin_port);//Ҫת��Ϊ������

	//3.listenFdΪ�����׽��֣��������ģʽ�ȴ���������
	err = listen(listenFd, MAX_QUEUE_LEN);//תΪ�����׽���
	if (err == SOCKET_ERROR)
	{
		std::cout << userId << "���û���Ӧ�ķ���˵��׽���ת���������׽���ʧ��..." << std::endl;
		closesocket(listenFd);
		return -1;
	}
	else
	{
		std::cout << userId << "���û���Ӧ�ķ���˵���" << endpointPort << "�Ŷ˿�������" << std::endl;
		isRunning = true;//���Ϊ��������
	}

	return endpointPort;
}

//���з���˵㣬ִ���߳�
void Endpoint::Run()
{
	while (isRunning)//�������˵��������о�һֱִ��
	{
		isWaitingConnect = true;//��ʼ�ȴ��û�����
		isUserOnline = false;	//�û���ʼ��Ϊ������

		std::thread timerThread(&Endpoint::WaitingTimer, this);
		std::thread communicateThread(&Endpoint::Communicate, this);
		timerThread.join();
		communicateThread.join();
	}
}

void Endpoint::WaitingTimer()
{
	std::unique_lock<std::mutex>lock(endpointMutex);

	//wait_for����������ֵΪ�û����������
	//���û�������ʱ������������ʹnotify�ˣ�Ҳֻ���û�����ʱ�̲߳Żᱻ����
	
	//����ȴ��û����ӳ���5���ӣ��û��Բ�����
	if (conditionVariable.wait_for(lock, std::chrono::minutes(5), [this] { return isUserOnline; }) == false)
	{
		//��Ǳ��ε㲻�����У��ر��׽���
		isRunning = false;
		closesocket(listenFd);
	}
	else//�û���������
	{
		std::cout << endpointPort << "�Ŷ˿����û����ߵ�¼" << std::endl;
	}

	server->SetUserIsOnline(userId, isUserOnline);
	isWaitingConnect = false;//����������������������ٵȴ�����
}

void Endpoint::Communicate()
{
	//���������������´���һ���׽������ںͿͻ���֮��ͨ������(listenFd������������)
	SOCKADDR_IN clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	connectFd = accept(listenFd, (SOCKADDR*)&clientAddr, &clientAddrLen);//����ֱ���ȴ����յ��пͻ�������

	//��ʱ�Ѿ��յ��û�����������(����ʧ��Ҳû��ϵ�����Ȼ���WaitingTimer�߳�)
	while (isWaitingConnect)
	{
		std::unique_lock<std::mutex>lock(endpointMutex);
		isUserOnline = true;//����û�����

		lock.unlock();//�Ƚ���������Ҫ������һ�߳�
		conditionVariable.notify_one();
		lock.lock();//��������
	}
	//�ж������Ƿ���ճɹ�
	if (connectFd == INVALID_SOCKET)
	{
		return;
	}
	else
		;
	std::cout << endpointPort << "�Ŷ˿����û����ӳɹ������Կ�ʼ��������" << std::endl;

	//��������ɹ�����ʼ�շ�����
	int recvBuf = recv(connectFd, socketBuffer, MAX_BUF_LEN, 0);//==0���ӹرգ�<0����; >0�յ������ݳ���
	while (recvBuf != 0 && recvBuf != SOCKET_ERROR && isRunning)//��while�������ղ��Ͽ�����ֱ�������
	{
		std::string sendBufString = "";

		std::vector<std::string> recvRequest = SplitBuf();
		int size = recvRequest.size();
		if (recvRequest[0] == "LOG_OUT")//�յ����������˳���¼,����Ҫ������Ϣ
		{
			//ֹͣ���б�����˵�
			isRunning = false;
		}
		else if (recvRequest[0] == "SEE_OTHER_USERS" && size >= 2)//�鿴�����û�
		{
			//�鿴�����û������֣�
			//һ���ǲ鿴ȫ���û�,���ص����û�������idΪ0����ϸ��Ϣ
			if (recvRequest[1] == "ALL")
			{
				sendBufString = server->GetUserInfo(0);
			}
			else//��һ���ǲ鿴ָ��ID���û�����ϸ��Ϣ�����ص���idΪָ��ID����ϸ��Ϣ
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
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���

		}
		else if (recvRequest[0] == "BATTLE" && size >= 4)//�û�����ս��
		{
			//�����û�ѡ������屦���α�ź͵ȼ�
			//����һֻ���屦���Σ�д����屦�����ļ������ظ��û�������Ϣ
			int pkmID = std::stoi(recvRequest[1]);
			int pkmLevel = std::stoi(recvRequest[2]);
			bool isUpgrateBattle = std::stoi(recvRequest[3]);

			sendBufString = "BATTLE_PKM";
			sendBufString += ' ' + server->CreateBattlePkm(pkmID, pkmLevel,isUpgrateBattle);

			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���

		}
		else if (recvRequest[0] == "LOSE_PKM" && size >= 2)
		{
			//�����û�ѡ��Ҫʧȥ�ĸ��屦���α�ţ��޸��û��ļ���ӵ�б����ε���Ϣ��pokemonsum--��

			server->SetUserTimes(userId, false);

			int pkmID = std::stoi(recvRequest[1]);
			sendBufString = server->UpdateUser(userId,pkmID, false);
			server->RewriteUserFile();

			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���
		}
		else if (recvRequest[0] == "LOSE")
		{
			server->SetUserTimes(userId, false);
			server->RewriteUserFile();

			sendBufString = "DONE";
			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���
	
		}
		else if (recvRequest[0] == "WIN_PKM" && size >= 11)//���Ի���±����εĻ�ʤ
		{
			/*
				��¼���ݼ������������£�
				��õı�����ID|ʤ������ID	|  �ȼ�	| ����ֵ	| ����ֵ	| ����ֵ	| ����ֵ	| �ع�ֵ	| �ط�ֵ	| �ٶ�ֵ	|
				  int		 |	int		|  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|
			*/

			server->SetUserTimes(userId, true);

			//�����û���õĸ��屦���α�ţ��޸��û��ļ���ӵ�б����ε���Ϣ��pokemonsum++�������޸ı�Ȼ����д�ļ�
			int pkmID = std::stoi(recvRequest[1]);
			sendBufString = server->UpdateUser(userId,pkmID, true);
			server->RewriteUserFile();

			//���¸��屦���α��е���Ϣ���޸��ļ�(exp/level...)
			int attr[8];
			for (int i = 0; i < 8; i++)
			{
				attr[i] = std::stoi(recvRequest[i + 3]);
			}

			int updatePkmID = std::stoi(recvRequest[2]);
			Pokemon* updatePkm = individualPokemon[updatePkmID];
			updatePkm->SetAttr(attr);
			server->RewritePkmFile();

			//���ͷ�����Ϣ���ͻ���
			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);

		}
		else if (recvRequest[0] == "WIN" && size >= 10)//�޻���±����εĻ�ʤ
		{
			//���¸��屦���α��е���Ϣ���޸��ļ�
			/*
				��¼���ݼ������������£�
				����ID	|  �ȼ�	| ����ֵ	| ����ֵ	| ����ֵ	| ����ֵ	| �ع�ֵ	| �ط�ֵ	| �ٶ�ֵ	|
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
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���
		}
		else
		{
			//���������Ϊ��Ч����
			sendBufString = "RECV_ERROR";
			strcpy(socketBuffer, sendBufString.c_str());
			send(connectFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ���ͻ���
		}

		//�������˵㻹�����У��ͻ���û��Ҫ��ǳ������ͽ�����һ����Ϣ
		if (isRunning)
		{
			recvBuf = recv(connectFd, socketBuffer, MAX_BUF_LEN, 0);
		}
		else
			;
	}//end of while

	//����û�����
	server->SetUserIsOnline(userId, false);
	
	std::cout << endpointPort << "�Ŷ˿��û��˳���¼";

	//��������
}

//�ָ���յ������ݣ������Կո�' '���ָ���
std::vector<std::string> Endpoint::SplitBuf() const
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