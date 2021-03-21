#include "MainHeader.h"

WaterPokemon::WaterPokemon(const Pokemon& pkm) : Pokemon(pkm)
{
	advantageType = Type::fire;
	disadvantageType = Type::electric;
}

FirePokemon::FirePokemon(const Pokemon& pkm) : Pokemon(pkm)
{
	advantageType = Type::grass;
	disadvantageType = Type::water;
}

GrassPokemon::GrassPokemon(const Pokemon& pkm) : Pokemon(pkm)
{
	advantageType = Type::electric;
	disadvantageType = Type::fire;
}

ElectricPokemon::ElectricPokemon(const Pokemon& pkm) : Pokemon(pkm)
{
	advantageType = Type::water;
	disadvantageType = Type::grass;
}

extern Weather battleWeather;
/*
	伤害 = （（等级+5）/20 × 攻击/防御 × 威力 +2）× 加成

	等级 是攻击方宝可梦的等级。
	攻击与防御 分别是攻击方的攻击或者特攻（取决于是物理/特殊招式），和防守方的防御或特防（取决于招式是物理/特殊招式）
	加成 使用如下的公式计算：加成=属性一致加成×属性相克造成的倍率×击中要害的倍率×其他加成×随机数（随机数∈[0.85，1]）
	当有属性一致加成效果时该值为1.5，否则为1。
	属性相克造成的倍率可以为0.5、1、2。
	当击中要害（即暴击）时，该值为3⁄2,如果没有击中要害，该值为1。
	其他加成 是指天气加成。
	最后计算出的伤害要进行无条件舍去（即向下取整），但至少会造成1ＨＰ的伤害。
*/
int WaterPokemon::Attack(Pokemon* enemy)
{
	//如果当前时间-上次进攻时间大于进攻间隔则可以发动进攻,按照公式计算伤害值，否则返回0
	if (currentTime - lastAttackTime < attackInterval)
	{
		return 0;
	}
	else
	{
		Move* attackMove = movesLibrary[AutoMove()];
		moveName = attackMove->GetName();

		//更新进攻时间
		lastAttackTime = currentTime;

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
		if (isAtCriticalState)
		{
			criticalLeftTimes--;//消耗一回合的爆击状态
			if (criticalLeftTimes <= 0)//更新暴击状态
			{
				isAtCriticalState = false;
				criticalLeftTimes = 0;
			}
			else;
		}
		else;

		//随机生成一个0-100的整数，如果这个数小于等于暴击率，暴击倍率为1.5
		int randomNum = rand() % 100;
		if (randomNum <= criticalRate)
		{
			bonus *= 1.5;
			isCritical = true;
		}
		else
		{
			isCritical = false;
		}

		//天气加成
		if (battleWeather == Weather::rain)//雨天对水系有利
		{
			bonus *= 1.5;
			//呆的时间越久命中率和闪避率越高
			accuracyLevel++;
			evasionLevel++;
		}
		else;
	#pragma endregion

		//计算伤害值,伤害 = （（等级 + 5） / 20 × 攻击 / 防御 × 威力 + 2）× 加成
		damage = ComputeFinalDamage(bonus, attackMove, enemy);
		//判断是否击中对方
		if (whetherHit(enemy, attackMove->GetAccuracy()))
		{
			isHit = true;
		}
		else
		{
			damage = 0;
			isHit = false;
		}

		return damage;

	}//end of else(大于进攻间隔)
	
}

int FirePokemon::Attack(Pokemon* enemy)
{
	//如果当前时间-上次进攻时间大于进攻间隔则可以发动进攻,按照公式计算伤害值，否则返回0
	if (currentTime - lastAttackTime < attackInterval)
	{
		return 0;
	}
	else
	{
		Move* attackMove = movesLibrary[AutoMove()];
		moveName = attackMove->GetName();

		//更新进攻时间
		lastAttackTime = currentTime;

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
		if (isAtCriticalState)
		{
			criticalLeftTimes--;//消耗一回合的爆击状态
			if (criticalLeftTimes <= 0)//更新暴击状态
			{
				isAtCriticalState = false;
				criticalLeftTimes = 0;
			}
			else;
		}
		else;

		//随机生成一个0-100的整数，如果这个数小于等于暴击率，暴击倍率为1.5
		int randomNum = rand() % 100;
		if (randomNum <= criticalRate)
		{
			bonus *= 1.5;
			isCritical = true;
		}
		else
		{
			isCritical = false;
		}

		//天气加成
		if (battleWeather == Weather::sun)//晴天对火系有利
		{
			bonus *= 1.5;
			//呆的时间越久命中率和闪避率越高
			accuracyLevel++;
			evasionLevel++;
		}
		else;
#pragma endregion

		//计算伤害值,伤害 = （（等级 + 5） / 20 × 攻击 / 防御 × 威力 + 2）× 加成
		damage = ComputeFinalDamage(bonus, attackMove, enemy);
		//判断是否击中对方
		if (whetherHit(enemy, attackMove->GetAccuracy()))
		{
			isHit = true;
		}
		else
		{
			damage = 0;
			isHit = false;
		}

		return damage;

	}//end of else(大于进攻间隔)

}

