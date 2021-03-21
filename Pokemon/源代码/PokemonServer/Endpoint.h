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

class Server;//需要前置声明，否则头文件会相互包含,该类定义在server.h中

class Endpoint//TCP连接的服务器端点，另一端点是客户端
{
	Server* server;//管理该endpoint的服务器server
	int endpointPort;	//端口
	SOCKET listenFd;	//监听套接字,可以接受来自客户端的连接请求
	SOCKET connectFd;	//已连接套接字,可以被用来与客户端进行通信
	char socketBuffer[1024];//数据缓存
	volatile bool isRunning;//本端点是否正在运行
	
	//所连接的用户
	int userId;//用户ID
	std::string userName;//用户名
	volatile bool isUserOnline;//本端点连接的用户是否在线

	std::mutex endpointMutex;//互斥锁
	std::condition_variable conditionVariable;//条件变量，阻塞线程直到通知恢复为止
	volatile bool isWaitingConnect;//是否正在等待用户连接


	//多线程相关函数
	void Communicate();//连接并通信
	void WaitingTimer();//管理计时器
	std::vector<std::string> SplitBuf() const;//分割接收到的数据

public:
	Endpoint(Server* server, int id);//构造

	int InitEndpoint();//初始化成功返回端口号，失败返回-1
	void Run();//运行本端口

	int GetUserID() const;//获得本端点连接的用户的ID
	bool GetIsOnline() const;//获得本端点连接的用户的在线情况
	int GetPort() const;//获得本端点的端口号
};


#endif // !ENDPOINT_H
