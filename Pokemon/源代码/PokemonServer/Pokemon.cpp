#include "MainHeader.h"
#include <ctime>
#include <cmath>

const int ATTACK_TIME = 120000;

std::string imgOriginalFrontLoc = "./res/images/original/front/";
std::string imgCoverFrontLoc = "./res/images/cover/front/";
std::string imgOriginalBackLoc = "./res/images/original/back/";
std::string imgCoverBackLoc = "./res/images/cover/back/";

extern std::vector<Pokemon*> pokemonLibrary;

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

	speciesID = id;
	baseEXP = base;
	growPoints = grow;
	this->name = name;
	
	PokemonType = type;
	mainAttribute = mainA;

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

}

int Pokemon::GetIndividualID()const
{
	return myID;
}


void Pokemon::InitIndividual(int individualID, int level)//初始分配的宝可梦初始化个体ID
{
	myID = individualID;
	this->level = level;
	EXP = levelEXP[level - 1];
}

void Pokemon::AppendIndividualFile() const
{
	/*
		文件记录内容及数据类型如下：
		 个体ID	| 种族ID	|  等级	| 经验值	| 生命值	| 攻击值	| 防御值	| 特攻值	| 特防值	| 速度值	|
		  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|
	*/
	bool isEmpty = CheckEmpty(POKEMON_INDIVIDUAL_LOC);

	std::ofstream individualFile;
	individualFile.open(POKEMON_INDIVIDUAL_LOC, std::ios::app | std::ios::out);//文件不存在就创建一个，存在也不会删除原先内容
	if (individualFile.is_open())
	{
		if (!isEmpty)//因为一行记录一个宝可梦的数据，所以如果文件不为空，则先换行
		{
			individualFile << '\n';
		}
		else;

		//各项数据之间用空格间隔
		individualFile << myID << ' ' << speciesID << ' ' << level << ' ' << EXP << ' '
			<< healthPoint << ' ' << attackPoint << ' ' << defencePoint << ' '
			<< specialAttack << ' ' << specialDefence << ' ' << attackSpeed;
	}
	else
		;
}

//判断某个要写入的文件是否为空
bool Pokemon::CheckEmpty(std::string loc)const
{
	bool isEmpty = false;
	std::ifstream in;
	in.open(loc, std::ios::in);
	if (in.is_open())
	{
		if (in.peek() == EOF)
		{
			isEmpty = true;
		}
		else;
	}
	else
	{
		isEmpty = true;
	}
	in.close();

	return isEmpty;
}

//根据种族生成宝可梦时更新各项属性的实际个体值
void Pokemon::UpdateAttribute(int _level)
{
	/*根据初始种族值换算出对应等级对应的种族值
	  最后计算实际个体值，实际值 = 种族值×因数 + 加数（因数和加数根据正版游戏的计算公式演变而来）*/

	int* speciesPtr[6] = { &healthPoint, &attackPoint, &defencePoint, &specialAttack, &specialDefence, &attackSpeed };//把上述种族值的地址存在一个数组里

	int deltaLevel = level - 1;//提升的等级
	if (deltaLevel != 0)//有等级的提升才进行下列计算（过滤初始获得个体宝可梦的情况）
	{
		//根据宝可梦的类型（高攻/高防/高血/高速）标记 指向主属性种族值的指针
		int* mainAttributePtr = nullptr;//该指针指向宝可梦主属性种族值，主属性种族值升级的时候增加的值会更多一些
		if (mainAttribute == "生命")//肉盾型：高生命值
		{
			mainAttributePtr = &healthPoint;
		}
		else if (mainAttribute == "攻击")//力量型：高攻击--之物理攻击
		{
			mainAttributePtr = &attackPoint;
		}
		else if (mainAttribute == "防御")//防御型：高防御--之物理防御
		{
			mainAttributePtr = &defencePoint;
		}
		else if (mainAttribute == "特攻")//力量型：高攻击--之特殊攻击
		{
			mainAttributePtr = &specialAttack;
		}
		else if (mainAttribute == "特防")//防御型：高防御--之特殊防御
		{
			mainAttributePtr = &specialDefence;
		}
		else//敏捷型：低攻击间隔
		{
			mainAttributePtr = &attackSpeed;
		}

		//给每种属性增加相对应的值，该值通过乘上一个随机因数（0.8-1.0）来增加随机性
		int levelGrow = deltaLevel * growPoints; //从1级到当前等级总共所增长的属性值
		for (int i = 0; i < 6; i++)
		{
			double finalGrow = 0.0;
			if (mainAttributePtr == speciesPtr[i])//该属性与主属性地址相同说明该属性是主属性
			{
				finalGrow = levelGrow * 1.5 * GetRandomFactor(80);//主属性的增长值更多一些
			}
			else
			{
				finalGrow = levelGrow * GetRandomFactor(80);
			}

			if (i == 0)//生命值每级提升量是别的指数的两倍（不管是不是高血量类型）
			{
				finalGrow *= 2;
			}
			else;

			*speciesPtr[i] += static_cast<int>(finalGrow);
		}
	}
	else
		;

	//根据公式从种族值中求出实际个体值
	double factor = 3.0 * (5.0 + level) / 20.0;//因数

	healthPoint = static_cast<int>(factor * healthPoint + 10 + 7.0 * level);
	attackPoint = static_cast<int>(factor* attackPoint + 5);
	defencePoint = static_cast<int>(factor* defencePoint + 5);
	specialAttack = static_cast<int>(factor* specialAttack + 5);
	specialDefence = static_cast<int>(factor* specialDefence + 5);
	attackSpeed = static_cast<int>(factor * attackSpeed + 5);
	attackInterval = ATTACK_TIME / attackSpeed;

}

double Pokemon::GetRandomFactor(int min) const
{
	srand(clock());
	double randomFactor = (rand() % (100 - min) + min + 1) / 100;
	return randomFactor;
}


std::string Pokemon::GetPokemonInfo() const
{

	/*
		文件记录内容及数据类型如下：
		个体ID	| 种族ID	|  等级	| 经验值	| 生命值	| 攻击值	| 防御值	| 特攻值	| 特防值	| 速度值	|(后6项为attr[6])
		int		| int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|  int	|
	*/
	std::string info = std::to_string(myID);
	info += ' ' + std::to_string(speciesID);
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

void Pokemon::SetAttr(int attr[8])
{
	level = attr[0];
	EXP = attr[1];
	healthPoint = attr[2];
	attackPoint = attr[3];
	defencePoint = attr[4];
	specialAttack = attr[5];
	specialDefence = attr[6];
	attackSpeed = attr[7];
}

int Pokemon::GetLevel() const
{
	return level;
}