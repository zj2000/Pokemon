#ifndef POKEMON_TYPE_H
#define POKEMON_TYPE_H

#include "Pokemon.h"

enum class Weather { rain, sun, thunder,normal};//��������Ӱ���ս

//���ԣ��ݿ˵磬���ˮ��ˮ�˻𣬻�˲ݡ�
class WaterPokemon : public Pokemon
{
	Type advantageType; //�����Կ��Ƶ�����
	Type disadvantageType;//���Ʊ����Ե�����
public:
	WaterPokemon(const Pokemon& pkm);
	int Attack(Pokemon* enemy) override;//��������,������Է����˺��ӳ�,�������˺�ֵ
};

class FirePokemon : public Pokemon
{
	Type advantageType; //�����Կ��Ƶ�����
	Type disadvantageType;//���Ʊ����Ե�����
public:
	FirePokemon(const Pokemon& pkm);
	int Attack(Pokemon* enemy) override;//��������,������Է����˺��ӳ�,�������˺�ֵ
};

class GrassPokemon : public Pokemon
{
	Type advantageType; //�����Կ��Ƶ�����
	Type disadvantageType;//���Ʊ����Ե�����
public:
	GrassPokemon(const Pokemon& pkm);
	int Attack(Pokemon* enemy) override;//��������,������Է����˺��ӳ�,�������˺�ֵ

};

class ElectricPokemon : public Pokemon
{
	Type advantageType; //�����Կ��Ƶ�����
	Type disadvantageType;//���Ʊ����Ե�����
public:
	ElectricPokemon(const Pokemon& pkm);
	int Attack(Pokemon* enemy) override;//��������,������Է����˺��ӳ�,�������˺�ֵ
};


#endif // !POKEMON_TYPE_H

