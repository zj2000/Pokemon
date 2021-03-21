#ifndef USER_H
#define USER_H

#include "Pokemon.h"

class Medal
{
	std::string medalName;
};

class User
{
	int id;
	int winTimes;	//��ʤ����
	int loseTimes;	//ʧ�ܴ���

	int pokemonSum;//ӵ�б�������
	int fullLevelSum;//ӵ�и߼�����������15����

	bool isOnline;//���û���ǰ�Ƿ�����

	std::string name; //�û���������Ψһ
	std::string password; // ���룬�Ƿ�Ҫ�޶���׼

	std::vector<Pokemon*> pokemonBags;//���û��ľ��鱳��

public:
	User(int id, std::string name, std::string password);//����
	User(int id, std::string name, std::string password,int winTimes, int loseTimes,int fulllevel);

	int GainPokemon(int first, int second, int level);//��ʼ��ñ�����,����ֵΪ��õı����ε�����ID
	std::string GetName()const;//��ȡ���û�������
	int GetID()const;//��ȡ���û���ID
	std::string GetPassword() const;//��ȡ���û�������
	std::string GetUserInfo(bool isForFile) const;//��ȡ���û�����Ϣ

	void SetIsOnline(bool isUserOnline);//���ø��û�����
	void SetBattleTimes(bool isWin);//����ʤ��/ʧ�ܴ���

	void AddPokemonToBags(Pokemon* newPokemon);//�����û��ı�����ӱ�����
	int LosePokemon(int pkmID);//�û�ս��ʧȥ������

	//�ļ�
	bool CheckEmpty(std::string loc)const;
	void AppendUserFile(std::string loc) const;//�����û�
	
};

#endif // !USER_H

