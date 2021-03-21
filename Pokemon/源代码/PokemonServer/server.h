#ifndef SERVER_H
#define SERVER_H

#include "Endpoint.h"
#include "User.h"

#include <fstream>


const int SERVER_PORT = 8000;//服务器端口号
const int MAX_QUEUE_LEN = 4;//队列最大长度，超过这个长度，内核开始拒绝连接请求
const int MAX_BUF_LEN = 1024;//socket缓存的最大长度
const int NOT_EXIST = -1;
const std::string POKEMON_SPECIES_LOC = "./res/pokemon.dat";//只读文件不许修改
const std::string POKEMON_INDIVIDUAL_LOC = "./res/pokemonIndividual.dat";
//const std::string MOVE_LOC = "./res/moves.dat";//只读文件不许修改


extern std::vector<Pokemon*> pokemonLibrary;
extern std::vector<Pokemon*> individualPokemon;//个体宝可梦，包括用户的和系统生成的

class Server
{
	//socket相关
	SOCKET listenFd;	//监听套接字（本质上是文件描述符）,可以接受来自客户端的连接请求
	SOCKET connectFd;	//已连接套接字（本质上是文件描述符）,可以被用来与客户端进行通信
	char socketBuffer[1024];//数据缓存
	
	volatile bool isRunning;//服务器是否正在工作

	//用户数据相关
	std::string userFileLoc;//用户数据文件地址，保存用户名、密码及拥有宝可梦等信息
	std::vector<User*> userList;//用户表

	std::vector<Endpoint*> serverEndpoints;//用于实现多用户登录问题，TCP连接的服务器端点

	std::mutex serverMutex;		//互斥锁，多线程，用来保护数据


	//以下两种功能由客户端连接到服务器主端口（SERVER_PORT）完成
	void SignUp(std::string userName, std::string password);//注册
	void LogIn(std::string userName, std::string password);//登录
	int CheckExistence(std::string userName) const;//检查某用户名是否存在
	

	
	Server(const Server&) = delete;				//禁用拷贝构造函数,只允许有一个服务器
	Server& operator=(const Server&) = delete;	//禁用拷贝赋值操作符,只允许有一个服务器

	//与线程有关的函数
	void Communicate();//连接并通信
	void End();//终止
	void ManageEndpoint(Endpoint* endpoint);//管理endpoint，监视和销毁
	std::vector<std::string> SplitBuf() const;//分隔收到的数据

	//有关文件读写
	void LoadUsersData();//加载文件内容到内存
	void LoadSpeciesPokemon()const;//加载种族宝可梦到资料库
	void LoadIndividualPKM()const;//加载个体宝可梦到内存
	


public:
	Server();//构造
	~Server();//析构

	bool InitServer();//初始化并启动服务器，成功则返回true，失败返回false
	void Run();//运行服务器：执行线程以及在线程执行完毕后销毁连接端点,并关闭服务器

	void SetUserIsOnline(int id, bool isOnline);//标记用户是否在线
	std::string GetUserInfo(int id) const;//返回某id的详细信息

	std::string CreateBattlePkm(int species, int lv,bool isUpgrateBattle)const;//给客户端构造一个对手并返回

	void RewriteUserFile();//重写文件内容
	void RewritePkmFile();//重写个体宝可梦文件

	std::string UpdateUser(int userid, int pkmid, bool isAdd);//更新用户信息
	void SetUserTimes(int userID,bool isWin);//更新用户战斗信息
};

#endif // !SERVER_H