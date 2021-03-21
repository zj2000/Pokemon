#include "MainHeader.h"

IMAGE imgStart;
IMAGE imgMainUI;
IMAGE imgSeeUser;
IMAGE imgSeePokemon;
IMAGE imgBattle;
IMAGE imgChoose[5];

IMAGE imgWaiting;
IMAGE imgFailed;
IMAGE imgWin[2];
IMAGE imgLose[2];
IMAGE imgList[2];

IMAGE imgFront[48];//小精灵战斗时显示的图（前方）,原图
IMAGE imgFrontCover[48];//小精灵战斗时显示的图（前方）,掩码图
IMAGE imgBack[48];//小精灵战斗时显示的图（后方）,原图
IMAGE imgBackCover[48];//小精灵战斗时显示的图（后方）,掩码图

LOGFONT font;//字体

extern Weather battleWeather;

Client::Client()
{
	GUIState = State::start;
	isWaitingMsg = false;

	curUserPage = 0;
	curPkmPage = 0;

	name = "";
	password = "";
	hintMsg = "";

	isOkRet = false;

	localUser = new User();
	curCheckUser = new User();

	//加载文件资源到本地
	LoadMoves();
	LoadSpeciesPokemon();
	LoadIndividualPKM();

	//初始化界面绘制相关
	InitCanves();
}

Client::~Client()
{
	delete localUser;
	delete curCheckUser;
	localUser = nullptr;
	curCheckUser = nullptr;

	closesocket(sockFd);

	WSACleanup();
}

//初始化画布并装载图片资源
void Client::InitCanves()
{
	//加载图片资源,位置？？？
	loadimage(&imgStart, _T("./res/images/start.png"), 660, 600);
	loadimage(&imgMainUI, _T("./res/images/mainUI.png"), 660, 600);
	loadimage(&imgSeeUser, _T("./res/images/user.png"), 660, 600);
	loadimage(&imgSeePokemon, _T("./res/images/pokemon.png"), 660, 600);
	loadimage(&imgBattle, _T("./res/images/battle.png"), 660, 600);
	loadimage(&imgChoose[0], _T("./res/images/choose0.png"), 660, 600);
	loadimage(&imgChoose[1], _T("./res/images/choose1.png"), 660, 600);
	loadimage(&imgChoose[2], _T("./res/images/choose2.png"), 660, 600);
	loadimage(&imgChoose[3], _T("./res/images/choose3.png"), 660, 600);
	loadimage(&imgChoose[4], _T("./res/images/choose4.png"), 660, 600);

	loadimage(&imgWaiting, _T("./res/images/waiting.png"), 201, 57);
	loadimage(&imgFailed, _T("./res/images/fail.png"), 201, 57);

	loadimage(&imgWin[0], _T("./res/images/win0.png"), 660, 600);
	loadimage(&imgWin[1], _T("./res/images/win1.png"), 660, 600);
	loadimage(&imgLose[0], _T("./res/images/lose0.png"), 660, 600);
	loadimage(&imgLose[1], _T("./res/images/lose1.png"), 660, 600);

	loadimage(&imgList[0], _T("./res/images/list0.png"), 52, 42);
	loadimage(&imgList[1], _T("./res/images/list1.png"), 52, 42);

	for (int i = 0; i < 48; i++)
	{
		std::string imgLoc;
		std::string idString = std::to_string(i);
		CString imgLocCString;

		imgLoc = imgOriginalFrontLoc + idString + ".png";
		imgLocCString = imgLoc.c_str();
		loadimage(&imgFront[i], imgLocCString, 96, 96, true);

		imgLoc = imgCoverFrontLoc + idString + ".png";
		imgLocCString = imgLoc.c_str();
		loadimage(&imgFrontCover[i], imgLocCString, 96, 96, true);

		imgLoc = imgOriginalBackLoc + idString + ".png";
		imgLocCString = imgLoc.c_str();
		loadimage(&imgBack[i], imgLocCString, 96, 96);

		imgLoc = imgCoverBackLoc + idString + ".png";
		imgLocCString = imgLoc.c_str();
		loadimage(&imgBackCover[i], imgLocCString, 96, 96);
	}

	initgraph(660, 600);

	putimage(0, 0, &imgStart);

	setbkmode(TRANSPARENT);//设置字体背景为透明
	settextcolor(RGB(0, 73, 93));//设置字体颜色

	SetFont();
}

//初始化客户端
bool Client::InitWSA()
{
	//初始化socket库
	//1.加载套接字库
	WORD versionRequested = MAKEWORD(2, 2);//版本号
	WSADATA wsaData;
	int err = WSAStartup(versionRequested, &wsaData);//使用Socket之前必须调用,加载套接字库返回信息至err
	if (err != 0)
	{
		return false;
	}
	else
		;
	//2.检测套接字库版本号
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wHighVersion) != 2)
	{
		WSACleanup();
		return false;
	}
	else
		;
	return true;
}


//加载招式到招式资源库
void Client::LoadMoves()
{
	std::ifstream movesFile;
	movesFile.open(MOVE_LOC, std::ios::in);
	if (movesFile.is_open())
	{
		while (movesFile.peek() != EOF)
		{
			int id;//标号
			int accuracy;//招式的命中（不是命中率）
			int power;//招式的威力

			bool isCriticalHit; //是否是容易产生暴击的招式
			bool isPhysical;//是否是物理攻击，true是物理攻击，false是特殊攻击
			std::string type;//招式的属性，招式的属性若与小精灵一致，则可以增大伤害
			std::string name;//招式名字

			movesFile >> id >> accuracy >> power >> isCriticalHit >> isPhysical >> type >> name;

			Move* curMove = new Move(id, accuracy, power, isCriticalHit, isPhysical, type, name);
			movesLibrary.push_back(curMove);
		}//end of while
	}//end of if
	else
		;
}

