#ifndef SERVER_H
#define SERVER_H

#include "Endpoint.h"
#include "User.h"

#include <fstream>


const int SERVER_PORT = 8000;//�������˿ں�
const int MAX_QUEUE_LEN = 4;//������󳤶ȣ�����������ȣ��ں˿�ʼ�ܾ���������
const int MAX_BUF_LEN = 1024;//socket�������󳤶�
const int NOT_EXIST = -1;
const std::string POKEMON_SPECIES_LOC = "./res/pokemon.dat";//ֻ���ļ������޸�
const std::string POKEMON_INDIVIDUAL_LOC = "./res/pokemonIndividual.dat";
//const std::string MOVE_LOC = "./res/moves.dat";//ֻ���ļ������޸�


extern std::vector<Pokemon*> pokemonLibrary;
extern std::vector<Pokemon*> individualPokemon;//���屦���Σ������û��ĺ�ϵͳ���ɵ�

class Server
{
	//socket���
	SOCKET listenFd;	//�����׽��֣����������ļ���������,���Խ������Կͻ��˵���������
	SOCKET connectFd;	//�������׽��֣����������ļ���������,���Ա�������ͻ��˽���ͨ��
	char socketBuffer[1024];//���ݻ���
	
	volatile bool isRunning;//�������Ƿ����ڹ���

	//�û��������
	std::string userFileLoc;//�û������ļ���ַ�������û��������뼰ӵ�б����ε���Ϣ
	std::vector<User*> userList;//�û���

	std::vector<Endpoint*> serverEndpoints;//����ʵ�ֶ��û���¼���⣬TCP���ӵķ������˵�

	std::mutex serverMutex;		//�����������̣߳�������������


	//�������ֹ����ɿͻ������ӵ����������˿ڣ�SERVER_PORT�����
	void SignUp(std::string userName, std::string password);//ע��
	void LogIn(std::string userName, std::string password);//��¼
	int CheckExistence(std::string userName) const;//���ĳ�û����Ƿ����
	

	
	Server(const Server&) = delete;				//���ÿ������캯��,ֻ������һ��������
	Server& operator=(const Server&) = delete;	//���ÿ�����ֵ������,ֻ������һ��������

	//���߳��йصĺ���
	void Communicate();//���Ӳ�ͨ��
	void End();//��ֹ
	void ManageEndpoint(Endpoint* endpoint);//����endpoint�����Ӻ�����
	std::vector<std::string> SplitBuf() const;//�ָ��յ�������

	//�й��ļ���д
	void LoadUsersData();//�����ļ����ݵ��ڴ�
	void LoadSpeciesPokemon()const;//�������屦���ε����Ͽ�
	void LoadIndividualPKM()const;//���ظ��屦���ε��ڴ�
	


public:
	Server();//����
	~Server();//����

	bool InitServer();//��ʼ�����������������ɹ��򷵻�true��ʧ�ܷ���false
	void Run();//���з�������ִ���߳��Լ����߳�ִ����Ϻ��������Ӷ˵�,���رշ�����

	void SetUserIsOnline(int id, bool isOnline);//����û��Ƿ�����
	std::string GetUserInfo(int id) const;//����ĳid����ϸ��Ϣ

	std::string CreateBattlePkm(int species, int lv,bool isUpgrateBattle)const;//���ͻ��˹���һ�����ֲ�����

	void RewriteUserFile();//��д�ļ�����
	void RewritePkmFile();//��д���屦�����ļ�

	std::string UpdateUser(int userid, int pkmid, bool isAdd);//�����û���Ϣ
	void SetUserTimes(int userID,bool isWin);//�����û�ս����Ϣ
};

#endif // !SERVER_H