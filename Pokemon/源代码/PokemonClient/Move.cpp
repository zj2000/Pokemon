#include "MainHeader.h"

Move::Move(int id, int accuracy, int power, bool isCri, bool isPhy, std::string type, std::string name)
{
	//movesFile >> id >> accuracy >> power >> isCriticalHit >> isPhysical >> type >> name;

	this->id = id;
	this->accuracy = accuracy;
	this->power = power;
	isCriticalHit = isCri;
	isPhysical = isPhy;

	//转换字符串为类型
#pragma region convert string to type
	if (type == "水")
	{
		moveType = Type::water;
	}
	else if (type == "火")
	{
		moveType = Type::fire;
	}
	else if (type == "草")
	{
		moveType = Type::grass;
	}
	else if(type == "电")
	{
		moveType = Type::electric;
	}
	else
	{
		moveType = Type::normal;
	}
#pragma endregion

	moveName = name;
}

int Move::GetAccuracy() const
{
	return accuracy;
}

int Move::GetPower() const
{
	return power;
}

Type Move::GetType() const
{
	return moveType;
}

bool Move::GetIsPhysical() const
{
	return isPhysical;
}

//返回招式名
std::string Move::GetName() const
{
	return moveName;
}

bool Move::GetIsCritical() const
{
	return isCriticalHit;
}