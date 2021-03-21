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
#pragma warning(disable:4996)//�������ʹ��strcpy

enum class State{start, mainUI,userUI,pokemonUI,battle, choose1, choose2,choose3,win,lose};//UI״̬
enum class Mode{low,mid,high};

class Client
{
	//�й�ͨ��
	SOCKET sockFd; //�ͻ����׽���
	char socketBuffer[1024];
	bool isWaitingMsg;//���λ���Ƿ����ڵȴ�����˵Ļ���
	bool isRequestFailed;//�����Ƿ�ʧ��
	int endpointPort;//���𱾿ͻ��˵ķ���˵�Ķ˿ں�
	
	//�й��û�
	std::string name;//���ͻ����û���
	std::string password;//���ͻ����û�����
	User* localUser;//�����û�
	User* curCheckUser;//��ǰ���ڲ鿴���û�
	
	//�йؽ���
	int curUserPage;//��ǰ�鿴���û�ҳ
	int totalUserSum;//ϵͳ�е��û�����
	int curCheckPkmSeq;//��ǰ�鿴�ı��������б���±��
	Type curCheckType;//��ǰ�鿴�ı����ε����
	int curPkmPage;//��ǰ�鿴�ı�����ҳ��
	int totalPkmSum;//��ǰ�鿴�ı����α�����ӵ�еı���������
	std::string hintMsg;//������ʾ��Ϣ
	State lastState;//��һ������״̬

	//�й�ս��
	bool isUpgradeBattle;//�Ƿ�����������true����������false�Ǿ�����
	Mode chooseMode;//ѡ����ս��ģʽ������/�м�/�߼�
	int myPkmID;//��ս�ı����ε�����ID
	int enemyID;//���ֵ�����id
	Pokemon* myBattlePkm;//��ս�ı�����
	Pokemon* enemyPkm;//���ֱ�����
	
	int gainedEXP;//սʤ��õľ���ֵ
	int loseSeq[3];//��ѡ����ֻ����ʧȥ�ı��������ܱ����е��±�,Ϊ-1ʱ��������
	int loseID;//ʧȥ�ı����εĸ���ID
	int loseSequence;//ʧȥ�ı����ε������±�
	bool isChooseLose;//�Ƿ���ȷ��Ҫʧȥ�ı�����
	bool isOkRet;//�Ƿ���Է���

	

	//�ļ�����
	void LoadSpeciesPokemon();//�������屦���ε����Ͽ�
	void LoadIndividualPKM();//���ظ��屦���ο�
	void LoadMoves();//������ʽ��

	//�й�ս������ʱ�Ĵ���
	void HandleWinEvent();//ս��ʤ��
	void HandleLoseEvent();//ս��ʧ��
	void SendLoseMsg();//���͸������ս����Ϣ

	std::vector<std::string> SplitBuf() const;//�ָ���յ�������

	//���������й�
	void InitCanves();//��ʼ��������װ��ͼƬ��Դ
	void SetFont();//��������

	//�ӷ��������ص���Ϣ����������ݽṹ
	void CreateUser(std::vector<std::string> recvReponse, User& user);//�����û�
	void CreateEnemyPkm(std::vector<std::string> recvReponse);//����ս���ĵз�������

	//�˳���¼
	void LogOut();

	//����¼�
	void HandleCheckPokemon(int typeNum);//���������鿴�����Ρ����¼�
	void HandleCheckUser(std::string mode,int id);//���������鿴�û������¼�

	//���ڵ�¼/ע��ʱ������
	std::string InputName(int mode);//�����û���
	std::string InputPassword(int mode);//��������
	bool IsNameValid(char userName[40]) const;//����û����Ƿ�Ϸ�
	bool IsPasswordValid(char userPass[30]) const;//��������Ƿ�Ϸ�

	//ͼ��
	bool IsBetween(int left, int right, int top, int bottom, int x, int y) const;

	//�������������
	bool ConnectHost(int port);


public:
	Client();
	~Client();
	bool InitWSA();

	//��Ҫ���ݵ�ǰ״̬�ĺ���
	void HandleClickEvent();//�����û���������¼�
	void RecvServerMsg();//���ݵ�ǰ������״̬�������˷��ص���Ϣ
	void Draw();//���ƻ���

	//�Զ�ģ��ս��
	void AutoBattle();//�Զ����ж�ս

};

#endif // !CLIENT_H