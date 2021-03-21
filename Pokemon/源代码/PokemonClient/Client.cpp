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

IMAGE imgFront[48];//С����ս��ʱ��ʾ��ͼ��ǰ����,ԭͼ
IMAGE imgFrontCover[48];//С����ս��ʱ��ʾ��ͼ��ǰ����,����ͼ
IMAGE imgBack[48];//С����ս��ʱ��ʾ��ͼ���󷽣�,ԭͼ
IMAGE imgBackCover[48];//С����ս��ʱ��ʾ��ͼ���󷽣�,����ͼ

LOGFONT font;//����

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

	//�����ļ���Դ������
	LoadMoves();
	LoadSpeciesPokemon();
	LoadIndividualPKM();

	//��ʼ������������
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

//��ʼ��������װ��ͼƬ��Դ
void Client::InitCanves()
{
	//����ͼƬ��Դ,λ�ã�����
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

	setbkmode(TRANSPARENT);//�������屳��Ϊ͸��
	settextcolor(RGB(0, 73, 93));//����������ɫ

	SetFont();
}

//��ʼ���ͻ���
bool Client::InitWSA()
{
	//��ʼ��socket��
	//1.�����׽��ֿ�
	WORD versionRequested = MAKEWORD(2, 2);//�汾��
	WSADATA wsaData;
	int err = WSAStartup(versionRequested, &wsaData);//ʹ��Socket֮ǰ�������,�����׽��ֿⷵ����Ϣ��err
	if (err != 0)
	{
		return false;
	}
	else
		;
	//2.����׽��ֿ�汾��
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wHighVersion) != 2)
	{
		WSACleanup();
		return false;
	}
	else
		;
	return true;
}


//������ʽ����ʽ��Դ��
void Client::LoadMoves()
{
	std::ifstream movesFile;
	movesFile.open(MOVE_LOC, std::ios::in);
	if (movesFile.is_open())
	{
		while (movesFile.peek() != EOF)
		{
			int id;//���
			int accuracy;//��ʽ�����У����������ʣ�
			int power;//��ʽ������

			bool isCriticalHit; //�Ƿ������ײ�����������ʽ
			bool isPhysical;//�Ƿ�����������true����������false�����⹥��
			std::string type;//��ʽ�����ԣ���ʽ����������С����һ�£�����������˺�
			std::string name;//��ʽ����

			movesFile >> id >> accuracy >> power >> isCriticalHit >> isPhysical >> type >> name;

			Move* curMove = new Move(id, accuracy, power, isCriticalHit, isPhysical, type, name);
			movesLibrary.push_back(curMove);
		}//end of while
	}//end of if
	else
		;
}

//�������屦�����ļ����ڴ�
void Client::LoadSpeciesPokemon()
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
void Client::LoadIndividualPKM()
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
		//����֮ǰ���ͷ�ԭ���Ŀռ�
		int size = individualPokemon.size();
		for (int i = 0; i < size; i++)
		{
			delete individualPokemon[i];
			individualPokemon[i] = nullptr;
		}
		individualPokemon.clear();//����֮ǰ�����

		//����
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

