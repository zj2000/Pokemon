#include "MainHeader.h"
#include <ctime>
#include <cmath>

const int ATTACK_TIME = 120000;

std::vector<Pokemon*> pokemonLibrary;
std::vector<Move*> movesLibrary;
std::vector<Pokemon*> individualPokemon;//个体宝可梦，包括用户的和系统生成的

int levelEXP[15] = { 0, 274, 2195, 7408, 17561,
					34300, 59270, 94119, 140492, 200037,
					274400, 365226, 474163, 602856, 750000 };//每个等级所需要的经验值


Pokemon::Pokemon(const Pokemon& copy)
{
	//个体相关（个体之间有所差异的值）
	myID = copy.myID;	//个体小精灵编号
	level = copy.level;	//等级
	EXP = copy.EXP;	//经验值

	//属性值
	healthPoint = copy.healthPoint;	//生命值	
	attackPoint = copy.attackPoint;	//攻击值--物理
	defencePoint = copy.defencePoint;	//防御值--物理
	specialAttack = copy.specialAttack;	//特攻值--特殊
	specialDefence = copy.specialDefence;	//特防值--特殊
	attackSpeed = copy.attackSpeed;	//速度
	attackInterval = copy.attackInterval;	//攻击间隔(= 固定时间 ÷ 速度),时间单位是毫秒

	//对战相关
	currentHP = copy.currentHP;		//对战时剩余的生命值
	accuracyLevel = copy.accuracyLevel;	//命中率等级，对战开始和结束时置为0，根据对战招式会有所变化，取值范围在-6～6，超出范围取范围的最值
	evasionLevel = copy.evasionLevel;	//闪避率等级，对战开始和结束时置为0，根据对战招式会有所变化，取值范围在-6～6，超出范围取范围的最值
	isAtCriticalState = copy.isAtCriticalState;//是否处于易中要害状态，是的话可以提高暴击率


	//种族相关（值与所有该种类个体宝可梦一致）
	speciesID = copy.speciesID;	//种族ID
	baseEXP = copy.baseEXP;	//基础经验值，用于计算对战胜利后的经验值提升
	growPoints = copy.growPoints;	//升级时各项指标提升的值
	name = copy.name;//精灵名字	

	mainAttribute = copy.mainAttribute;//主属性：高攻/高防/高血/高速
	PokemonType = copy.PokemonType;//类别：水/火/草/土/电

	for (int i = 0; i < 4; i++)
	{
		learnedMoves[i] = copy.learnedMoves[i];
	}

}

//种族的构造函数
Pokemon::Pokemon(int id, std::string name, std::string type, std::string mainA, int base, int grow, int attr[6], int move[4])
{
	myID = -1;
	level = 1;
	EXP = 0;

	//6项属性
	healthPoint = attr[0];	//生命值	
	attackPoint = attr[1];	//攻击值--物理
	defencePoint = attr[2];	//防御值--物理
	specialAttack = attr[3];	//特攻值--特殊
	specialDefence = attr[4];	//特防值--特殊
	attackSpeed = attr[5];	//速度
	attackInterval = ATTACK_TIME / attackSpeed;	//攻击间隔(= 固定时间 ÷ 速度),时间单位是毫秒

	//初始化战斗相关数值
	Reset();

	speciesID = id;
	baseEXP = base;
	growPoints = grow;
	this->name = name;
	
	//转换字符串为类型
#pragma region convert string to type
	
	if (type == "水")
	{
		PokemonType = Type::water;
	}
	else if(type == "火")
	{
		PokemonType = Type::fire;
	}
	else if (type == "草")
	{
		PokemonType = Type::grass;
	}
	else
	{
		PokemonType = Type::electric;
	}
#pragma endregion

	//转换字符串为主属性
#pragma region convert string to MainAttribute
	if (mainA == "生命")
	{
		mainAttribute = Attribute::highHP;
	}
	else if(mainA == "攻击")
	{
		mainAttribute = Attribute::highPhysicalAttcak;
	}
	else if (mainA == "防御")
	{
		mainAttribute = Attribute::highPhysicalDefence;
	}
	else if (mainA == "特攻")
	{
		mainAttribute = Attribute::highSpecialAttack;
	}
	else if (mainA == "特防")
	{
		mainAttribute = Attribute::highSpecialDefence;
	}
	else
	{
		mainAttribute = Attribute::highSpeed;
	}
#pragma endregion


	//4个招式
	for (int i = 0; i < 4; i++)
	{
		learnedMoves[i] = move[i];
	}

}

