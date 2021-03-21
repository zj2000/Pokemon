#ifndef POKEMON_H
#define POKEMON_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <easyx.h>

//�������˵ı�������
class Pokemon
{
	//������أ�����֮�����������ֵ��
	int myID;	//����С������
	int level;	//�ȼ�
	int EXP;	//����ֵ

	//����ֵ
	int healthPoint;	//����ֵ	
	int attackPoint;	//����ֵ--����
	int defencePoint;	//����ֵ--����
	int specialAttack;	//�ع�ֵ--����
	int specialDefence;	//�ط�ֵ--����
	int attackSpeed;	//�ٶ�
	int attackInterval;	//�������(= �̶�ʱ�� �� �ٶ�),ʱ�䵥λ�Ǻ���

	//������أ�ֵ�����и�������屦����һ�£�
	int speciesID;	//����ID
	int baseEXP;	//��������ֵ�����ڼ����սʤ����ľ���ֵ����
	int growPoints;	//����ʱ����ָ��������ֵ
	std::string name;//��������	

	std::string mainAttribute;//�����ԣ��߹�/�߷�/��Ѫ/����
	std::string PokemonType;//���ˮ/��/��/��

	int learnedMoves[4];//��С�������յ���ʽ,һֻ���������ֻ��ͬʱ��ס4����ʽ

public:
	//����
	Pokemon(const Pokemon& copy);
	Pokemon(int id, std::string name, std::string type, std::string mainA, int base, int grow, int attr[6],int move[4]);//����Ĺ��캯��
	Pokemon(int myID, int speciesID, int level, int EXP, int attr[6]);//����Ĺ��캯��
	void InitIndividual(int individualID, int level);//��ʼ���ɵı����γ�ʼ������id�͵ȼ�
	void SetAttr(int attr[8]);//���ݿͻ��˴������������ú͸��±�������Ϣ
	void UpdateAttribute(int _level);//���±���������ֵ
	
	int GetIndividualID()const;//��ñ����θ���ID
	int GetLevel() const;//��ñ����εȼ�
	double GetRandomFactor(int min) const;//����һ�����С���������С���ķ�Χ��min/100 - 1

	//�ļ�
	void AppendIndividualFile() const;//׷�ӱ������ļ�
	bool CheckEmpty(std::string) const;//����ļ��Ƿ�Ϊ��
	std::string GetPokemonInfo() const;//��ñ�������Ϣ
};

#endif // !POKEMON_H

