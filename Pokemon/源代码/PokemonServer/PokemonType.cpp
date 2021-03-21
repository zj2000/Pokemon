#include "MainHeader.h"

WaterPokemon::WaterPokemon(const Pokemon& pkm) : Pokemon(pkm)
{
	//myType = Type::water;
	advantageType = Type::fire;
	disadvantageType = Type::electric;
	typeName = "水";
}

int WaterPokemon::Attack(Move* attackMove, Pokemon* enemy)
{
	/*
		伤害 = （（2×等级+5）/40 × 攻击/防御 × 威力 +2）× 加成

		等级 是攻击方宝可梦的等级。
		攻击与防御 分别是攻击方的攻击或者特攻（取决于是物理/特殊招式），和防守方的防御或特防（取决于招式是物理/特殊招式）
		加成 使用如下的公式计算：加成=属性一致加成×属性相克造成的倍率×击中要害的倍率×其他加成×随机数（随机数∈[0.85，1]）
		当有属性一致加成效果时该值为1.5，否则为1。
		属性相克造成的倍率可以为0.5、1、2。
		当击中要害（即暴击）时，该值为3⁄2,如果没有击中要害，该值为1。
		其他加成 是指特性加成、道具加成、天气和状态加成、目标数量加成等的乘积。？？？暂时没用到
		最后计算出的伤害要进行无条件舍去（即向下取整），但至少会造成1ＨＰ的伤害。
	*/

	//计算加成部分
#pragma region compute bonus
	double bonus = GetRandomFactor(85);//初始化为加成中的随机数部分
	Type attackType = attackMove->GetType();//招式的属性

	//计算属性一致加成
	if (attackType == PokemonType)//如果招式属性与本宝可梦属性一致有1.5倍的加成
	{
		bonus *= 1.5;
	}
	else//否则为1
		;

	//计算属性相克造成的倍率
	if (attackType == advantageType)//若敌方宝可梦的属性 被 本招式的属性 所克制,倍率为2
	{
		bonus *= 2;
	}
	else if (attackType == disadvantageType)//若本招式的属性 被 敌方宝可梦的属性 所克制,倍率为0.5
	{
		bonus *= 0.5;
	}
	else//其他情况倍率为1
		;

	//计算击中要害(暴击)的倍率
	int criticalRate = static_cast<int>(ComputeCriticalRate(attackMove) * 100);//将暴击率从0-1的小数转换成0-100的整数

	//随机生成一个0-100的整数，如果这个数小于等于暴击率，暴击倍率为1.5
	srand(clock());
	int randomNum = rand() % 100;
	if (randomNum <= criticalRate)
	{
		bonus *= 1.5;
	}
	else
		;
#pragma endregion

	//计算伤害值,伤害 = （（2×等级 + 5） / 40 × 攻击 / 防御 × 威力 + 2）× 加成

	//根据招式攻击的类型判断参与计算的攻击是物攻还是特攻，防御是物防还是特防
	int atk = 1;//要参与伤害计算的攻击值
	int def = 1;//要参与伤害计算的防御值
	if (attackMove->GetIsPhysical())
	{
		//是物理攻击
		atk = attackPoint;
		def = enemy->GetPhysicalDEF();
	}
	else
	{
		//是特殊攻击
		atk = specialAttack;
		def = enemy->GetSpecialDEF();
	}

	double preciseDamage = ((2.0 * level + 5.0) / 40.0 * atk / def * attackMove->GetPower() + 2.0) * bonus;//精确到小数的伤害值
	int realDamage = static_cast<int>(preciseDamage);//最后计算出的伤害要进行无条件舍去（即向下取整）
	if (realDamage > 0)
	{
		return realDamage;
	}
	else
	{
		return 1;//保证至少会造成1ＨＰ的伤害
	}
}