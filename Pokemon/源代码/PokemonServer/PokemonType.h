#ifndef POKEMON_TYPE_H
#define POKEMON_TYPE_H

#include "Pokemon.h"


//���ԣ��ݿ��������˵磬���ˮ��ˮ�˻𣬻�˲ݡ�
class WaterPokemon : public Pokemon
{
	//Type myType; 
	std::string typeName;
	Type advantageType; //�����Կ��Ƶ�����
	Type disadvantageType;//���Ʊ����Ե�����
public:
	WaterPokemon(const Pokemon& pkm);
	int Attack(Move* attackMove, Pokemon* enemy) override;//��������,������Է����˺��ӳ�,�������˺�ֵ
	//WaterPokemon* CreateWaterPokemon(int id);//����һ��ˮϵ�����Σ��������Ϊĳ���౦���εı�ţ��������ɵĸ��屦���ε�ָ��
	//void Damage() override;//С�����ܵ�����ʱ���˺�����
};

class FirePokemon : public Pokemon
{

	//Type myType; 
	std::string typeName;
	Type advantageType; //�����Կ��Ƶ�����
	Type disadvantageType;//���Ʊ����Ե�����
public:
	FirePokemon(const Pokemon& pkm);
	int Attack(Move* attackMove, Pokemon* enemy) override;//��������,������Է����˺��ӳ�,�������˺�ֵ
};

class GrassPokemon : public Pokemon
{

	//Type myType; 
	std::string typeName;
	Type advantageType; //�����Կ��Ƶ�����
	Type disadvantageType;//���Ʊ����Ե�����
public:
	GrassPokemon(const Pokemon& pkm);
	int Attack(Move* attackMove, Pokemon* enemy) override;//��������,������Է����˺��ӳ�,�������˺�ֵ

};

class GroundPokemon : public Pokemon
{

	//Type myType; 
	std::string typeName;
	Type advantageType; //�����Կ��Ƶ�����
	Type disadvantageType;//���Ʊ����Ե�����
public:
	GroundPokemon(const Pokemon& pkm);
	int Attack(Move* attackMove, Pokemon* enemy) override;//��������,������Է����˺��ӳ�,�������˺�ֵ
};

class ElectricPokemon : public Pokemon
{

	//Type myType; 
	std::string typeName;
	Type advantageType; //�����Կ��Ƶ�����
	Type disadvantageType;//���Ʊ����Ե�����
public:
	ElectricPokemon(const Pokemon& pkm);
	int Attack(Move* attackMove, Pokemon* enemy) override;//��������,������Է����˺��ӳ�,�������˺�ֵ
};


#endif // !POKEMON_TYPE_H