int GrassPokemon::Attack(Pokemon* enemy)
{
	//如果当前时间-上次进攻时间大于进攻间隔则可以发动进攻,按照公式计算伤害值，否则返回0
	if (currentTime - lastAttackTime < attackInterval)
	{
		return 0;
	}
	else
	{
		Move* attackMove = movesLibrary[AutoMove()];
		moveName = attackMove->GetName();

		//更新进攻时间
		lastAttackTime = currentTime;

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
		if (isAtCriticalState)
		{
			criticalLeftTimes--;//消耗一回合的爆击状态
			if (criticalLeftTimes <= 0)//更新暴击状态
			{
				isAtCriticalState = false;
				criticalLeftTimes = 0;
			}
			else;
		}
		else;

		//随机生成一个0-100的整数，如果这个数小于等于暴击率，暴击倍率为1.5
		int randomNum = rand() % 100;
		if (randomNum <= criticalRate)
		{
			bonus *= 1.5;
			isCritical = true;
		}
		else
		{
			isCritical = false;
		}

		//天气加成
		if (battleWeather == Weather::sun)//晴天对草系有利
		{
			bonus *= 1.5;
			//呆的时间越久命中率和闪避率越高
			accuracyLevel++;
			evasionLevel++;
		}
		else;
#pragma endregion

		//计算伤害值,伤害 = （（等级 + 5） / 20 × 攻击 / 防御 × 威力 + 2）× 加成
		damage = ComputeFinalDamage(bonus, attackMove, enemy);

		//判断是否击中对方
		if (whetherHit(enemy, attackMove->GetAccuracy()))
		{
			isHit = true;
		}
		else
		{
			damage = 0;
			isHit = false;
		}

		return damage;

	}//end of else(大于进攻间隔)

}

int ElectricPokemon::Attack(Pokemon* enemy)
{
	//如果当前时间-上次进攻时间大于进攻间隔则可以发动进攻,按照公式计算伤害值，否则返回0
	if (currentTime - lastAttackTime < attackInterval)
	{
		return 0;
	}
	else
	{
		Move* attackMove = movesLibrary[AutoMove()];
		moveName = attackMove->GetName();

		//更新进攻时间
		lastAttackTime = currentTime;

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
		if (isAtCriticalState)
		{
			criticalLeftTimes--;//消耗一回合的爆击状态
			if (criticalLeftTimes <= 0)//更新暴击状态
			{
				isAtCriticalState = false;
				criticalLeftTimes = 0;
			}
			else;
		}
		else;

		//随机生成一个0-100的整数，如果这个数小于等于暴击率，暴击倍率为1.5
		int randomNum = rand() % 100;
		if (randomNum <= criticalRate)
		{
			bonus *= 1.5;
			isCritical = true;
		}
		else
		{
			isCritical = false;
		}

		//天气加成
		if (battleWeather == Weather::thunder)//打雷对电系有利
		{
			bonus *= 1.5;
			//呆的时间越久命中率和闪避率越高
			accuracyLevel++;
			evasionLevel++;
		}
		else;
#pragma endregion

		//计算伤害值,伤害 = （（等级 + 5） / 20 × 攻击 / 防御 × 威力 + 2）× 加成
		damage = ComputeFinalDamage(bonus, attackMove, enemy);
		
		//判断是否击中对方
		if (whetherHit(enemy, attackMove->GetAccuracy()))
		{
			isHit = true;
		}
		else
		{
			damage = 0;
			isHit = false;
		}

		return damage;

	}//end of else(大于进攻间隔)

}