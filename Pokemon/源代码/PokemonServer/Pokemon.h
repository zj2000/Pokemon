#ifndef POKEMON_H
#define POKEMON_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <easyx.h>

//服务器端的宝可梦类
class Pokemon
{
	//个体相关（个体之间有所差异的值）
	int myID;	//个体小精灵编号
	int level;	//等级
	int EXP;	//经验值

	//属性值
	int healthPoint;	//生命值	
	int attackPoint;	//攻击值--物理
	int defencePoint;	//防御值--物理
	int specialAttack;	//特攻值--特殊
	int specialDefence;	//特防值--特殊
	int attackSpeed;	//速度
	int attackInterval;	//攻击间隔(= 固定时间 ÷ 速度),时间单位是毫秒

	//种族相关（值与所有该种类个体宝可梦一致）
	int speciesID;	//种族ID
	int baseEXP;	//基础经验值，用于计算对战胜利后的经验值提升
	int growPoints;	//升级时各项指标提升的值
	std::string name;//精灵名字	

	std::string mainAttribute;//主属性：高攻/高防/高血/高速
	std::string PokemonType;//类别：水/火/草/电

	int learnedMoves[4];//该小精灵掌握的招式,一只宝可梦最多只能同时记住4个招式

public:
	//构造
	Pokemon(const Pokemon& copy);
	Pokemon(int id, std::string name, std::string type, std::string mainA, int base, int grow, int attr[6],int move[4]);//种族的构造函数
	Pokemon(int myID, int speciesID, int level, int EXP, int attr[6]);//个体的构造函数
	void InitIndividual(int individualID, int level);//初始生成的宝可梦初始化个体id和等级
	void SetAttr(int attr[8]);//根据客户端传来的数据设置和更新宝可梦信息
	void UpdateAttribute(int _level);//更新宝可梦属性值
	
	int GetIndividualID()const;//获得宝可梦个体ID
	int GetLevel() const;//获得宝可梦等级
	double GetRandomFactor(int min) const;//返回一个随机小数，该随机小数的范围是min/100 - 1

	//文件
	void AppendIndividualFile() const;//追加宝可梦文件
	bool CheckEmpty(std::string) const;//检查文件是否为空
	std::string GetPokemonInfo() const;//获得宝可梦信息
};

#endif // !POKEMON_H

