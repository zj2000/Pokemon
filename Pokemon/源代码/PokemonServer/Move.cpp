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

	//ת���ַ���Ϊ����
#pragma region convert string to type
	if (type == "ˮ")
	{
		moveType = Type::water;
	}
	else if (type == "��")
	{
		moveType = Type::fire;
	}
	else if (type == "��")
	{
		moveType = Type::grass;
	}
	else if (type == "��")
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