//个体的构造函数
Pokemon::Pokemon(int myID, int speciesID, int level, int EXP, int attr[6])
{
	this->myID = myID;
	this->level = level;
	this->EXP = EXP;

	//6项属性
	healthPoint = attr[0];	//生命值	
	attackPoint = attr[1];	//攻击值--物理
	defencePoint = attr[2];	//防御值--物理
	specialAttack = attr[3];	//特攻值--特殊
	specialDefence = attr[4];	//特防值--特殊
	attackSpeed = attr[5];	//速度
	attackInterval = ATTACK_TIME / attackSpeed;	//攻击间隔(= 固定时间 ÷ 速度),时间单位是毫秒

	//初始化种族相关的值
	Pokemon* speciesPokemon = pokemonLibrary[speciesID];
	
	this->speciesID = speciesID;
	baseEXP = speciesPokemon->baseEXP;
	growPoints = speciesPokemon->growPoints;
	name = speciesPokemon->name;

	mainAttribute = speciesPokemon->mainAttribute;//主属性：高攻/高防/高血/高速
	PokemonType = speciesPokemon->PokemonType;//类别：水/火/草/土/电

	//招式
	for (int i = 0; i < 4; i++)
	{
		learnedMoves[i] = speciesPokemon->learnedMoves[i];
	}

	//初始化战斗相关数值
	Reset();

}

double Pokemon::ComputeAccuracyRate() const//计算并返回命中率
{
	double accuracyRate = 1.0;//等级为0时都是100%
	switch (accuracyLevel)
	{
	case 1:
		accuracyRate = 1.5;
		break;
	case 2:
		accuracyRate = 2.0;
		break;
	case 3:
		accuracyRate = 2.5;
		break;
	case 4:
		accuracyRate = 3.0;
		break;
	case 5:
		accuracyRate = 3.5;
		break;
	default:
		accuracyRate = 4.0;
		break;

	}
	return accuracyRate;
}

double Pokemon::ComputeEvasionRate(Pokemon* enemy) const//计算并返回闪避率
{
	double evasionRate = 1.0;//等级为0时都是100%
	switch (enemy->evasionLevel)
	{
	case 1:
		evasionRate = 1.5;
		break;
	case 2:
		evasionRate = 2.0;
		break;
	case 3:
		evasionRate = 2.5;
		break;
	case 4:
		evasionRate = 3.0;
		break;
	case 5:
		evasionRate = 3.5;
		break;
	default:
		evasionRate = 4.0;
		break;
	}
	return evasionRate;
}

double Pokemon::ComputeCriticalRate(Move* attackMove)
{
	int criticalLevel = 0;
	if (attackMove->GetIsCritical())//如果是易于暴击出招，这回合和下回合都会处于易于暴击的状态
	{
		criticalLeftTimes += 2;
		isAtCriticalState = true;

		criticalLevel += 1;
	}
	if (isAtCriticalState)
	{
		criticalLevel += 2;
	}

	if (criticalLevel == 0)
	{
		return (1.0 / 16.0);
	}
	else if (criticalLevel == 1)
	{
		return (1.0 / 8.0);
	}
	else if(criticalLevel == 2)
	{
		return (1.0 / 4.0);
	}
	else //criticalLevel == 3
	{
		return (1.0 / 3.0);
	}

}

bool Pokemon::whetherHit(Pokemon* enemy, int moveAccuracy) const//判断并返回对战时是否命中对方
{
	/*	令A = B × C ÷ D
		B由招式的命中决定，C由攻击方命中率决定，D由防御方回避率决定
		产生一个1～127之间的随机数，该随机数小于A时视为命中，否则为失误。*/

	double myAccuracyRate = ComputeAccuracyRate();		//公式中的C
	double enemyEvasionRate = ComputeEvasionRate(enemy);//公式中的D
	int hitNum = static_cast<int>(moveAccuracy * myAccuracyRate / enemyEvasionRate);


	int random = rand() % 127 + 1;//生成一个1-255的随机数
	if (random < hitNum)
		return true;//命中
	else
		return false;//失误
}

int Pokemon::UpdateExp(Pokemon* enemyPokemon)//对战胜利后，更新经验值，并自动升级
{
	//根据正版游戏第五世代的经验值计算方法计算

	if (level == 15)
		return 0;
	else
	{
		double tempEXP1 = (enemyPokemon->baseEXP * enemyPokemon->level) / 2; //公式一：被击败的宝可梦的基础经验值×被击败的宝可梦的等级/2
		double tempEXP2 = (2 * enemyPokemon->level + 10) / (enemyPokemon->level + level + 10);//公式2

		int gainEXP = static_cast<int>(tempEXP1 * std::pow(tempEXP2, 2.5) + 1);
		EXP += gainEXP;
		if (EXP > levelEXP[14])//升级后到达满级
		{
			EXP = levelEXP[14];
		}
		else;

		bool isLevelUp = false;//是否升级
		while (EXP >= levelEXP[level])//可能一次性升多级
		{
			level++;
			isLevelUp = true;
		}
		if (isLevelUp == true)
		{
			UpdateAttribute();
		}
		else
			;

		return gainEXP;
	}
	
}

