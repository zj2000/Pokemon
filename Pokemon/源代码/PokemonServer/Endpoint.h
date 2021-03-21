#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <iostream>
#include <vector>

#pragma comment(lib,"Ws2_32.lib")

class Server;//��Ҫǰ������������ͷ�ļ����໥����,���ඨ����server.h��

class Endpoint//TCP���ӵķ������˵㣬��һ�˵��ǿͻ���
{
	Server* server;//�����endpoint�ķ�����server
	int endpointPort;	//�˿�
	SOCKET listenFd;	//�����׽���,���Խ������Կͻ��˵���������
	SOCKET connectFd;	//�������׽���,���Ա�������ͻ��˽���ͨ��
	char socketBuffer[1024];//���ݻ���
	volatile bool isRunning;//���˵��Ƿ���������
	
	//�����ӵ��û�
	int userId;//�û�ID
	std::string userName;//�û���
	volatile bool isUserOnline;//���˵����ӵ��û��Ƿ�����

	std::mutex endpointMutex;//������
	std::condition_variable conditionVariable;//���������������߳�ֱ��֪ͨ�ָ�Ϊֹ
	volatile bool isWaitingConnect;//�Ƿ����ڵȴ��û�����


	//���߳���غ���
	void Communicate();//���Ӳ�ͨ��
	void WaitingTimer();//�����ʱ��
	std::vector<std::string> SplitBuf() const;//�ָ���յ�������

public:
	Endpoint(Server* server, int id);//����

	int InitEndpoint();//��ʼ���ɹ����ض˿ںţ�ʧ�ܷ���-1
	void Run();//���б��˿�

	int GetUserID() const;//��ñ��˵����ӵ��û���ID
	bool GetIsOnline() const;//��ñ��˵����ӵ��û����������
	int GetPort() const;//��ñ��˵�Ķ˿ں�
};


#endif // !ENDPOINT_H
