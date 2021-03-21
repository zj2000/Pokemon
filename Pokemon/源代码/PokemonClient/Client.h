#ifndef CLIENT_H
#define CLIENT_H

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <atlstr.h>
#include <ctime>

#include"User.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma warning(disable:4996)//解决不让使用strcpy

enum class State{start, mainUI,userUI,pokemonUI,battle, choose1, choose2,choose3,win,lose};//UI状态
enum class Mode{low,mid,high};

class Client
{
	//有关通信
	SOCKET sockFd; //客户端套接字
	char socketBuffer[1024];
	bool isWaitingMsg;//标记位，是否正在等待服务端的回信
	bool isRequestFailed;//请求是否失败
	int endpointPort;//负责本客户端的服务端点的端口号
	
	//有关用户
	std::string name;//本客户端用户名
	std::string password;//本客户端用户密码
	User* localUser;//本机用户
	User* curCheckUser;//当前正在查看的用户
	
	//有关界面
	int curUserPage;//当前查看的用户页
	int totalUserSum;//系统中的用户总数
	int curCheckPkmSeq;//当前查看的宝可梦在列表的下标号
	Type curCheckType;//当前查看的宝可梦的类别
	int curPkmPage;//当前查看的宝可梦页码
	int totalPkmSum;//当前查看的宝可梦背包所拥有的宝可梦总数
	std::string hintMsg;//界面提示信息
	State lastState;//上一个界面状态

	//有关战斗
	bool isUpgradeBattle;//是否是升级赛，true是升级赛，false是决斗赛
	Mode chooseMode;//选择挑战的模式：初级/中级/高级
	int myPkmID;//出战的宝可梦的种族ID
	int enemyID;//对手的种族id
	Pokemon* myBattlePkm;//出战的宝可梦
	Pokemon* enemyPkm;//对手宝可梦
	
	int gainedEXP;//战胜获得的经验值
	int loseSeq[3];//挑选的三只即将失去的宝可梦在总背包中的下标,为-1时代表不存在
	int loseID;//失去的宝可梦的个体ID
	int loseSequence;//失去的宝可梦的数组下标
	bool isChooseLose;//是否点击确认要失去的宝可梦
	bool isOkRet;//是否可以返回

	

	//文件加载
	void LoadSpeciesPokemon();//加载种族宝可梦到资料库
	void LoadIndividualPKM();//加载个体宝可梦库
	void LoadMoves();//加载招式库

	//有关战斗结束时的处理
	void HandleWinEvent();//战斗胜利
	void HandleLoseEvent();//战斗失败
	void SendLoseMsg();//发送给服务端战败信息

	std::vector<std::string> SplitBuf() const;//分割接收到的数据

	//与界面绘制有关
	void InitCanves();//初始化画布并装载图片资源
	void SetFont();//设置字体

	//从服务器返回的消息构造相关数据结构
	void CreateUser(std::vector<std::string> recvReponse, User& user);//构造用户
	void CreateEnemyPkm(std::vector<std::string> recvReponse);//构造战斗的敌方宝可梦

	//退出登录
	void LogOut();

	//点击事件
	void HandleCheckPokemon(int typeNum);//处理点击“查看宝可梦”的事件
	void HandleCheckUser(std::string mode,int id);//处理点击“查看用户”的事件

	//关于登录/注册时的输入
	std::string InputName(int mode);//输入用户名
	std::string InputPassword(int mode);//输入密码
	bool IsNameValid(char userName[40]) const;//检查用户名是否合法
	bool IsPasswordValid(char userPass[30]) const;//检查密码是否合法

	//图像
	bool IsBetween(int left, int right, int top, int bottom, int x, int y) const;

	//与服务器的连接
	bool ConnectHost(int port);


public:
	Client();
	~Client();
	bool InitWSA();

	//需要根据当前状态的函数
	void HandleClickEvent();//处理用户点击界面事件
	void RecvServerMsg();//根据当前所处的状态处理服务端返回的信息
	void Draw();//绘制画面

	//自动模拟战斗
	void AutoBattle();//自动出招对战

};

#endif // !CLIENT_H