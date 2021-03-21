#ifndef POKEMON_TYPE_H
#define POKEMON_TYPE_H

#include "Pokemon.h"

enum class Weather { rain, sun, thunder,normal};//天气可以影响对战

//属性：草克电，电克水，水克火，火克草。
class WaterPokemon : public Pokemon
{
	Type advantageType; //本属性克制的属性
	Type disadvantageType;//克制本属性的属性
public:
	WaterPokemon(const Pokemon& pkm);
	int Attack(Pokemon* enemy) override;//攻击函数,计算给对方的伤害加成,并返回伤害值
};

class FirePokemon : public Pokemon
{
	Type advantageType; //本属性克制的属性
	Type disadvantageType;//克制本属性的属性
public:
	FirePokemon(const Pokemon& pkm);
	int Attack(Pokemon* enemy) override;//攻击函数,计算给对方的伤害加成,并返回伤害值
};

class GrassPokemon : public Pokemon
{
	Type advantageType; //本属性克制的属性
	Type disadvantageType;//克制本属性的属性
public:
	GrassPokemon(const Pokemon& pkm);
	int Attack(Pokemon* enemy) override;//攻击函数,计算给对方的伤害加成,并返回伤害值

};

class ElectricPokemon : public Pokemon
{
	Type advantageType; //本属性克制的属性
	Type disadvantageType;//克制本属性的属性
public:
	ElectricPokemon(const Pokemon& pkm);
	int Attack(Pokemon* enemy) override;//攻击函数,计算给对方的伤害加成,并返回伤害值
};


#endif // !POKEMON_TYPE_H

