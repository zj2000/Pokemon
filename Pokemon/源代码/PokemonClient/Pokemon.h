#ifndef POKEMON_H
#define POKEMON_H


#include <vector>
#include <easyx.h>

#include "Move.h"

enum class Attribute { highHP, highPhysicalAttcak, highPhysicalDefence, highSpecialAttack, highSpecialDefence, highSpeed };

extern IMAGE imgFront[48];//小精灵战斗时显示的图（前方）,原图
extern IMAGE imgFrontCover[48];//小精灵战斗时显示的图（前方）,掩码图
extern IMAGE imgBack[48];//小精灵战斗时显示的图（后方）,原图
extern IMAGE imgBackCover[48];//小精灵战斗时显示的图（后方）,掩码图

class Pokemon
{
protected:
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

	//对战相关
	int currentHP;		//对战时剩余的生命值
	int accuracyLevel;	//命中率等级，对战开始和结束时置为0，根据对战会有所变化，取值范围在0～6，超出范围取范围的最值
	int evasionLevel;	//闪避率等级，对战开始和结束时置为0，根据对战会有所变化，取值范围在0～6，超出范围取范围的最值
	bool isAtCriticalState;//是否处于易中要害状态，是的话可以提高暴击率
	int criticalLeftTimes;//剩余可以处于易中要害状态的次数
	clock_t lastAttackTime;//上次出招的时间，用于计算间隔
	std::string moveName; //某次出招的招式名称
	int damage;		//某次出招给对方带来的伤害
	bool isHit;		//是否击中对方
	bool isCritical;//是否暴击


	//种族相关（值与所有该种类个体宝可梦一致）
	int speciesID;	//种族ID
	int baseEXP;	//基础经验值，用于计算对战胜利后的经验值提升
	int growPoints;	//升级时各项指标提升的值
	std::string name;//精灵名字	

	Attribute mainAttribute;//主属性：高攻/高防/高血/高速
	Type PokemonType;//类别：水/火/草/土/电

	int learnedMoves[4];//该小精灵掌握的招式,一只宝可梦最多只能同时记住4个招式

public:
	Pokemon(const Pokemon& copy);
	Pokemon(int id, std::string name, std::string type, std::string mainA, int base, int grow, int attr[6],int move[4]);//种族的构造函数
	Pokemon(int myID, int speciesID, int level, int EXP, int attr[6]);//个体的构造函数

	//战斗相关
	void UpdateAttribute();//升级函数
	int AutoMove() const;//随机出招,返回值为招式在招式库下标
	virtual int Attack(Pokemon* enemy);//攻击函数,计算给对方的伤害加成,并返回伤害值
	void Damage(int damage);//小精灵受到攻击时的伤害计算
	int UpdateExp(Pokemon* enemyPokemon);//对战胜利后，更新经验值,并自动升级
	void Reset();//战斗开始前重置战斗相关数值
	int ComputeFinalDamage(double bonus, Move* attackMove, Pokemon* enemy) const;
	double ComputeAccuracyRate() const;//计算并返回命中率
	double ComputeEvasionRate(Pokemon* enemy) const;//计算并返回闪避率
	double ComputeCriticalRate(Move* attackMove);//计算并返回暴击率
	bool whetherHit(Pokemon* enemy, int moveAccuracy) const;//判断并返回对战时是否命中对方
	double GetRandomFactor(int min) const;//返回一个随机小数，该随机小数的范围是min/100 - 1
	int GetCurrentHP() const;//返回剩余血量

	//获得宝可梦信息
	Type GetType()const;//类别
	int GetLevel() const;//等级
	int GetIndividualID()const;//个体ID
	int GetSpeiesID()const;//种族ID
	std::string GetName() const;//名字
	int GetSpeed() const;//速度
	std::string GetTypeString(Type type) const;
	std::string GetAttributeString() const;
	std::string GetPokemonInfo() const;//综合信息

	//界面相关
	void DrawPkmInfo() const;//绘制综合信息到界面上
	void DrawPkmList(int curPage, int total, int curCheckSeq) const;//绘制宝可梦背包列表
	void DrawLose(int seq) const;//打印要失去的宝可梦
	void DrawBattle(bool isAtFront) const;//打印战斗相关信息到界面

};

#endif // !POKEMON_H