//初始获得宝可梦或等级提升时更新各项属性的实际个体值
void Pokemon::UpdateAttribute()
{	
	/*从图鉴中找到对应标号的宝可梦，从图鉴中读取该种类宝可梦初始种族值，根据初始种族值换算出对应等级对应的种族值
	  再根据宝可梦的类型（高攻/高防/高血/高速）给主属性增加更多的值
	  最后计算实际个体值，实际值 = 种族值×因数 + 加数（因数和加数根据正版游戏的计算公式演变而来）*/
	
	//从图鉴中读取该种类宝可梦初始种族值（1级）
	double speciesHP = pokemonLibrary[speciesID]->healthPoint;		//生命值
	double speciesATK = pokemonLibrary[speciesID]->attackPoint;		//攻击
	double speciesDEF = pokemonLibrary[speciesID]->defencePoint;		//防御
	double speciesSPATK = pokemonLibrary[speciesID]->specialAttack;	//特攻
	double speciesSPDEF = pokemonLibrary[speciesID]->specialDefence;	//特防
	double speciesSpeed = pokemonLibrary[speciesID]->attackSpeed;		//速度
	double* speciesPtr[6] ={&speciesHP, &speciesATK, &speciesDEF, &speciesSPATK, &speciesSPDEF, &speciesSpeed};//把上述种族值的地址存在一个数组里

	int deltaLevel = level - 1;//提升的等级
	if (deltaLevel != 0)//有等级的提升才进行下列计算（过滤初始获得个体宝可梦的情况）
	{
		//根据宝可梦的类型（高攻/高防/高血/高速）标记 指向主属性种族值的指针
		double* mainAttributePtr = nullptr;//该指针指向宝可梦主属性种族值，主属性种族值升级的时候增加的值会更多一些
		switch (mainAttribute)
		{
		case Attribute::highHP://肉盾型：高生命值
			mainAttributePtr = &speciesHP;
			break;

		case Attribute::highPhysicalAttcak://力量型：高攻击--之物理攻击
			mainAttributePtr = &speciesATK;
			break;

		case Attribute::highSpecialAttack://力量型：高攻击--之特殊攻击
			mainAttributePtr = &speciesSPATK;
			break;

		case Attribute::highPhysicalDefence://防御型：高防御--之物理防御
			mainAttributePtr = &speciesDEF;
			break;

		case Attribute::highSpecialDefence://防御型：高防御--之特殊防御
			mainAttributePtr = &speciesSPDEF;
			break;

		case Attribute::highSpeed://敏捷型：低攻击间隔
			mainAttributePtr = &speciesSpeed;
			break;

		default:
			break;
		}

		//给每种属性增加相对应的值，该值通过乘上一个随机因数（0.8-1.0）来增加随机性
		int levelGrow = deltaLevel * growPoints; //从1级到当前等级总共所增长的属性值
		for (int i = 0; i < 6; i++)
		{
			double finalGrow = 0.0;
			if (mainAttributePtr == speciesPtr[i])//该属性与主属性地址相同说明该属性是主属性
			{
				finalGrow = 1.5 * levelGrow * GetRandomFactor(80);//主属性的增长值更多一些
			}
			else
			{
				finalGrow = GetRandomFactor(80) * levelGrow;
			}

			if (i == 0)//生命值每级提升量是别的指数的两倍（不管是不是高血量类型）
			{
				finalGrow *= 2;
			}
			else;

			*speciesPtr[i] += finalGrow;
		}
	}
	else
		;

	//根据公式从种族值中求出实际个体值
	double factor = 3.0 * (5.0 + level) / 20.0;//因数

	healthPoint = static_cast<int>(speciesHP * factor + 10 + 7.0 * level);
	attackPoint = static_cast<int>(speciesATK * factor + 5);
	defencePoint = static_cast<int>(speciesDEF * factor + 5);
	specialAttack = static_cast<int>(speciesSPATK * factor + 5);
	specialDefence = static_cast<int>(speciesSPDEF * factor + 5);
	attackSpeed = static_cast<int>(speciesSpeed * factor + 5);
	attackInterval = ATTACK_TIME / attackSpeed;
}

