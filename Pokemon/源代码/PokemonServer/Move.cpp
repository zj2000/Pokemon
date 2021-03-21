#include "MainHeader.h"

Move::Move(int id, int accuracy, int power, int PP, bool isCri, bool isPhy, std::string type, std::string name, std::string info)
{
	//movesFile >> id >> accuracy >> power >> PP >> isCriticalHit >> isPhysical >> type >> name >> info;

	this->id = id;
	this->accuracy = accuracy;
	this->power = power;
	this->PP = PP;
	leftPP = PP;
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
	else if (type == "土")
	{
		moveType = Type::ground;
	}
	else
	{
		moveType = Type::electric;
	}
#pragma endregion

	moveName = name;
	this->info = info;
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

std::string Move::GetInfo() const
{
	return info;
}
