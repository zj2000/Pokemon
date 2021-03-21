#ifndef POKEMON_TYPE_H
#define POKEMON_TYPE_H

#include "Pokemon.h"


//属性：草克土，土克电，电克水，水克火，火克草。
class WaterPokemon : public Pokemon
{
	//Type myType; 
	std::string typeName;
	Type advantageType; //本属性克制的属性
	Type disadvantageType;//克制本属性的属性
public:
	WaterPokemon(const Pokemon& pkm);
	int Attack(Move* attackMove, Pokemon* enemy) override;//攻击函数,计算给对方的伤害加成,并返回伤害值
	//WaterPokemon* CreateWaterPokemon(int id);//生成一个水系宝可梦，输入参数为某种类宝可梦的标号，返回生成的个体宝可梦的指针
	//void Damage() override;//小精灵受到攻击时的伤害计算
};

class FirePokemon : public Pokemon
{

	//Type myType; 
	std::string typeName;
	Type advantageType; //本属性克制的属性
	Type disadvantageType;//克制本属性的属性
public:
	FirePokemon(const Pokemon& pkm);
	int Attack(Move* attackMove, Pokemon* enemy) override;//攻击函数,计算给对方的伤害加成,并返回伤害值
};

class GrassPokemon : public Pokemon
{

	//Type myType; 
	std::string typeName;
	Type advantageType; //本属性克制的属性
	Type disadvantageType;//克制本属性的属性
public:
	GrassPokemon(const Pokemon& pkm);
	int Attack(Move* attackMove, Pokemon* enemy) override;//攻击函数,计算给对方的伤害加成,并返回伤害值

};

class GroundPokemon : public Pokemon
{

	//Type myType; 
	std::string typeName;
	Type advantageType; //本属性克制的属性
	Type disadvantageType;//克制本属性的属性
public:
	GroundPokemon(const Pokemon& pkm);
	int Attack(Move* attackMove, Pokemon* enemy) override;//攻击函数,计算给对方的伤害加成,并返回伤害值
};

class ElectricPokemon : public Pokemon
{

	//Type myType; 
	std::string typeName;
	Type advantageType; //本属性克制的属性
	Type disadvantageType;//克制本属性的属性
public:
	ElectricPokemon(const Pokemon& pkm);
	int Attack(Move* attackMove, Pokemon* enemy) override;//攻击函数,计算给对方的伤害加成,并返回伤害值
};


#endif // !POKEMON_TYPE_H