double Pokemon::GetRandomFactor(int min) const
{
	double randomFactor =(1.0 * (rand() % (100 - min)) + min + 1) / 100.0;
	return randomFactor;
}

int Pokemon::GetLevel() const
{
	return level;
}

Type Pokemon::GetType()const
{
	return PokemonType;
}

int Pokemon::GetIndividualID()const
{
	return myID;
}

int Pokemon::GetSpeiesID() const
{
	return speciesID;
}

int Pokemon::GetCurrentHP() const
{
	return currentHP;
}

std::string Pokemon::GetTypeString(Type type) const
{
	std::string typeString;
	switch (type)
	{
	case Type::water:
		typeString = "水系";
		break;

	case Type::fire:
		typeString = "火系";
		break;

	case Type::grass:
		typeString = "草系";
		break;

	case Type::electric:
		typeString = "电系";
		break;
		
	default:
		typeString = "普通";
		break;
	}
	return typeString;
}

std::string Pokemon::GetAttributeString() const
{
	std::string attr;
	switch (mainAttribute)
	{
	case Attribute::highHP:
		attr = "高生命型";
		break;

	case Attribute::highPhysicalAttcak:
		attr = "高攻击型";
		break;

	case Attribute::highPhysicalDefence:
		attr = "高防御型";
		break;

	case Attribute::highSpecialAttack:
		attr = "高特攻型";
		break;

	case Attribute::highSpecialDefence:
		attr = "高特防型";
		break;

	default:
		attr = "高速度型";
		break;
	}
	return attr;

}

//初始化战斗相关数值
void Pokemon::Reset()
{
	currentHP = healthPoint;//对战时剩余的生命值
	accuracyLevel = 0;		//命中率等级，对战开始和结束时置为0，根据对战招式会有所变化，取值范围在-6～6，超出范围取范围的最值
	evasionLevel = 0;		//闪避率等级，对战开始和结束时置为0，根据对战招式会有所变化，取值范围在-6～6，超出范围取范围的最值
	isAtCriticalState = false;	//是否处于易中要害状态，是的话可以提高暴击率
	criticalLeftTimes = 0;

	moveName = "";
	damage = 0;

	lastAttackTime = currentTime;
}

std::string Pokemon::GetName()const
{
	return name;
}

int Pokemon::GetSpeed() const
{
	return attackSpeed;
}

//随机出招,返回值为招式在招式库下标,即招式ID
int Pokemon::AutoMove()const
{
	int size = level / 4 + 1;//当前学会的的招式数
	
	//随机出招
	int seq = rand() % size;

	return learnedMoves[seq];
}

void Pokemon::Damage(int damage)
{
	currentHP -= damage;
}

void Pokemon::DrawPkmInfo() const
{
	std::string text;
	CString textCString;

	//宝可梦ID
	text = std::to_string(myID);
	textCString = text.c_str();
	outtextxy(239, 48, textCString);

	//宝可梦名字
	textCString = name.c_str();
	outtextxy(222, 87, textCString);

	//等级
	text = std::to_string(level);
	textCString = text.c_str();
	outtextxy(233, 126, textCString);

	//经验值
	text = std::to_string(EXP);
	textCString = text.c_str();
	outtextxy(215, 163, textCString);

	//距下一级的经验
	if (level == 15)
	{
		text = "已到达满级";
	}
	else
	{
		text = std::to_string(levelEXP[level] - EXP);
	}
	textCString = text.c_str();
	outtextxy(280, 199, textCString);

	//种类及主属性
	text = GetTypeString(PokemonType) + GetAttributeString();
	textCString = text.c_str();
	outtextxy(262, 239, textCString);

	//生命
	text = std::to_string(healthPoint);
	textCString = text.c_str();
	outtextxy(210, 280, textCString);

	//速度
	text = std::to_string(attackSpeed);
	textCString = text.c_str();
	outtextxy(353, 280, textCString);

	//攻击
	text = std::to_string(attackPoint);
	textCString = text.c_str();
	outtextxy(210, 318, textCString);

	//防御
	text = std::to_string(defencePoint);
	textCString = text.c_str();
	outtextxy(353, 318, textCString);

	//特攻
	text = std::to_string(specialAttack);
	textCString = text.c_str();
	outtextxy(210, 356, textCString);

	//特防
	text = std::to_string(specialDefence);
	textCString = text.c_str();
	outtextxy(353, 356, textCString);

	//招式表
	int curHeight = 128;
	for (int i = 0; i < 4; i++)
	{
		Move* curMove = movesLibrary[learnedMoves[i]];
		//招式名
		text = curMove->GetName() + " - " + GetTypeString(curMove->GetType());
		textCString = text.c_str();
		outtextxy(462, curHeight, textCString);
		//威力
		text = std::to_string(curMove->GetPower());
		textCString = text.c_str();
		outtextxy(497, curHeight + 24, textCString);
		
		curHeight += 64;
	}
}