//�����������������¼�
void Client::HandleClickEvent()
{
	if (MouseHit() && !isWaitingMsg)
	{
		MOUSEMSG mouse = GetMouseMsg();//��ȡ�����Ϣ
		if (mouse.uMsg == WM_LBUTTONUP)//�������
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
						if (IsBetween(69, 166, 504, 533, mouse.x, mouse.y))//����������ǡ�ע�ᡱ��ť
						{
							//��Ҫ���������˽�������
							std::string inputName = InputName(1);//ģʽ1��ע��
							std::string inputPass = InputPassword(1);
							if (inputName != "" && inputPass != "")//�����û��������붼�Ǻϸ��
							{
								//�����������������
								bool isConnected = ConnectHost(SERVER_PORT);
								while (!isConnected)
								{
									isConnected = ConnectHost(SERVER_PORT);
								}
								//�ٷ�����Ϣ
								std::string sendBufString = "SIGN_UP";
								sendBufString += ' ' + inputName + ' ' + inputPass;
								
								memset(socketBuffer, 0, MAX_BUF_LEN);
								strcpy(socketBuffer, sendBufString.c_str());
								send(sockFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ�������

								isWaitingMsg = true;
							}
							else
								;
						}
						else if (IsBetween(192, 289, 504, 533, mouse.x, mouse.y))//����������ǡ���¼����ť
						{
							//�ȼ���Ƿ��������û���������
							//�������˵Ļ���Ҫ���������˽������Ӳ���������
							//ֻҪ��һ��û������Ͳ������ӣ�����ʾ�û����������Ϣ

							if (name != "" && password != "")//�û��������붼�Ѿ���������Һϸ�
							{
								//�����������������
								bool isConnected = ConnectHost(SERVER_PORT);
								while (!isConnected)
								{
									isConnected = ConnectHost(SERVER_PORT);
								}
								//�ٷ�����Ϣ
								std::string sendBufString = "LOG_IN";
								sendBufString += ' ' + name + ' ' + password;
								strcpy(socketBuffer, sendBufString.c_str());
								send(sockFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ�������

								isWaitingMsg = true;
							}
							else//��ʾ�û����������Ϣ
							{
								hintMsg = "�������û���������.";
							}
						}
						else if (IsBetween(121, 272, 416, 444, mouse.x, mouse.y))//������ǡ��û������ı���
						{
							InputName(2);//�����û�������ģʽ2�ǵ�¼��ע������벻ͨ��������
							
						}
						else if (IsBetween(121, 272, 450, 478, mouse.x, mouse.y))//������ǡ����롱�ı���
						{
							InputPassword(2);//�������루ģʽ2�ǵ�¼��ע������벻ͨ��������
							
						}
						else//����������������
							;
					}
					break;

				case State::mainUI:
					{
						*curCheckUser = *localUser;
						if (IsBetween(59, 189, 456, 485, mouse.x, mouse.y))//����������ǡ��鿴�û�����ť
						{
							HandleCheckUser("ALL", 0);
						}
						else if (IsBetween(59, 189, 496, 525, mouse.x, mouse.y))//����������ǡ�����������ť
						{
							//ת��״̬, ѡ��Ҫ��ս�ı����κ�Ҫ��ս�ı�����
							GUIState = State::choose1;
							hintMsg = "";
							totalPkmSum = localUser->GetPkmSum();
							curPkmPage = 0;
							curCheckPkmSeq = 0;

							isUpgradeBattle = true;

						}
						else if (IsBetween(59, 189, 536, 565, mouse.x, mouse.y))//����������ǡ�����������ť
						{
							//ת��״̬, ѡ��Ҫ��ս�ı����κ�Ҫ��ս�ı�����
							GUIState = State::choose1;
							hintMsg = "";
							totalPkmSum = localUser->GetPkmSum();
							curPkmPage = 0;
							curCheckPkmSeq = 0;

							isUpgradeBattle = false;

						}
						else if (IsBetween(408, 610, 121, 331, mouse.x, mouse.y))//������鿴�����Ρ�
						{
							lastState = State::mainUI;
							int typeNum = (mouse.y - 121) / 53;//��y���껻������������
							HandleCheckPokemon(typeNum);

						}//end of else if(isBetween)
						else if (IsBetween(200, 304, 469, 504, mouse.x, mouse.y))//�ǳ�
						{
							LogOut();
						}
						else
							;
					}
					break;

				case State::userUI:
					{
						if (IsBetween(59, 189, 415, 444, mouse.x, mouse.y))//����������ǡ������桱��ť
						{
							GUIState = State::mainUI;
							hintMsg = "";
							//*curCheckUser = *localUser;
						}
						
						else if (IsBetween(408, 610, 121, 331, mouse.x, mouse.y))//������鿴�����Ρ�
						{
							lastState = State::userUI;
							int typeNum = (mouse.y - 121) / 53;//��y���껻������������
							HandleCheckPokemon(typeNum);

						}
						else if (IsBetween(45, 97, 65, 343, mouse.x, mouse.y))//�л���ǰ�鿴���˻�
						{
							//�ȼ����û�ID,���û�г�����Χ�������������������
							int requestID = curUserPage * 6 + (mouse.y - 65) / 46;
							if (requestID < totalUserSum)
							{
								HandleCheckUser("ONE", requestID);
							}
							else
								;

						}
						else if (IsBetween(40, 100, 361, 384, mouse.x, mouse.y))//������л�ҳ�桱��ť
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
						else if (IsBetween(200, 304, 469, 504, mouse.x, mouse.y))//�ǳ�
						{
							LogOut();
						}
						else
							;
					}
					break;

				case State::pokemonUI:
					{
						if (IsBetween(59, 189, 415, 444, mouse.x, mouse.y))//����������ǡ������桱��ť
						{
							GUIState = State::mainUI;
							hintMsg = "";
							//*curCheckUser = *localUser;
						}
						else if (IsBetween(59, 189, 456, 485, mouse.x, mouse.y))//����������ǡ��鿴�û�����ť
						{
							HandleCheckUser("ALL", 0);
						}
						else if (IsBetween(550, 620, 35, 65, mouse.x, mouse.y))//������ذ�ť�����ص��û�ҳ��
						{
							GUIState = lastState;
							hintMsg = "";
						}
						
						else if (IsBetween(200, 304, 469, 504, mouse.x, mouse.y))//�ǳ�
						{
							LogOut();
						}
						else if (IsBetween(40, 100, 361, 384, mouse.x, mouse.y))//������л�ҳ�桱��ť
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
						else if (IsBetween(45, 97, 65, 343, mouse.x, mouse.y))//�л���ǰ�鿴�ı�����
						{
							//�ȼ����û�ID,���û�г�����Χ�����л��±��
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
						//���Լ��ı�����ѡ��Ҫ��ս�ı�����
						if (IsBetween(45, 97, 65, 343, mouse.x, mouse.y))//�л�Ҫѡ��ı�����
						{
							int seq = curPkmPage * 6 + (mouse.y - 65) / 46;//�����±�
							if (seq < totalPkmSum)
							{
								curCheckPkmSeq = seq;

							}
							else;
						}
						else if (IsBetween(40, 100, 361, 384, mouse.x, mouse.y))//������л�ҳ�桱��ť
						{
							if (mouse.x <= 63)//��ҳ��
							{
								if (curPkmPage != 0)
								{
									curPkmPage--;
								}
								else
									;
							}
							else if (mouse.x >= 78)//��ҳ��
							{
								if ((curPkmPage + 1) * 6 < totalPkmSum)
								{
									curPkmPage++;
								}
								else;
							}
							else;
						}
						else if (IsBetween(475, 545, 36, 66, mouse.x, mouse.y))//���ȷ�ϣ�������һģʽ
						{
							myBattlePkm = localUser->GetTotalBagPkm(curCheckPkmSeq);
							myPkmID = myBattlePkm->GetSpeiesID();

							GUIState = State::choose2;
							//Ĭ��ģʽ��ѡ�����
							chooseMode = Mode::low;
							hintMsg = "��ǰѡ�񡰳��������Ρ�ģʽ";
						}
						else if (IsBetween(551, 621, 36, 66, mouse.x, mouse.y))//���ȡ��������������
						{
							GUIState = State::mainUI;
							hintMsg = "";
							//*curCheckUser = *localUser;
						}
						else
							;

						hintMsg = "�;��������ˣ�" + localUser->GetTotalBagPkm(curCheckPkmSeq)->GetName() + "!";
					}
					break;

				case State::choose2:
					{
						//ѡ����սģʽ
						//���ȷ�Ϻ�������һ��ѡ��ģʽ
						if (IsBetween(195, 457, 108, 175, mouse.x, mouse.y))//�������ģʽ
						{
							chooseMode = Mode::low;
							hintMsg = "��ǰѡ�񡰳��������Ρ�ģʽ";
						}
						else if (IsBetween(195, 457, 200, 267, mouse.x, mouse.y))
						{
							chooseMode = Mode::mid;
							hintMsg = "��ǰѡ���м������Ρ�ģʽ";
						}
						else if (IsBetween(195, 457, 295, 362, mouse.x, mouse.y))
						{
							chooseMode = Mode::high;
							hintMsg = "��ǰѡ�񡰸߼������Ρ�ģʽ";
						}
						else if (IsBetween(29, 121, 39, 81, mouse.x, mouse.y))
						{
							//����choose1
							GUIState = State::choose1;

						}
						else if (IsBetween(542, 625, 38, 80, mouse.x, mouse.y))
						{
							//���ȷ�ϣ�������һģʽ
							GUIState = State::choose3;
							hintMsg = "";
						}
						else
							;
					}
					break;

				case State::choose3:
					{
						//ѡ����ս����
						if (IsBetween(98, 189, 90, 364, mouse.x, mouse.y))//ˮϵ������
						{
							int seq = ((mouse.y - 90) / 74) * 3 + (int)chooseMode;//�����±�

							enemyID = seq;//���ֵ�����id

							hintMsg = "��ǰѡ����ս�ı�����Ϊ��" + pokemonLibrary[enemyID]->GetName();
						}
						else if (IsBetween(220, 311, 90, 364, mouse.x, mouse.y))//��ϵ������
						{
							int seq = ((mouse.y - 90) / 74) * 3 + (int)chooseMode;//�����±�
							enemyID = seq + 12;//���ֵ�����id

							hintMsg = "��ǰѡ����ս�ı�����Ϊ��" + pokemonLibrary[enemyID]->GetName();
						}
						else if (IsBetween(344, 435, 90, 364, mouse.x, mouse.y))//��ϵ������
						{
							int seq = ((mouse.y - 90) / 74) * 3 + (int)chooseMode;//�����±�
							enemyID = seq + 24;//���ֵ�����id

							hintMsg = "��ǰѡ����ս�ı�����Ϊ��" + pokemonLibrary[enemyID]->GetName();

						}
						else if (IsBetween(468, 561, 90, 364, mouse.x, mouse.y))//��ϵ������
						{
							int seq = ((mouse.y - 90) / 74) * 3 + (int)chooseMode;//�����±�
							enemyID = seq + 36;//���ֵ�����id

							hintMsg = "��ǰѡ����ս�ı�����Ϊ��" + pokemonLibrary[enemyID]->GetName();
						}
						else if (IsBetween(29, 121, 39, 81, mouse.x, mouse.y))//������أ�����choose2
						{
							GUIState = State::choose2;
						}
						else if (IsBetween(542, 625, 38, 80, mouse.x, mouse.y))//���ȷ�ϣ����͸����������ȴ���Ӧ
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
							send(sockFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ�������

							isWaitingMsg = true;
						}
						else
							;
						//���ȷ�Ϻ�Ὺʼģ��ս��


					}
					break;

				case State::win:
					{
						if (IsBetween(280, 375, 329, 364, mouse.x, mouse.y))//���ȷ��,����������
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
						if (isUpgradeBattle)//�����������ȷ�ϼ��ɷ���
						{
							if (IsBetween(280, 375, 329, 364, mouse.x, mouse.y))//���ȷ��
							{
								GUIState = State::mainUI;
								hintMsg = "";
								//*curCheckUser = *localUser;
							}
							else
								;
						}
						else//�����������ȷ�Ϻ���ʱ���᷵�������棬�õȵõ��������Ŀ϶��ش��Ż�ת��
						{
							if (IsBetween(280, 375, 329, 364, mouse.x, mouse.y))//���ȷ��
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
							else if(IsBetween(120, 533, 130, 314, mouse.x, mouse.y))//ѡ��Ҫʧȥ�ı�����
							{
								loseSequence = (mouse.x - 120) / 146;

								Pokemon* pkm = localUser->GetTotalBagPkm(loseSeq[loseSequence]);
								loseID = pkm->GetIndividualID();
								hintMsg = "����ǰѡ��Ҫʧȥ�ı�����Ϊ��" + pkm->GetName();
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
			

		}//end of if(������������)
		else
			;//û���������������
	}
	else
		;//���û�е����ǰ���ڵȴ���������Ӧ
}

void Client::RecvServerMsg()
{
	if (isWaitingMsg)//ֻ�ڵȴ����յ�״̬�½���
	{
		putimage(228, 177, &imgWaiting);//���صȴ�ͼ��
		recv(sockFd, socketBuffer, MAX_BUF_LEN, 0);//û�н��ܵ���Ӧ��Ϣ���������Ҳ�����б仯

		std::vector<std::string> recvReponse = SplitBuf();
		int splitSize = recvReponse.size();

		switch (GUIState)
		{
		case State::start://�ý���������������Ҫ���ӷ�������ע��/��¼
			{
				//�ȶϿ�����
				closesocket(sockFd);//��SERVER_PORT���Ƕ����ӣ���һ�ξ͹�
				
				//�ж��յ�����Ϣ
				if(recvReponse[0] == "ALLOW_SIGNUP")//ͬ��ע��
				{
					hintMsg = "ע��ɹ���";
				}
				else if (recvReponse[0] == "EXIST")//�ܾ�ע��
				{
					hintMsg = "ע��ʧ��,���û����Ѵ���.";
					isRequestFailed = true;
				}
				else if (recvReponse[0] == "NOT_EXIST")//��¼���û���������
				{
					hintMsg = "�û���������.";
					isRequestFailed = true;
				}
				else if (recvReponse[0] == "ERROR")//������û�����ƥ��
				{
					hintMsg = "�û����������������.";
					isRequestFailed = true;
				}
				else if (recvReponse[0] == "ALREADY_ONLINE")//�û��Ѿ���½
				{
					hintMsg = "�����ڶ�̨�豸�ϵ�¼.";
					isRequestFailed = true;
				}
				else if (recvReponse[0] == "ALLOWED" && splitSize >= 9)//���Ե�¼
				{
					//����localUser
					int pkmSum = std::stoi(recvReponse[7]);//��������

					if (splitSize >= 9 + pkmSum)
					{
						//���챾��User
						CreateUser(recvReponse,*localUser);

						//�����¶˿�
						bool isConnected = ConnectHost(endpointPort);
						while (!isConnected)
						{
							isConnected = ConnectHost(endpointPort);
						}
						localUser->SetIsOnline(true);

						//���ӳɹ���ɽ�����Ϸ������
						GUIState = State::mainUI;
						hintMsg = "";
						//*curCheckUser = *localUser;
					}
					else
					{
						hintMsg = "����ʧ�ܣ������³���.";
						isRequestFailed = true;
					}
				}
				else if (recvReponse[0] == "SERVER_ERROR")//����������
				{
					hintMsg = "����ʧ�ܣ������³���.";
					isRequestFailed = true;
				}
				else
					;
			}
			break;

		case State::mainUI:
		case State::userUI:
			{
				if (recvReponse[0] == "USER_INFO" && splitSize >= 9)//�鿴�û�
				{
					int pkmSum = std::stoi(recvReponse[7]);//��������
					if (splitSize >= 9 + pkmSum)
					{
						//���쵱ǰ�鿴��User
						CreateUser(recvReponse, *curCheckUser);
						GUIState = State::userUI;
						hintMsg = "";
					}
					else
					{
						hintMsg = "����ʧ�ܣ������³���.";
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
					CreateEnemyPkm(recvReponse);//�������

					//���Կ�ʼս����
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
					myBattlePkm->Reset();//��ʼ�������ֵ
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
				if (recvReponse[0] == "DONE")//����Ǿ���������ʧȥ�ı����δ��û�����ɾ��
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
				else if (recvReponse[0] == "NEW_PKM" && splitSize >= 11)//ս��Ψһ�ı�����Ҳʧȥ�ˣ�ϵͳ���·���һֻ�µ�
				{
					localUser->LosePokemon(loseID);//��ɾȥԭ������ֻ

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

					//���뱳�������µĵ�ַ����������ͷ���
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
			hintMsg = "����ʧ�ܣ������³���.";
			isRequestFailed = true;
		}
		else
			;
	}
}

void Client::Draw()
{
	currentTime = clock();
	if (currentTime - lastUpdateTime >= UPDATE_INTERVAL && !isWaitingMsg)//������˸���ʱ�������͸��»���
	{
		BeginBatchDraw();

		lastUpdateTime = currentTime;

		std::string text;
		CString textCString;
		

		//���ݵ�ǰ������UI״̬������ʲôͼ
		switch (GUIState)
		{
		case State::start:
			{
				putimage(0, 0, &imgStart);

				font.lfHeight = 20; // ��������߶�
				settextstyle(&font);// ����������ʽ
				
				//����������û���������ʾ
				if (name != "")
				{
					textCString = name.c_str();
					outtextxy(131, 422, textCString);
				}
				else
					;

				//������������룬����ʾ8��*
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
				
				font.lfHeight = 26; // ��������߶�
				settextstyle(&font);// ����������ʽ

				localUser->DrawUserInfo(0, (int)GUIState);//��ӡ�û���Ϣ������
			}
			break;

		case State::userUI:
			{
				putimage(0, 0, &imgSeeUser);

				font.lfHeight = 26; // ��������߶�
				settextstyle(&font);// ����������ʽ

				curCheckUser->DrawUserInfo(localUser->GetID(),(int)GUIState);//��ӡ�û���Ϣ������
				curCheckUser->DrawUserList(curUserPage,totalUserSum);//��ӡ�û�ID�б�

			}
			break;

		case State::pokemonUI:
			{
				putimage(0, 0, &imgSeePokemon);

				font.lfHeight = 23; // ��������߶�
				settextstyle(&font);// ����������ʽ

				Pokemon* curCheckPkm;
				if (lastState == State::mainUI)
				{
					curCheckPkm = localUser->GetCurPkm(curCheckType, curCheckPkmSeq);
				}
				else
				{
					curCheckPkm = curCheckUser->GetCurPkm(curCheckType, curCheckPkmSeq);
				}
				
				curCheckPkm->DrawPkmInfo();//��ӡ��������Ϣ������
				curCheckPkm->DrawPkmList(curPkmPage, totalPkmSum, curCheckPkmSeq);//��ӡ�����α����б�

				int speciesID = curCheckPkm->GetSpeiesID();
				putimage(307, 25, &imgFrontCover[speciesID], SRCPAINT);
				putimage(307, 25, &imgFront[speciesID], SRCAND);
			}
			break;

		case State::choose1:
			{
				putimage(0, 0, &imgChoose[4]);

				font.lfHeight = 23; // ��������߶�
				settextstyle(&font);// ����������ʽ

				Pokemon* curCheckPkm = localUser->GetTotalBagPkm(curCheckPkmSeq);
				curCheckPkm->DrawPkmInfo();//��ӡ��������Ϣ������
				curCheckPkm->DrawPkmList(curPkmPage, totalPkmSum, curCheckPkmSeq);//��ӡ�����α����б�

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

				//�������ε�ͼ���ҷ���back���з���Front
				setaspectratio(2, 2);
				putimage(193, 33, &imgFrontCover[enemyID], SRCPAINT);//��������
				putimage(193, 33, &imgFront[enemyID], SRCAND);//����ԭͼ
				putimage(47, 101, &imgBackCover[myPkmID], SRCPAINT);//��������
				putimage(47, 101, &imgBack[myPkmID], SRCAND);//����ԭͼ
				setaspectratio(1, 1);

				//��ӡ����״̬
				font.lfHeight = 26; // ��������߶�
				settextstyle(&font);// ����������ʽ
				std::string text;
				CString textCString;
				switch (battleWeather)
				{
				case Weather::rain:
					text = "����(ˮϵ�ӳ�)";
					break;
				case Weather::sun:
					text = "����(�𡢲�ϵ�ӳ�)";
					break;
				case Weather::thunder:
					text = "����(��ϵ�ӳ�)";
					break;
				default:
					text = "��ͨ";
					break;
				}
				textCString = text.c_str();
				outtextxy(125, 29, textCString);

				//��ӡ������ʣ��Ѫ��������״��
				myBattlePkm->DrawBattle(false);
				enemyPkm->DrawBattle(true);
			}
			break;

		case State::win:
			{
				if (isUpgradeBattle)//���ֻ��������
				{
					putimage(0, 0, &imgWin[1]);
				}
				else//����Ǿ�����
				{
					putimage(0, 0, &imgWin[0]);
				}

				//��ӡ����ֵ
				font.lfHeight = 26; // ��������߶�
				settextstyle(&font);// ����������ʽ
				text = std::to_string(gainedEXP);
				textCString = text.c_str();
				outtextxy(385, 143, textCString);
				
			}
			break;

		case State::lose:
			{
				if (isUpgradeBattle)//���ֻ��������
				{
					putimage(0, 0, &imgLose[1]);
				}
				else//����Ǿ�����
				{
					putimage(0, 0, &imgLose[0]);

					//��������ͼ�������Ϣ
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

		if (isRequestFailed)//�������ʧ���ˣ������ʧ��ͼ��
		{
			putimage(228, 177, &imgFailed);
		}
		else
			;

		//��ӡ��ʾ��Ϣ
		font.lfHeight = 20; // ��������߶�
		settextstyle(&font);// ����������ʽ
		settextcolor(BLACK);
		textCString = hintMsg.c_str();
		outtextxy(352, 425, textCString);

		EndBatchDraw();
		
	}
	else
		;//���򲻸��»���
}

void Client::AutoBattle()
{
	currentTime = clock();

	int myHP = myBattlePkm->GetCurrentHP();
	int enemyHP = enemyPkm->GetCurrentHP();
	int myAttackDamage = 0;
	int enemyAttackDamage = 0;
	if (myHP > 0 && enemyHP > 0)//��˫��������Ѫ����ʱ��ս������
	{
		myAttackDamage = myBattlePkm->Attack(enemyPkm);
		enemyAttackDamage = enemyPkm->Attack(myBattlePkm);

		myBattlePkm->Damage(enemyAttackDamage);
		enemyPkm->Damage(myAttackDamage);
	}
	else
		;

	//����һ��û��Ѫ��ʱ��ս������������Ѫ���ж�˭��ʤ����,����������������Ϣ
	if (myHP > 0 && enemyHP <= 0)
	{
		//�ҷ���ʤ,��������������Ϣ
		HandleWinEvent();
	}
	else if (enemyHP > 0 && myHP <= 0)
	{
		//�з���ʤ
		HandleLoseEvent();
	}
	else if (myHP <= 0 && enemyHP <= 0 && myAttackDamage && enemyAttackDamage)
	{
		//˫��ͬһʱ������Ҷ��ԶԷ���������˭�ٶȿ�˭Ӯ

		if (myBattlePkm->GetSpeed() >= enemyPkm->GetSpeed())//�ҷ���ʤ
		{
			HandleWinEvent();
		}
		else//�з���ʤ
		{
			HandleLoseEvent();
		}
	}
	else
		;

}

//�����û���ʤ�¼�
void Client::HandleWinEvent()
{
	//�����û�ʤ������
	localUser->SetBattleTimes(true);

	//ת��GUI״̬
	GUIState = State::win;
	isWaitingMsg = true;

	//����Ҫ�Ӿ���
	gainedEXP = myBattlePkm->UpdateExp(enemyPkm);

	//����������������
	/*
		��¼���ݼ������������£�(�������Ļ�û�е�һ��)
		��õı�����ID|ʤ������ID	|  �ȼ�	| ����ֵ	| ����ֵ	| ����ֵ	| ����ֵ	| �ع�ֵ	| �ط�ֵ	| �ٶ�ֵ	|
			int		 |	int		|  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|
	*/
	//������������
	std::string sendBufString;
	if (isUpgradeBattle)//������
	{
		sendBufString = "WIN";
	}
	else//������
	{
		sendBufString = "WIN_PKM";
		sendBufString += ' ' + std::to_string(enemyPkm->GetIndividualID());

		//��սʤ�ı����μ��뱳��
		localUser->AddPokemonToBags(enemyPkm);
		*curCheckUser = *localUser;
	}
	sendBufString += ' ' + myBattlePkm->GetPokemonInfo();//�����˺�9��

	//����������������
	localUser->UpdateFullLevel();
	
	//����
	strcpy(socketBuffer, sendBufString.c_str());
	send(sockFd, socketBuffer, MAX_BUF_LEN, 0);
}

//�����û�ս��ʧ���¼�
void Client::HandleLoseEvent()
{
	//ת��GUI״̬
	GUIState = State::lose;

	//�����û�ʧ�ܴ���
	localUser->SetBattleTimes(false);

	if (isUpgradeBattle)//ֻ��������
	{
		std::string sendBufString = "LOSE";
		//����
		strcpy(socketBuffer, sendBufString.c_str());
		send(sockFd, socketBuffer, MAX_BUF_LEN, 0);
		isWaitingMsg = true;
	}
	else
	{
		//���û����������ѡ��ֻ����¼�ڱ����е��±꣬��������Ϊ-1
		//��ȫ��ʼ��Ϊ-1
		for (int i = 0; i < 3; i++)
		{
			loseSeq[i] = -1;
		}
		//���ȡ3ֻ
		for (int i = 0; i < 3; i++)
		{
			loseSeq[i] = localUser->RandPickLosePkm(loseSeq[0], loseSeq[1]);
		}

		//�ͷ��ͷŵз������εĿռ�
		delete enemyPkm;
		enemyPkm = nullptr;

		//ת��״̬��һЩ��ʼ��
		GUIState = State::lose;
		Pokemon* pkm = localUser->GetTotalBagPkm(loseSeq[0]);
		loseID = pkm->GetIndividualID();
		hintMsg = "����ǰѡ��Ҫʧȥ�ı�����Ϊ��" + pkm->GetName();

		isChooseLose = false;
		isOkRet = false;
	}
}

void Client::SendLoseMsg()
{
	//������������
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
	char userName[40];//���һ���ֹԽ��
	bool isConfirmed = InputBox(userName, 40, _T("�������û�����\n��0<����<=20,�Ҳ��ð����հ��ַ�)"), 0, 0, 0, 0, false);//�����ȡ���������ȡ����ֱ�ӷ���

	while (!IsNameValid(userName) && isConfirmed)
	{
		isConfirmed = InputBox(userName, 40, _T("�û�����ʽ����ȷ�����������룺\n��0<����<=20,�Ҳ��ð����հ��ַ�)"), 0, 0, 0, 0, false);//�����ȡ���������ȡ����ֱ�ӷ���
	}
	if (isConfirmed)//������ȷ�ϣ���˿̵��û����ǺϷ���
	{
		userNameString = userName;

		if (mode == 2)//ģʽ2�ǵ�¼��1��ע��
		{
			name = userNameString;
		}
		else
			;
	}
	else//������ȡ�������ô���
		;

	return userNameString;
}

std::string Client::InputPassword(int mode)
{
	password = "";

	char userPass[30];//���һ���ֹԽ��
	std::string userPassString = "";
	bool isConfirmed = InputBox(userPass, 30, _T("���������룺\n��6<=����<=20,��ֻ�������ֻ���ĸ)"), 0, 0, 0, 0, false);//�����ȡ���������ȡ����ֱ�ӷ���

	while (!IsPasswordValid(userPass) && isConfirmed)
	{
		isConfirmed = InputBox(userPass, 30, _T("�����ʽ����ȷ�����������룺\n��6<=����<=20,��ֻ�������ֻ���ĸ)"), 0, 0, 0, 0, false);//�����ȡ���������ȡ����ֱ�ӷ���
	}
	if (isConfirmed)//������ȷ�ϣ���˿̵��û����ǺϷ���
	{
		userPassString = userPass;

		if (mode == 2)//ģʽ2�ǵ�¼��1��ע��
		{
			password = userPassString;
		}
		else
			;
	}
	else//������ȡ�������ô���
		;

	return userPassString;
}

bool Client::IsNameValid(char userName[40]) const
{
	bool isValid = true;

	int len = strlen(userName);
	if (len == 0 || len > 20)//��鳤���Ƿ�ϸ�
	{
		isValid = false;
	}
	else
		;

	for (int i = 0; i < len && isValid; i++)
	{
		if (userName[i] == '\t' || userName[i] == '\n' || userName[i] == '\b')//����Ƿ��в��ϸ��ַ�
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
	if (len < 6 || len > 20)//��鳤���Ƿ�ϸ�
	{
		isValid = false;
	}
	else
		;

	for (int i = 0; i < len && isValid; i++)
	{

		if (!((userPass[i] >= 'a' && userPass[i] <= 'z')
			|| (userPass[i] >= 'A' && userPass[i] <= 'Z')
			|| (userPass[i] >= '0' && userPass[i] <= '9')))//����Ƿ��в��ϸ��ַ�
		{
			isValid = false;
		}
		else
			;
	}

	return isValid;
}

//�ָ���յ������ݣ������Կո�' '���ָ���
std::vector<std::string> Client::SplitBuf() const
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

//��ʼ��Ҫ��ӡ�ڽ����ϵ�����
void Client::SetFont()
{
	//��������
	gettextstyle(&font);// ��ȡ��ǰ��������
	font.lfWeight = FW_BOLD;
	font.lfQuality = PROOF_QUALITY;    // �������Ч��Ϊ����� 
	_tcscpy_s(font.lfFaceName, _T("΢���ź�"));
	settextstyle(&font);           // ����������ʽ
}

void Client::CreateUser(std::vector<std::string> recvReponse, User& user)
{
	if (recvReponse[0] == "USER_INFO")//�������ǵ�ǰ�鿴���û�
	{
		totalUserSum = std::stoi(recvReponse[1]);
	}
	else//�������Ǳ����û�
	{
		endpointPort = std::stoi(recvReponse[1]);
	}


	//ÿ�ι������û�֮ǰ�����Ȱ�ԭ�û��ı����ռ��ͷ�
	user.ClearBags();
	user.SetUser(recvReponse);

	//���¶�����屦�����ļ����������޸ģ�
	LoadIndividualPKM();


	//�����û����������͸��º�ĸ��屦���ο���û�������ӱ�����
	int sum = user.GetPkmSum();
	user.SetPkmSum(0);
	for (int i = 9; i < 9 + sum; i++)//recvResponse�±��9��ʼ�Ǳ����εĸ���ID
	{
		int seq = std::stoi(recvReponse[i]);
		user.AddPokemonToBags(individualPokemon[seq]);
	}
}

void Client::CreateEnemyPkm(std::vector<std::string> recvReponse)
{
	//ÿ�ι���֮ǰҪ�Ȱ�ԭ�����ͷ�
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

	//�������������ָ��Ȼ��ǿ��ת��Ϊ����ָ��
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
	//�������������Ϣ����ֱ�ӽ�������
	isLogOut = true;

	std::string sendBufString = "LOG_OUT";
	strcpy(socketBuffer, sendBufString.c_str());
	send(sockFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ�������

	closesocket(sockFd);
}

//����鿴�û�
void Client::HandleCheckUser(std::string mode, int id)
{
	//�����˷�����Ϣ,���յ����ź��ڽ���ҳ�����ת
	std::string sendBufString = "SEE_OTHER_USERS";
	sendBufString += ' ' + mode;

	if (mode == "ONE")//������л��鿴��ǰ���û����������Ҫ�鿴���û���ID
	{
		sendBufString += ' ' + std::to_string(id);
	}
	else
		;

	strcpy(socketBuffer, sendBufString.c_str());
	send(sockFd, socketBuffer, MAX_BUF_LEN, 0);//���ͷ�����Ϣ�������

	isWaitingMsg = true;

	curUserPage = 0;
}

//����鿴ĳ�������ı�����
void Client::HandleCheckPokemon(int typeNum)
{
	//ȷ������ı������ĸ�����
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

	//�ж������Ƿ�Ϊ0�������򲻷�Ӧ/��ʾ�ޱ����Σ���������ת��
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
		hintMsg = "û�и����͵ı�����>_<..";
	}
}

bool Client::ConnectHost(int port)
{
	//�����׽��֣���ַ��ΪInernet�����ַ��socket�������ݸ�ʽΪ����Э����TCP
	sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�˴�Ϊ�����׽���
	if (sockFd == INVALID_SOCKET)
	{
		closesocket(sockFd);
		WSACleanup();
		return false;
	}
	else
		;

	//����������ӣ�ֻ�������ӽ����ɹ������ʱconnect�����ŷ���
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (connect(sockFd, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) != -1)//���ӳɹ�
	{
		return true;
	}
	else//����ʧ�ܣ����׽ӿڽ�������ʹ�ã�����ر�
	{
		closesocket(sockFd);
		return false;
	}
}