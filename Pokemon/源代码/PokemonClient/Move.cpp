#include "MainHeader.h"

Move::Move(int id, int accuracy, int power, bool isCri, bool isPhy, std::string type, std::string name)
{
	//movesFile >> id >> accuracy >> power >> isCriticalHit >> isPhysical >> type >> name;

	this->id = id;
	this->accuracy = accuracy;
	this->power = power;
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
	else if(type == "��")
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

//������ʽ��
std::string Move::GetName() const
{
	return moveName;
}

bool Move::GetIsCritical() const
{
	return isCriticalHit;
}