void Pokemon::DrawPkmList(int curPage, int total, int curCheckSeq) const
{
	int deltaHeight = 46;
	int curHeight = 65;
	int startID = curPage * 6;
	for (int i = startID; i < (startID + 6) && i < total; i++)
	{
		if (i != curCheckSeq)//不是当前正在查看的宝可梦
		{
			putimage(45, curHeight, &imgList[0]);
		}
		else//当前正在查看的宝可梦
		{
			putimage(45, curHeight, &imgList[1]);
		}

		std::string text = std::to_string(i);
		CString textCString = text.c_str();
		outtextxy(63, curHeight + 6, textCString);

		curHeight += deltaHeight;
	}

}

void Pokemon::DrawLose(int seq) const
{
	putimage((131 + seq * 146), 141, &imgFrontCover[speciesID], SRCPAINT);
	putimage((131 + seq * 146), 141, &imgFront[speciesID], SRCAND);

	std::string text;
	CString textCString;

	settextcolor(RGB(210, 225, 230));
	textCString = name.c_str();
	outtextxy((150 + seq * 146), 244, textCString);

	settextcolor(RGB(0, 73, 93));
	text = std::to_string(level);
	textCString = text.c_str();
	outtextxy((181 + seq * 146), 276, textCString);
}

void Pokemon::DrawBattle(bool isAtFront) const
{
	std::string text;
	CString textCString;

	font.lfHeight = 26; // 设置字体高度
	settextstyle(&font);// 设置字体样式

	int xLoc;
	if (isAtFront)
	{
		xLoc = 355;
	}
	else
	{
		xLoc = 52;
	}

	//名字+等级
	text = name + "(Lv:  " + std::to_string(level) + ")";
	textCString = text.c_str();
	outtextxy(xLoc, 410, textCString);

	//剩余血量
	text = "剩余血量：" + std::to_string(currentHP);
	textCString = text.c_str();
	outtextxy(xLoc, 442, textCString);

	if (currentTime - lastAttackTime <= 1000 && moveName != "")//出招显示时间为1秒
	{
		text = "发动攻击：" + moveName;
		textCString = text.c_str();
		outtextxy(xLoc, 474, textCString);

		text = "造成伤害：" + std::to_string(damage);
		if (!isHit)
		{
			text += "(对方闪避)";
		}
		textCString = text.c_str();
		outtextxy(xLoc, 506, textCString);

		if (isCritical)
		{
			text = "造成暴击！";
			textCString = text.c_str();
			outtextxy(xLoc, 538, textCString);
		}
		else;
	}
	else
		;
}

//要发送给服务器的宝可梦信息
std::string Pokemon::GetPokemonInfo() const
{

	/*
		文件记录内容及数据类型如下：
		个体ID	|  等级	| 经验值	| 生命值	| 攻击值	| 防御值	| 特攻值	| 特防值	| 速度值	|
		int		|  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|
	*/
	std::string info = std::to_string(myID);
	info += ' ' + std::to_string(level);
	info += ' ' + std::to_string(EXP);
	info += ' ' + std::to_string(healthPoint);
	info += ' ' + std::to_string(attackPoint);
	info += ' ' + std::to_string(defencePoint);
	info += ' ' + std::to_string(specialAttack);
	info += ' ' + std::to_string(specialDefence);
	info += ' ' + std::to_string(attackSpeed);

	return info;
}


int Pokemon::ComputeFinalDamage(double bonus, Move* attackMove, Pokemon* enemy) const
{
	//计算最终伤害值,伤害 = （（等级 + 5） / 20 × 攻击 / 防御 × 威力 + 2）× 加成

	//根据招式攻击的类型判断参与计算的攻击是物攻还是特攻，防御是物防还是特防
	double atk = 1;//要参与伤害计算的攻击值
	double def = 1;//要参与伤害计算的防御值
	if (attackMove->GetIsPhysical())
	{
		//是物理攻击
		atk = static_cast<double>(attackPoint);
		def = static_cast<double>(enemy->defencePoint);
	}
	else
	{
		//是特殊攻击
		atk = static_cast<double>(specialAttack);
		def = static_cast<double>(enemy->specialDefence);
	}

	double preciseDamage = ((5.0 + level) / 20.0 * atk * attackMove->GetPower() / def + 2.0) * bonus;//精确到小数的伤害值
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

int Pokemon::Attack(Pokemon* enemy)
{
	return 0;
}