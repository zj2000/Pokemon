#ifndef USER_H
#define USER_H

#include "PokemonType.h"

//�ͻ��˵��û���
class User
{
	int userID;

	bool isOnline;//�Ƿ�����

	int winTimes;
	int loseTimes;

	int pokemonSum;
	int fullLevelSum;

	std::string name; //�û���������Ψһ

	std::vector<Pokemon*>totalPokemonBags;
	std::vector<WaterPokemon*> waterPokemonBags;//ˮϵ�����α���
	std::vector<FirePokemon*> firePokemonBags;//��ϵ�����α���
	std::vector<GrassPokemon*> grassPokemonBags;//��ϵ�����α���
	std::vector<ElectricPokemon*> electricPokemonBags;//��ϵ�����α���
public:
	User();
	User& operator=(const User&);
	void SetUser(std::vector<std::string> recvReponse);//���ݷ���˴��������������û��������Ϣ
	void SetIsOnline(bool isonline);//�����û��������
	void ClearBags();//��ձ���
	void AddPokemonToBags(Pokemon* newPokemon);//�ѱ�������ӵ��ܱ�������𱳰���
	void LosePokemon(int individualID);//ɾ��������ս�ܵ�ʧȥ�ı�����
	void UpdateFullLevel();//����ӵ�е�������������
	void SetBattleTimes(bool isWin);//����ʤ��/ʧ�ܴ���
	void SetPkmSum(int sum);//�����û�ӵ�еı����ε�����

	double GetWinRate() const; //��ȡ���û���ʤ��
	int HasPokemon(Type type) const;//����ĳ�����еı�������
	int GetID() const;//��ȡ�û���ID
	Pokemon* GetCurPkm(Type type, int seq) const;//��ȡ��ǰ�鿴�ı�����
	int GetPkmSum() const;//��ȡ�û�ӵ�еı�������
	Pokemon* GetTotalBagPkm(int seq)const;///��ȡ������

	void DrawUserInfo(int localID, int state) const;//��ӡ�û���Ϣ��������
	void DrawUserList(int curPage, int total) const;//��ӡ�û��б�����

	int RandPickLosePkm(int first, int second) const;//���ѡȡҪʧȥ�ı�����
};

#endif // !USER_H