//加载种族宝可梦文件到内存
void Client::LoadSpeciesPokemon()
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
void Client::LoadIndividualPKM()
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
		//读入之前先释放原来的空间
		int size = individualPokemon.size();
		for (int i = 0; i < size; i++)
		{
			delete individualPokemon[i];
			individualPokemon[i] = nullptr;
		}
		individualPokemon.clear();//读入之前先清空

		//读入
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

//根据情况处理鼠标点击事件
void Client::HandleClickEvent()
{
	if (MouseHit() && !isWaitingMsg)
	{
		MOUSEMSG mouse = GetMouseMsg();//获取鼠标信息
		if (mouse.uMsg == WM_LBUTTONUP)//左键弹起
		{
			//mouse = GetMouseMsg();
			if (isRequestFailed)
			{
				isRequestFailed = false;
			}
			else
			{
				switch (GUIState)
				{
				case State::start:
					{
						if (IsBetween(69, 166, 504, 533, mouse.x, mouse.y))//点击的区域是“注册”按钮
						{
							//需要跟服务器端建立连接
							std::string inputName = InputName(1);//模式1是注册
							std::string inputPass = InputPassword(1);
							if (inputName != "" && inputPass != "")//输入用户名和密码都是合格的
							{
								//先与服务器建立连接
								bool isConnected = ConnectHost(SERVER_PORT);
								while (!isConnected)
								{
									isConnected = ConnectHost(SERVER_PORT);
								}
								//再发送信息
								std::string sendBufString = "SIGN_UP";
								sendBufString += ' ' + inputName + ' ' + inputPass;
								
								memset(socketBuffer, 0, MAX_BUF_LEN);
								strcpy(socketBuffer, sendBufString.c_str());
								send(sockFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给服务端

								isWaitingMsg = true;
							}
							else
								;
						}
						else if (IsBetween(192, 289, 504, 533, mouse.x, mouse.y))//点击的区域是“登录”按钮
						{
							//先检查是否输入了用户名和密码
							//都输入了的话需要跟服务器端建立连接并传输数据
							//只要有一项没有输入就不让连接，并提示用户输入相关信息

							if (name != "" && password != "")//用户名和密码都已经输入过了且合格
							{
								//先与服务器建立连接
								bool isConnected = ConnectHost(SERVER_PORT);
								while (!isConnected)
								{
									isConnected = ConnectHost(SERVER_PORT);
								}
								//再发送信息
								std::string sendBufString = "LOG_IN";
								sendBufString += ' ' + name + ' ' + password;
								strcpy(socketBuffer, sendBufString.c_str());
								send(sockFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给服务端

								isWaitingMsg = true;
							}
							else//提示用户输入相关信息
							{
								hintMsg = "请输入用户名或密码.";
							}
						}
						else if (IsBetween(121, 272, 416, 444, mouse.x, mouse.y))//点击的是“用户名”文本框
						{
							InputName(2);//输入用户名，（模式2是登录，注册的输入不通过点击这里）
							
						}
						else if (IsBetween(121, 272, 450, 478, mouse.x, mouse.y))//点击的是“密码”文本框
						{
							InputPassword(2);//输入密码（模式2是登录，注册的输入不通过点击这里）
							
						}
						else//点击其他区域不予理会
							;
					}
					break;

				case State::mainUI:
					{
						*curCheckUser = *localUser;
						if (IsBetween(59, 189, 456, 485, mouse.x, mouse.y))//点击的区域是“查看用户”按钮
						{
							HandleCheckUser("ALL", 0);
						}
						else if (IsBetween(59, 189, 496, 525, mouse.x, mouse.y))//点击的区域是“升级赛”按钮
						{
							//转换状态, 选择要出战的宝可梦和要挑战的宝可梦
							GUIState = State::choose1;
							hintMsg = "";
							totalPkmSum = localUser->GetPkmSum();
							curPkmPage = 0;
							curCheckPkmSeq = 0;

							isUpgradeBattle = true;

						}
						else if (IsBetween(59, 189, 536, 565, mouse.x, mouse.y))//点击的区域是“决斗赛”按钮
						{
							//转换状态, 选择要出战的宝可梦和要挑战的宝可梦
							GUIState = State::choose1;
							hintMsg = "";
							totalPkmSum = localUser->GetPkmSum();
							curPkmPage = 0;
							curCheckPkmSeq = 0;

							isUpgradeBattle = false;

						}
						else if (IsBetween(408, 610, 121, 331, mouse.x, mouse.y))//点击“查看宝可梦”
						{
							lastState = State::mainUI;
							int typeNum = (mouse.y - 121) / 53;//从y坐标换算出点击的类型
							HandleCheckPokemon(typeNum);

						}//end of else if(isBetween)
						else if (IsBetween(200, 304, 469, 504, mouse.x, mouse.y))//登出
						{
							LogOut();
						}
						else
							;
					}
					break;

				case State::userUI:
					{
						if (IsBetween(59, 189, 415, 444, mouse.x, mouse.y))//点击的区域是“主界面”按钮
						{
							GUIState = State::mainUI;
							hintMsg = "";
							//*curCheckUser = *localUser;
						}
						
						else if (IsBetween(408, 610, 121, 331, mouse.x, mouse.y))//点击“查看宝可梦”
						{
							lastState = State::userUI;
							int typeNum = (mouse.y - 121) / 53;//从y坐标换算出点击的类型
							HandleCheckPokemon(typeNum);

						}
						else if (IsBetween(45, 97, 65, 343, mouse.x, mouse.y))//切换当前查看的账户
						{
							//先计算用户ID,如果没有超出范围，就向服务器放松请求
							int requestID = curUserPage * 6 + (mouse.y - 65) / 46;
							if (requestID < totalUserSum)
							{
								HandleCheckUser("ONE", requestID);
							}
							else
								;

						}
						else if (IsBetween(40, 100, 361, 384, mouse.x, mouse.y))//点击“切换页面”按钮
						{
							if (mouse.x <= 63)
							{
								if (curUserPage != 0)
								{
									curUserPage--;
								}
								else
									;
							}
							else if (mouse.x >= 78)
							{
								if ((curUserPage + 1) * 6 < totalUserSum)
								{
									curUserPage++;
								}
								else;
							}
							else;
						}
						else if (IsBetween(200, 304, 469, 504, mouse.x, mouse.y))//登出
						{
							LogOut();
						}
						else
							;
					}
					break;

				case State::pokemonUI:
					{
						if (IsBetween(59, 189, 415, 444, mouse.x, mouse.y))//点击的区域是“主界面”按钮
						{
							GUIState = State::mainUI;
							hintMsg = "";
							//*curCheckUser = *localUser;
						}
						else if (IsBetween(59, 189, 456, 485, mouse.x, mouse.y))//点击的区域是“查看用户”按钮
						{
							HandleCheckUser("ALL", 0);
						}
						else if (IsBetween(550, 620, 35, 65, mouse.x, mouse.y))//点击返回按钮，返回到用户页面
						{
							GUIState = lastState;
							hintMsg = "";
						}
						
						else if (IsBetween(200, 304, 469, 504, mouse.x, mouse.y))//登出
						{
							LogOut();
						}
						else if (IsBetween(40, 100, 361, 384, mouse.x, mouse.y))//点击“切换页面”按钮
						{
							if (mouse.x <= 63)
							{
								if (curPkmPage != 0)
								{
									curPkmPage--;
								}
								else
									;
							}
							else if (mouse.x >= 78)
							{
								if ((curPkmPage + 1) * 6 < totalPkmSum)
								{
									curPkmPage++;
								}
								else;
							}
							else;
						}
						else if (IsBetween(45, 97, 65, 343, mouse.x, mouse.y))//切换当前查看的宝可梦
						{
							//先计算用户ID,如果没有超出范围，就切换下标号
							int changeID = curPkmPage * 6 + (mouse.y - 65) / 46;
							if (changeID < totalPkmSum)
							{
								curCheckPkmSeq = changeID;
							}
							else
								;
						}
						else
							;
					}
					break;

				case State::choose1:
					{
						//从自己的背包中选择要出战的宝可梦
						if (IsBetween(45, 97, 65, 343, mouse.x, mouse.y))//切换要选择的宝可梦
						{
							int seq = curPkmPage * 6 + (mouse.y - 65) / 46;//换算下标
							if (seq < totalPkmSum)
							{
								curCheckPkmSeq = seq;

							}
							else;
						}
						else if (IsBetween(40, 100, 361, 384, mouse.x, mouse.y))//点击“切换页面”按钮
						{
							if (mouse.x <= 63)//左页面
							{
								if (curPkmPage != 0)
								{
									curPkmPage--;
								}
								else
									;
							}
							else if (mouse.x >= 78)//右页面
							{
								if ((curPkmPage + 1) * 6 < totalPkmSum)
								{
									curPkmPage++;
								}
								else;
							}
							else;
						}
						else if (IsBetween(475, 545, 36, 66, mouse.x, mouse.y))//点击确认，进入下一模式
						{
							myBattlePkm = localUser->GetTotalBagPkm(curCheckPkmSeq);
							myPkmID = myBattlePkm->GetSpeiesID();

							GUIState = State::choose2;
							//默认模式是选择初级
							chooseMode = Mode::low;
							hintMsg = "当前选择“初级宝可梦”模式";
						}
						else if (IsBetween(551, 621, 36, 66, mouse.x, mouse.y))//点击取消，返回主界面
						{
							GUIState = State::mainUI;
							hintMsg = "";
							//*curCheckUser = *localUser;
						}
						else
							;

						hintMsg = "就决定是你了，" + localUser->GetTotalBagPkm(curCheckPkmSeq)->GetName() + "!";
					}
					break;

				case State::choose2:
					{
						//选择挑战模式
						//点击确认后会进入下一个选择模式
						if (IsBetween(195, 457, 108, 175, mouse.x, mouse.y))//点击初级模式
						{
							chooseMode = Mode::low;
							hintMsg = "当前选择“初级宝可梦”模式";
						}
						else if (IsBetween(195, 457, 200, 267, mouse.x, mouse.y))
						{
							chooseMode = Mode::mid;
							hintMsg = "当前选择“中级宝可梦”模式";
						}
						else if (IsBetween(195, 457, 295, 362, mouse.x, mouse.y))
						{
							chooseMode = Mode::high;
							hintMsg = "当前选择“高级宝可梦”模式";
						}
						else if (IsBetween(29, 121, 39, 81, mouse.x, mouse.y))
						{
							//返回choose1
							GUIState = State::choose1;

						}
						else if (IsBetween(542, 625, 38, 80, mouse.x, mouse.y))
						{
							//点击确认，进入下一模式
							GUIState = State::choose3;
							hintMsg = "";
						}
						else
							;
					}
					break;

				case State::choose3:
					{
						//选择挑战精灵
						if (IsBetween(98, 189, 90, 364, mouse.x, mouse.y))//水系宝可梦
						{
							int seq = ((mouse.y - 90) / 74) * 3 + (int)chooseMode;//换算下标

							enemyID = seq;//对手的种族id

							hintMsg = "当前选择挑战的宝可梦为：" + pokemonLibrary[enemyID]->GetName();
						}
						else if (IsBetween(220, 311, 90, 364, mouse.x, mouse.y))//火系宝可梦
						{
							int seq = ((mouse.y - 90) / 74) * 3 + (int)chooseMode;//换算下标
							enemyID = seq + 12;//对手的种族id

							hintMsg = "当前选择挑战的宝可梦为：" + pokemonLibrary[enemyID]->GetName();
						}
						else if (IsBetween(344, 435, 90, 364, mouse.x, mouse.y))//草系宝可梦
						{
							int seq = ((mouse.y - 90) / 74) * 3 + (int)chooseMode;//换算下标
							enemyID = seq + 24;//对手的种族id

							hintMsg = "当前选择挑战的宝可梦为：" + pokemonLibrary[enemyID]->GetName();

						}
						else if (IsBetween(468, 561, 90, 364, mouse.x, mouse.y))//电系宝可梦
						{
							int seq = ((mouse.y - 90) / 74) * 3 + (int)chooseMode;//换算下标
							enemyID = seq + 36;//对手的种族id

							hintMsg = "当前选择挑战的宝可梦为：" + pokemonLibrary[enemyID]->GetName();
						}
						else if (IsBetween(29, 121, 39, 81, mouse.x, mouse.y))//点击返回，返回choose2
						{
							GUIState = State::choose2;
						}
						else if (IsBetween(542, 625, 38, 80, mouse.x, mouse.y))//点击确认，发送给服务器，等待响应
						{
							hintMsg = "";

							std::string sendBufString = "BATTLE";
							sendBufString += ' ' + std::to_string(enemyID);
							sendBufString += ' ' + std::to_string(myBattlePkm->GetLevel());
							if (isUpgradeBattle)
							{
								sendBufString += ' ' + std::to_string(1);
							}
							else
							{
								sendBufString += ' ' + std::to_string(0);
							}

							strcpy(socketBuffer, sendBufString.c_str());
							send(sockFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给服务端

							isWaitingMsg = true;
						}
						else
							;
						//点击确认后会开始模拟战斗


					}
					break;

				case State::win:
					{
						if (IsBetween(280, 375, 329, 364, mouse.x, mouse.y))//点击确认,返回主界面
						{
							GUIState = State::mainUI;
							hintMsg = "";
							//*curCheckUser = *localUser;
						}
						else
							;
					}//end of case win
					break;

				case State::lose:
					{
						if (isUpgradeBattle)//升级赛，点击确认即可返回
						{
							if (IsBetween(280, 375, 329, 364, mouse.x, mouse.y))//点击确认
							{
								GUIState = State::mainUI;
								hintMsg = "";
								//*curCheckUser = *localUser;
							}
							else
								;
						}
						else//决斗赛，点击确认后暂时不会返回主界面，得等得到服务器的肯定回答后才会转换
						{
							if (IsBetween(280, 375, 329, 364, mouse.x, mouse.y))//点击确认
							{
								if (isOkRet)
								{
									GUIState = State::mainUI;
									//*curCheckUser = *localUser;
									hintMsg = "";
								}
								else
								{
									isWaitingMsg = true;
									loseSeq[loseSequence] = -1;
									SendLoseMsg();
								}
							}
							else if(IsBetween(120, 533, 130, 314, mouse.x, mouse.y))//选择要失去的宝可梦
							{
								loseSequence = (mouse.x - 120) / 146;

								Pokemon* pkm = localUser->GetTotalBagPkm(loseSeq[loseSequence]);
								loseID = pkm->GetIndividualID();
								hintMsg = "您当前选择要失去的宝可梦为：" + pkm->GetName();
							}
							else
								;
						}
					}//end of case lose
					break;

				default:
					break;
				}
			}
			

		}//end of if(鼠标有左键按下)
		else
			;//没有鼠标点击不做处理
	}
	else
		;//鼠标没有点击或当前正在等待服务器回应
}

void Client::RecvServerMsg()
{
	if (isWaitingMsg)//只在等待接收的状态下进行
	{
		putimage(228, 177, &imgWaiting);//加载等待图标
		recv(sockFd, socketBuffer, MAX_BUF_LEN, 0);//没有接受到相应信息，点击画面也不会有变化

		std::vector<std::string> recvReponse = SplitBuf();
		int splitSize = recvReponse.size();

		switch (GUIState)
		{
		case State::start://该界面有两个功能需要连接服务器：注册/登录
			{
				//先断开连接
				closesocket(sockFd);//与SERVER_PORT都是短链接，用一次就关
				
				//判断收到的信息
				if(recvReponse[0] == "ALLOW_SIGNUP")//同意注册
				{
					hintMsg = "注册成功！";
				}
				else if (recvReponse[0] == "EXIST")//拒绝注册
				{
					hintMsg = "注册失败,该用户名已存在.";
					isRequestFailed = true;
				}
				else if (recvReponse[0] == "NOT_EXIST")//登录的用户名不存在
				{
					hintMsg = "用户名不存在.";
					isRequestFailed = true;
				}
				else if (recvReponse[0] == "ERROR")//密码和用户名不匹配
				{
					hintMsg = "用户名或密码输入错误.";
					isRequestFailed = true;
				}
				else if (recvReponse[0] == "ALREADY_ONLINE")//用户已经登陆
				{
					hintMsg = "不可在多台设备上登录.";
					isRequestFailed = true;
				}
				else if (recvReponse[0] == "ALLOWED" && splitSize >= 9)//可以登录
				{
					//构建localUser
					int pkmSum = std::stoi(recvReponse[7]);//宝可梦数

					if (splitSize >= 9 + pkmSum)
					{
						//构造本机User
						CreateUser(recvReponse,*localUser);

						//连接新端口
						bool isConnected = ConnectHost(endpointPort);
						while (!isConnected)
						{
							isConnected = ConnectHost(endpointPort);
						}
						localUser->SetIsOnline(true);

						//连接成功后可进入游戏主画面
						GUIState = State::mainUI;
						hintMsg = "";
						//*curCheckUser = *localUser;
					}
					else
					{
						hintMsg = "连接失败，请重新尝试.";
						isRequestFailed = true;
					}
				}
				else if (recvReponse[0] == "SERVER_ERROR")//服务器错误
				{
					hintMsg = "连接失败，请重新尝试.";
					isRequestFailed = true;
				}
				else
					;
			}
			break;

		case State::mainUI:
		case State::userUI:
			{
				if (recvReponse[0] == "USER_INFO" && splitSize >= 9)//查看用户
				{
					int pkmSum = std::stoi(recvReponse[7]);//宝可梦数
					if (splitSize >= 9 + pkmSum)
					{
						//构造当前查看的User
						CreateUser(recvReponse, *curCheckUser);
						GUIState = State::userUI;
						hintMsg = "";
					}
					else
					{
						hintMsg = "请求失败，请重新尝试.";
						isRequestFailed = true;
					}
				}
				else
					;
			}
			break;

		case State::choose3:
			{
				if (recvReponse[0] == "BATTLE_PKM" && splitSize >= 11)
				{
					CreateEnemyPkm(recvReponse);//构造敌人

					//可以开始战斗了
					GUIState = State::battle;

					switch (rand() % 4)
					{
					case 0:
						battleWeather = Weather::rain;
						break;
					case 1:
						battleWeather = Weather::sun;
						break;
					case 2:
						battleWeather = Weather::thunder;
						break;
					default:
						break;
					}

					currentTime = clock();
					myBattlePkm->Reset();//初始化相关数值
					enemyPkm->Reset();
				}
				else
					;
			}
			break;

		case State::win:
			{
				if (recvReponse[0] == "DONE")
				{
					//GUIState = State::mainUI;
					//*curCheckUser = *localUser;
				}
				else
				{
					isRequestFailed = true;
				}
			}
			break;
		case State::lose:
			{
				if (recvReponse[0] == "DONE")//如果是决斗赛，将失去的宝可梦从用户背包删除
				{
					if (isUpgradeBattle)
						;
					else
					{
						isOkRet = true;

						//
						GUIState = State::mainUI;
						hintMsg = "";

						localUser->LosePokemon(loseID);
						*curCheckUser = *localUser;
					}
				}
				else if (recvReponse[0] == "NEW_PKM" && splitSize >= 11)//战败唯一的宝可梦也失去了，系统重新分配一只新的
				{
					localUser->LosePokemon(loseID);//先删去原来的那只

					GUIState = State::mainUI;
					hintMsg = "";
					isOkRet = true;

					int pkmIndividualID = std::stoi(recvReponse[1]);
					int speciesID = std::stoi(recvReponse[2]);
					int pkmLevel = std::stoi(recvReponse[3]);
					int pkmEXP = std::stoi(recvReponse[4]);
					int attr[6];
					for (int i = 0; i < 6; i++)
					{
						attr[i] = std::stoi(recvReponse[i + 5]);
					}

					Pokemon* newPkm = new Pokemon(pkmIndividualID, speciesID, pkmLevel, pkmEXP, attr);
					localUser->AddPokemonToBags(newPkm);
					*curCheckUser = *localUser;

					//加入背包的是新的地址，这个可以释放了
					delete newPkm;
					newPkm = nullptr;
				}
				else
				{
					isRequestFailed = true;
				}
			}
			break;
		default:
			break;
		}

		isWaitingMsg = false;

		if (recvReponse[0] == "RECV_ERROR")
		{
			hintMsg = "请求失败，请重新尝试.";
			isRequestFailed = true;
		}
		else
			;
	}
}

void Client::Draw()
{
	currentTime = clock();
	if (currentTime - lastUpdateTime >= UPDATE_INTERVAL && !isWaitingMsg)//如果到了更新时间间隔，就更新画面
	{
		BeginBatchDraw();

		lastUpdateTime = currentTime;

		std::string text;
		CString textCString;
		

		//根据当前所处的UI状态决定贴什么图
		switch (GUIState)
		{
		case State::start:
			{
				putimage(0, 0, &imgStart);

				font.lfHeight = 20; // 设置字体高度
				settextstyle(&font);// 设置字体样式
				
				//如果输入了用户名，则显示
				if (name != "")
				{
					textCString = name.c_str();
					outtextxy(131, 422, textCString);
				}
				else
					;

				//如果输入了密码，则显示8个*
				if (password != "")
				{
					text = "********";
					textCString = text.c_str();
					outtextxy(131, 455, textCString);
				}
			}
			break;

		case State::mainUI:
			{
				putimage(0, 0, &imgMainUI);
				
				font.lfHeight = 26; // 设置字体高度
				settextstyle(&font);// 设置字体样式

				localUser->DrawUserInfo(0, (int)GUIState);//打印用户信息到界面
			}
			break;

		case State::userUI:
			{
				putimage(0, 0, &imgSeeUser);

				font.lfHeight = 26; // 设置字体高度
				settextstyle(&font);// 设置字体样式

				curCheckUser->DrawUserInfo(localUser->GetID(),(int)GUIState);//打印用户信息到界面
				curCheckUser->DrawUserList(curUserPage,totalUserSum);//打印用户ID列表

			}
			break;

		case State::pokemonUI:
			{
				putimage(0, 0, &imgSeePokemon);

				font.lfHeight = 23; // 设置字体高度
				settextstyle(&font);// 设置字体样式

				Pokemon* curCheckPkm;
				if (lastState == State::mainUI)
				{
					curCheckPkm = localUser->GetCurPkm(curCheckType, curCheckPkmSeq);
				}
				else
				{
					curCheckPkm = curCheckUser->GetCurPkm(curCheckType, curCheckPkmSeq);
				}
				
				curCheckPkm->DrawPkmInfo();//打印宝可梦信息到界面
				curCheckPkm->DrawPkmList(curPkmPage, totalPkmSum, curCheckPkmSeq);//打印宝可梦背包列表

				int speciesID = curCheckPkm->GetSpeiesID();
				putimage(307, 25, &imgFrontCover[speciesID], SRCPAINT);
				putimage(307, 25, &imgFront[speciesID], SRCAND);
			}
			break;

		case State::choose1:
			{
				putimage(0, 0, &imgChoose[4]);

				font.lfHeight = 23; // 设置字体高度
				settextstyle(&font);// 设置字体样式

				Pokemon* curCheckPkm = localUser->GetTotalBagPkm(curCheckPkmSeq);
				curCheckPkm->DrawPkmInfo();//打印宝可梦信息到界面
				curCheckPkm->DrawPkmList(curPkmPage, totalPkmSum, curCheckPkmSeq);//打印宝可梦背包列表

				int speciesID = curCheckPkm->GetSpeiesID();
				putimage(307, 25, &imgFrontCover[speciesID], SRCPAINT);
				putimage(307, 25, &imgFront[speciesID], SRCAND);
			}
			break;

		case State::choose2:
			{
				putimage(0, 0, &imgChoose[3]);
			}
			break;

		case State::choose3:
			{
				putimage(0, 0, &imgChoose[(int)chooseMode]);
			}
			break;

		case State::battle:
			{
				putimage(0, 0, &imgBattle);

				//贴宝可梦的图，我方贴back，敌方贴Front
				setaspectratio(2, 2);
				putimage(193, 33, &imgFrontCover[enemyID], SRCPAINT);//对手掩码
				putimage(193, 33, &imgFront[enemyID], SRCAND);//对手原图
				putimage(47, 101, &imgBackCover[myPkmID], SRCPAINT);//本方掩码
				putimage(47, 101, &imgBack[myPkmID], SRCAND);//本方原图
				setaspectratio(1, 1);

				//打印天气状态
				font.lfHeight = 26; // 设置字体高度
				settextstyle(&font);// 设置字体样式
				std::string text;
				CString textCString;
				switch (battleWeather)
				{
				case Weather::rain:
					text = "雨天(水系加成)";
					break;
				case Weather::sun:
					text = "晴天(火、草系加成)";
					break;
				case Weather::thunder:
					text = "打雷(电系加成)";
					break;
				default:
					text = "普通";
					break;
				}
				textCString = text.c_str();
				outtextxy(125, 29, textCString);

				//打印宝可梦剩余血量及出招状况
				myBattlePkm->DrawBattle(false);
				enemyPkm->DrawBattle(true);
			}
			break;

		case State::win:
			{
				if (isUpgradeBattle)//如果只是升级赛
				{
					putimage(0, 0, &imgWin[1]);
				}
				else//如果是决斗赛
				{
					putimage(0, 0, &imgWin[0]);
				}

				//打印经验值
				font.lfHeight = 26; // 设置字体高度
				settextstyle(&font);// 设置字体样式
				text = std::to_string(gainedEXP);
				textCString = text.c_str();
				outtextxy(385, 143, textCString);
				
			}
			break;

		case State::lose:
			{
				if (isUpgradeBattle)//如果只是升级赛
				{
					putimage(0, 0, &imgLose[1]);
				}
				else//如果是决斗赛
				{
					putimage(0, 0, &imgLose[0]);

					//贴宝可梦图和相关信息
					for (int i = 0; i < 3; i++)
					{
						if (loseSeq[i] != -1)
						{
							Pokemon* curPkm = localUser->GetTotalBagPkm(loseSeq[i]);
							curPkm->DrawLose(i);
						}
					}
				}
			}
			break;

		default:
			break;
		}

		if (isRequestFailed)//如果加载失败了，则加载失败图标
		{
			putimage(228, 177, &imgFailed);
		}
		else
			;

		//打印提示信息
		font.lfHeight = 20; // 设置字体高度
		settextstyle(&font);// 设置字体样式
		settextcolor(BLACK);
		textCString = hintMsg.c_str();
		outtextxy(352, 425, textCString);

		EndBatchDraw();
		
	}
	else
		;//否则不更新画面
}

void Client::AutoBattle()
{
	currentTime = clock();

	int myHP = myBattlePkm->GetCurrentHP();
	int enemyHP = enemyPkm->GetCurrentHP();
	int myAttackDamage = 0;
	int enemyAttackDamage = 0;
	if (myHP > 0 && enemyHP > 0)//当双方都还有血量的时候，战斗持续
	{
		myAttackDamage = myBattlePkm->Attack(enemyPkm);
		enemyAttackDamage = enemyPkm->Attack(myBattlePkm);

		myBattlePkm->Damage(enemyAttackDamage);
		enemyPkm->Damage(myAttackDamage);
	}
	else
		;

	//当有一方没有血量时，战斗结束，根据血量判断谁是胜利方,并给服务器发送信息
	if (myHP > 0 && enemyHP <= 0)
	{
		//我方获胜,给服务器发送信息
		HandleWinEvent();
	}
	else if (enemyHP > 0 && myHP <= 0)
	{
		//敌方获胜
		HandleLoseEvent();
	}
	else if (myHP <= 0 && enemyHP <= 0 && myAttackDamage && enemyAttackDamage)
	{
		//双方同一时间出手且都对对方致命，则谁速度快谁赢

		if (myBattlePkm->GetSpeed() >= enemyPkm->GetSpeed())//我方获胜
		{
			HandleWinEvent();
		}
		else//敌方获胜
		{
			HandleLoseEvent();
		}
	}
	else
		;

}

//处理用户获胜事件
void Client::HandleWinEvent()
{
	//更新用户胜利次数
	localUser->SetBattleTimes(true);

	//转换GUI状态
	GUIState = State::win;
	isWaitingMsg = true;

	//首先要加经验
	gainedEXP = myBattlePkm->UpdateExp(enemyPkm);

	//给服务器发送请求
	/*
		记录内容及数据类型如下：(升级赛的话没有第一项)
		获得的宝可梦ID|胜利个体ID	|  等级	| 经验值	| 生命值	| 攻击值	| 防御值	| 特攻值	| 特防值	| 速度值	|
			int		 |	int		|  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|
	*/
	//构建发送内容
	std::string sendBufString;
	if (isUpgradeBattle)//升级赛
	{
		sendBufString = "WIN";
	}
	else//决斗赛
	{
		sendBufString = "WIN_PKM";
		sendBufString += ' ' + std::to_string(enemyPkm->GetIndividualID());

		//把战胜的宝可梦加入背包
		localUser->AddPokemonToBags(enemyPkm);
		*curCheckUser = *localUser;
	}
	sendBufString += ' ' + myBattlePkm->GetPokemonInfo();//包含了后9项

	//更新满级精灵的情况
	localUser->UpdateFullLevel();
	
	//发送
	strcpy(socketBuffer, sendBufString.c_str());
	send(sockFd, socketBuffer, MAX_BUF_LEN, 0);
}

//处理用户战斗失败事件
void Client::HandleLoseEvent()
{
	//转换GUI状态
	GUIState = State::lose;

	//更新用户失败次数
	localUser->SetBattleTimes(false);

	if (isUpgradeBattle)//只是升级赛
	{
		std::string sendBufString = "LOSE";
		//发送
		strcpy(socketBuffer, sendBufString.c_str());
		send(sockFd, socketBuffer, MAX_BUF_LEN, 0);
		isWaitingMsg = true;
	}
	else
	{
		//从用户背包中随机选三只，记录在背包中的下标，不够的设为-1
		//先全初始化为-1
		for (int i = 0; i < 3; i++)
		{
			loseSeq[i] = -1;
		}
		//随机取3只
		for (int i = 0; i < 3; i++)
		{
			loseSeq[i] = localUser->RandPickLosePkm(loseSeq[0], loseSeq[1]);
		}

		//释放释放敌方宝可梦的空间
		delete enemyPkm;
		enemyPkm = nullptr;

		//转移状态及一些初始化
		GUIState = State::lose;
		Pokemon* pkm = localUser->GetTotalBagPkm(loseSeq[0]);
		loseID = pkm->GetIndividualID();
		hintMsg = "您当前选择要失去的宝可梦为：" + pkm->GetName();

		isChooseLose = false;
		isOkRet = false;
	}
}

void Client::SendLoseMsg()
{
	//构建发送内容
	std::string sendBufString = "LOSE_PKM";
	sendBufString += ' ' + std::to_string(loseID);

	strcpy(socketBuffer, sendBufString.c_str());
	send(sockFd, socketBuffer, MAX_BUF_LEN, 0);
}

bool Client::IsBetween(int left, int right, int top, int bottom, int x, int y) const
{
	if ((x >= left && x <= right) && (y <= bottom && y >= top))
		return true;
	else
		return false;
}

std::string Client::InputName(int mode)
{
	name = "";
	std::string userNameString = "";
	char userName[40];//设大一点防止越界
	bool isConfirmed = InputBox(userName, 40, _T("请输入用户名：\n（0<长度<=20,且不得包含空白字符)"), 0, 0, 0, 0, false);//允许点取消，如果点取消则直接返回

	while (!IsNameValid(userName) && isConfirmed)
	{
		isConfirmed = InputBox(userName, 40, _T("用户名格式不正确，请重新输入：\n（0<长度<=20,且不得包含空白字符)"), 0, 0, 0, 0, false);//允许点取消，如果点取消则直接返回
	}
	if (isConfirmed)//如果点击确认，则此刻的用户名是合法的
	{
		userNameString = userName;

		if (mode == 2)//模式2是登录，1是注册
		{
			name = userNameString;
		}
		else
			;
	}
	else//如果点击取消，不用处理
		;

	return userNameString;
}

std::string Client::InputPassword(int mode)
{
	password = "";

	char userPass[30];//设大一点防止越界
	std::string userPassString = "";
	bool isConfirmed = InputBox(userPass, 30, _T("请输入密码：\n（6<=长度<=20,且只包含数字或字母)"), 0, 0, 0, 0, false);//允许点取消，如果点取消则直接返回

	while (!IsPasswordValid(userPass) && isConfirmed)
	{
		isConfirmed = InputBox(userPass, 30, _T("密码格式不正确，请重新输入：\n（6<=长度<=20,且只包含数字或字母)"), 0, 0, 0, 0, false);//允许点取消，如果点取消则直接返回
	}
	if (isConfirmed)//如果点击确认，则此刻的用户名是合法的
	{
		userPassString = userPass;

		if (mode == 2)//模式2是登录，1是注册
		{
			password = userPassString;
		}
		else
			;
	}
	else//如果点击取消，不用处理
		;

	return userPassString;
}

bool Client::IsNameValid(char userName[40]) const
{
	bool isValid = true;

	int len = strlen(userName);
	if (len == 0 || len > 20)//检查长度是否合格
	{
		isValid = false;
	}
	else
		;

	for (int i = 0; i < len && isValid; i++)
	{
		if (userName[i] == '\t' || userName[i] == '\n' || userName[i] == '\b')//检查是否有不合格字符
		{
			isValid = false;
		}
		else
			;
	}

	return isValid;
}

bool Client::IsPasswordValid(char userPass[30]) const
{
	bool isValid = true;

	int len = strlen(userPass);
	if (len < 6 || len > 20)//检查长度是否合格
	{
		isValid = false;
	}
	else
		;

	for (int i = 0; i < len && isValid; i++)
	{

		if (!((userPass[i] >= 'a' && userPass[i] <= 'z')
			|| (userPass[i] >= 'A' && userPass[i] <= 'Z')
			|| (userPass[i] >= '0' && userPass[i] <= '9')))//检查是否有不合格字符
		{
			isValid = false;
		}
		else
			;
	}

	return isValid;
}

//分割接收到的数据，数据以空格' '做分隔符
std::vector<std::string> Client::SplitBuf() const
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

//初始化要打印在界面上的字体
void Client::SetFont()
{
	//设置字体
	gettextstyle(&font);// 获取当前字体设置
	font.lfWeight = FW_BOLD;
	font.lfQuality = PROOF_QUALITY;    // 设置输出效果为抗锯齿 
	_tcscpy_s(font.lfFaceName, _T("微软雅黑"));
	settextstyle(&font);           // 设置字体样式
}

void Client::CreateUser(std::vector<std::string> recvReponse, User& user)
{
	if (recvReponse[0] == "USER_INFO")//构建的是当前查看的用户
	{
		totalUserSum = std::stoi(recvReponse[1]);
	}
	else//构建的是本地用户
	{
		endpointPort = std::stoi(recvReponse[1]);
	}


	//每次构造新用户之前，得先把原用户的背包空间释放
	user.ClearBags();
	user.SetUser(recvReponse);

	//重新读入个体宝可梦文件（可能有修改）
	LoadIndividualPKM();


	//根据用户宝可梦数和更新后的个体宝可梦库给用户背包添加宝可梦
	int sum = user.GetPkmSum();
	user.SetPkmSum(0);
	for (int i = 9; i < 9 + sum; i++)//recvResponse下标从9开始是宝可梦的个体ID
	{
		int seq = std::stoi(recvReponse[i]);
		user.AddPokemonToBags(individualPokemon[seq]);
	}
}

void Client::CreateEnemyPkm(std::vector<std::string> recvReponse)
{
	//每次构造之前要先把原来的释放
	delete enemyPkm;
	enemyPkm = nullptr;

	int pkmIndividualID = std::stoi(recvReponse[1]);
	int speciesID = std::stoi(recvReponse[2]);
	int pkmLevel = std::stoi(recvReponse[3]);
	int pkmEXP = std::stoi(recvReponse[4]);
	int attr[6];
	for (int i = 0; i < 6; i++)
	{
		attr[i] = std::stoi(recvReponse[i + 5]);
	}
	
	Pokemon* newEnemyPkm = new Pokemon(pkmIndividualID, speciesID, pkmLevel, pkmEXP, attr);

	//根据类别构造子类指针然后强制转换为父类指针
	switch (newEnemyPkm->GetType())
	{
		case Type::water:
			{
				WaterPokemon* waterPkm = new WaterPokemon(*newEnemyPkm);
				enemyPkm = (Pokemon*)waterPkm;
			}
			break;

		case Type::fire:
			{
				FirePokemon* firePkm = new FirePokemon(*newEnemyPkm);
				enemyPkm = (Pokemon*)firePkm;
			}
			break;

		case Type::grass:
			{
				GrassPokemon* grassPkm = new GrassPokemon(*newEnemyPkm);
				enemyPkm = (Pokemon*)grassPkm;
			}
			break;

		default://electric
			{
				ElectricPokemon* electricPkm = new ElectricPokemon(*newEnemyPkm);
				enemyPkm = (Pokemon*)electricPkm;
			}
			break;
	}//end of swtich
}

void Client::LogOut()
{
	//向服务器发送信息，并直接结束程序
	isLogOut = true;

	std::string sendBufString = "LOG_OUT";
	strcpy(socketBuffer, sendBufString.c_str());
	send(sockFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给服务端

	closesocket(sockFd);
}

//点击查看用户
void Client::HandleCheckUser(std::string mode, int id)
{
	//向服务端发送信息,等收到回信后在进行页面的跳转
	std::string sendBufString = "SEE_OTHER_USERS";
	sendBufString += ' ' + mode;

	if (mode == "ONE")//如果是切换查看当前的用户，还需加上要查看的用户的ID
	{
		sendBufString += ' ' + std::to_string(id);
	}
	else
		;

	strcpy(socketBuffer, sendBufString.c_str());
	send(sockFd, socketBuffer, MAX_BUF_LEN, 0);//发送反馈信息给服务端

	isWaitingMsg = true;

	curUserPage = 0;
}

//点击查看某个背包的宝可梦
void Client::HandleCheckPokemon(int typeNum)
{
	//确定点击的背包是哪个类型
	switch (typeNum)
	{
	case 0:
		curCheckType = Type::water;
		break;

	case 1:
		curCheckType = Type::fire;
		break;

	case 2:
		curCheckType = Type::grass;
		break;

	default:
		curCheckType = Type::electric;
		break;
	}

	//判断数量是否为0，若是则不反应/提示无宝可梦，若不是则转换
	if (lastState == State::mainUI)
	{
		totalPkmSum = localUser->HasPokemon(curCheckType);
	}
	else
	{
		totalPkmSum = curCheckUser->HasPokemon(curCheckType);
	}

	if (totalPkmSum)
	{
		GUIState = State::pokemonUI;
		hintMsg = "";
		curPkmPage = 0;
		curCheckPkmSeq = 0;
	}
	else
	{
		hintMsg = "没有该类型的宝可梦>_<..";
	}
}

bool Client::ConnectHost(int port)
{
	//创建套接字，地址族为Inernet网络地址，socket接受数据格式为流，协议是TCP
	sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//此处为主动套接字
	if (sockFd == INVALID_SOCKET)
	{
		closesocket(sockFd);
		WSACleanup();
		return false;
	}
	else
		;

	//与服务器连接，只有在连接建立成功或出错时connect函数才返回
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (connect(sockFd, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) != -1)//连接成功
	{
		return true;
	}
	else//连接失败，且套接口将不能再使用，必须关闭
	{
		closesocket(sockFd);
		return false;
	}
}