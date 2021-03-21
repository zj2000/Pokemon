#include "MainHeader.h"

User::User()
{
	userID = -1;

	isOnline = false;//是否在线

	winTimes = 0;
	loseTimes = 0;

	pokemonSum = 0;
	fullLevelSum = 0;
}

User& User::operator=(const User& copy)
{
	if (copy.userID != userID)//两者指向用户不相同时赋值
	{
		this->userID = copy.userID;
		this->winTimes = copy.winTimes;
		this->loseTimes = copy.loseTimes;

		this->pokemonSum = copy.pokemonSum;
		this->fullLevelSum = copy.fullLevelSum;

		this->name = copy.name; //用户名，必须唯一
		this->isOnline = copy.isOnline;

		this->ClearBags();

		this->totalPokemonBags.assign(copy.totalPokemonBags.begin(), copy.totalPokemonBags.end());
		this->waterPokemonBags.assign(copy.waterPokemonBags.begin(), copy.waterPokemonBags.end());
		this->firePokemonBags.assign(copy.firePokemonBags.begin(), copy.firePokemonBags.end());
		this->grassPokemonBags.assign(copy.grassPokemonBags.begin(), copy.grassPokemonBags.end());
		this->electricPokemonBags.assign(copy.electricPokemonBags.begin(), copy.electricPokemonBags.end());
	}
	else
		;

	return *this;
}


void User::UpdateFullLevel()
{
	int size = totalPokemonBags.size();
	int fullSum = 0;

	for (int i = 0; i < size; i++)
	{
		if (totalPokemonBags[i]->GetLevel() == 15)
		{
			fullSum++;
		}
		else
			;

	}
	fullLevelSum = fullSum;
}



int User::HasPokemon(Type type) const
{
	int size = 0;
	switch (type)
	{
		case Type::water:
			{
				size = waterPokemonBags.size();
			}
			break;
		
		case Type::fire:
			{
				size = firePokemonBags.size();
			}
			break;
		
		case Type::grass:
			{
				size = grassPokemonBags.size();
			}
			break;
		
		default://electric
			{
				size = electricPokemonBags.size();
			}
			break;
	}

	return size;
}

Pokemon* User::GetCurPkm(Type type, int seq) const
{
	if (type == Type::water)
	{
		return (Pokemon*)waterPokemonBags[seq];
	}
	else if (type == Type::fire)
	{
		return (Pokemon*)firePokemonBags[seq];
	}
	else if (type == Type::grass)
	{
		return (Pokemon*)grassPokemonBags[seq];
	}
	else//type == Type::electric
	{
		return (Pokemon*)electricPokemonBags[seq];
	}
}

int User::GetPkmSum() const
{
	return pokemonSum;
}

int User::GetID()const
{
	return userID;
}

//返回被强制转换成父类指针的子类指针
Pokemon* User::GetTotalBagPkm(int seq) const
{
	int pkmID = totalPokemonBags[seq]->GetIndividualID();
	bool isFound = false;
	Pokemon* retPkm = nullptr;

	Type pkmType = totalPokemonBags[seq]->GetType();
	if (pkmType == Type::water)
	{
		for (UINT i = 0; i < waterPokemonBags.size() && !isFound; i++)
		{
			if (waterPokemonBags[i]->GetIndividualID() == pkmID)
			{
				retPkm = (Pokemon*)waterPokemonBags[i];
				isFound = true;
			}
			else
				;
		}
	}
	else if (pkmType == Type::fire)
	{
		for (UINT i = 0; i < firePokemonBags.size() && !isFound; i++)
		{
			if (firePokemonBags[i]->GetIndividualID() == pkmID)
			{
				retPkm = (Pokemon*)firePokemonBags[i];
				isFound = true;
			}
			else
				;
		}
	}
	else if (pkmType == Type::grass)
	{
		for (UINT i = 0; i < grassPokemonBags.size() && !isFound; i++)
		{
			if (grassPokemonBags[i]->GetIndividualID() == pkmID)
			{
				retPkm = (Pokemon*)grassPokemonBags[i];
				isFound = true;
			}
			else
				;
		}
	}
	else//pkmType == Type::electric
	{
		for (UINT i = 0; i < electricPokemonBags.size() && !isFound; i++)
		{
			if (electricPokemonBags[i]->GetIndividualID() == pkmID)
			{
				retPkm = (Pokemon*)electricPokemonBags[i];
				isFound = true;
			}
			else
				;
		}
	}

	return retPkm;
}

double User::GetWinRate() const
{
	int totalTimes = winTimes + loseTimes;
	if (totalTimes == 0)
	{
		return 0.0;
	}
	else
	{
		return (static_cast<double>(winTimes) / static_cast<double>(totalTimes));
	}
}

void User::DrawUserInfo(int localID, int state)const
{
	std::string text;
	CString textCString;

	//打印用户在线情况
	if (isOnline)
	{
		text = "在线";
		if ((state == (int)State::userUI && localID == userID) 
			|| state == (int)State::mainUI)
		{
			text += "(本人)";
		}
		else;
	}
	else
	{
		text = "不在线";
	}
	textCString = text.c_str();
	outtextxy(260, 44, textCString);

	//打印用户名
	text = name;
	textCString = text.c_str();
	outtextxy(230, 86, textCString);

	//打印拥有宝可梦数
	text = std::to_string(pokemonSum);
	textCString = text.c_str();
	outtextxy(298, 127, textCString);

	//打印拥有高级（满级）宝可梦数
	text = std::to_string(fullLevelSum);
	textCString = text.c_str();
	outtextxy(316, 169, textCString);

	//打印胜率
	//text = std::to_string(GetWinRate()) + "%";
	text = std::to_string(winTimes) + "/" + std::to_string(winTimes + loseTimes);
	textCString = text.c_str();
	outtextxy(248, 211, textCString);

	//打印获得勋章,铜5，银10，金20
	font.lfHeight = 20; // 设置字体高度
	settextstyle(&font);// 设置字体样式

	//宠物个数徽章
	//铜
	if (pokemonSum >= 5)
	{
		text = "宠物个数徽章-铜";
		textCString = text.c_str();
		outtextxy(135, 275, textCString);
	}
	else;
	//银
	if (pokemonSum >= 10)
	{
		text = "宠物个数徽章-银";
		textCString = text.c_str();
		outtextxy(135, 316, textCString);
	}
	else;
	//金
	if (pokemonSum >= 20)
	{
		text = "宠物个数徽章-金";
		textCString = text.c_str();
		outtextxy(135, 357, textCString);
	}
	else;
	//高级宠物徽章
	//铜
	if (fullLevelSum >= 5)
	{
		text = "高级宠物徽章-铜";
		textCString = text.c_str();
		outtextxy(269, 275, textCString);
	}
	else;
	//银
	if (fullLevelSum >= 10)
	{
		text = "高级宠物徽章-银";
		textCString = text.c_str();
		outtextxy(269, 316, textCString);
	}
	else;
	//金
	if (fullLevelSum >= 20)
	{
		text = "高级宠物徽章-金";
		textCString = text.c_str();
		outtextxy(269, 357, textCString);
	}
	else;

	//打印每个背包拥有的宝可梦数

	text = std::to_string(waterPokemonBags.size());
	textCString = text.c_str();
	outtextxy(566, 124, textCString);

	text = std::to_string(firePokemonBags.size());
	textCString = text.c_str();
	outtextxy(566, 177, textCString);

	text = std::to_string(grassPokemonBags.size());
	textCString = text.c_str();
	outtextxy(566, 230, textCString);

	text = std::to_string(electricPokemonBags.size());
	textCString = text.c_str();
	outtextxy(566, 282, textCString);
}

void User::DrawUserList(int curPage, int total) const
{
	font.lfHeight = 26; // 设置字体高度
	settextstyle(&font);// 设置字体样式

	int deltaHeight = 46;
	int curHeight = 65;
	int startID = curPage * 6;
	for (int i = startID; i < (startID + 6) && i < total; i++)
	{
		if (i != userID)//不是当前正在查看的用户
		{
			putimage(45, curHeight, &imgList[0]);
		}
		else//当前正在查看的用户
		{
			putimage(45, curHeight, &imgList[1]);
		}

		std::string text = std::to_string(i);
		CString textCString = text.c_str();
		outtextxy(63, curHeight + 6, textCString);

		curHeight += deltaHeight;
	}

}

int User::RandPickLosePkm(int first, int second) const
{
	if (pokemonSum < 3)////背包里只剩1/2只宝可梦
	{
		if (first == -1)
		{
			return 0;
		}
		else if(second == -1 && pokemonSum == 2)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else//背包里有三只及以上的宝可梦
	{
		int seq = rand() % pokemonSum;
		while (seq == first || seq == second)
		{
			seq = rand() % pokemonSum;
		}
		return seq;
	}
}

void User::SetUser(std::vector<std::string> recvReponse)
{
	userID = std::stoi(recvReponse[2]);
	name = recvReponse[3];
	isOnline = std::stoi(recvReponse[4]);
	winTimes = std::stoi(recvReponse[5]);
	loseTimes = std::stoi(recvReponse[6]);
	pokemonSum = std::stoi(recvReponse[7]);
	fullLevelSum = std::stoi(recvReponse[8]);
}

void User::AddPokemonToBags(Pokemon* newPkm)
{
	pokemonSum++;

	Pokemon* newPokemon = new Pokemon(*newPkm);
	totalPokemonBags.push_back(newPokemon);

	//根据类别加进相应的背包
	switch (newPokemon->GetType())
	{
		case Type::water:
			{
				WaterPokemon* myWaterPkm = new WaterPokemon(*newPokemon);
				waterPokemonBags.push_back(myWaterPkm);
			}
			break;

		case Type::fire:
			{
				FirePokemon* myFirePkm = new FirePokemon(*newPokemon);
				firePokemonBags.push_back(myFirePkm);
			}
			break;

		case Type::grass:
			{
				GrassPokemon* myGrassPkm = new GrassPokemon(*newPokemon);
				grassPokemonBags.push_back(myGrassPkm);
			}
			break;

		default://electric
			{
				ElectricPokemon* myElectricPkm = new ElectricPokemon(*newPokemon);
				electricPokemonBags.push_back(myElectricPkm);
			}
			break;
	}//end of swtich
}

void User::ClearBags()
{
	pokemonSum = 0;
	//释放总背包
	for (UINT i = 0; i < totalPokemonBags.size(); i++)
	{
		delete totalPokemonBags[i];
		totalPokemonBags[i] = nullptr;
	}
	totalPokemonBags.clear();

	//分别释放四个类型的背包资源
	for (UINT i = 0; i < firePokemonBags.size(); i++)
	{
		delete firePokemonBags[i];
		firePokemonBags[i] = nullptr;
	}
	firePokemonBags.clear();

	for (UINT i = 0; i < grassPokemonBags.size(); i++)
	{
		delete grassPokemonBags[i];
		grassPokemonBags[i] = nullptr;
	}
	grassPokemonBags.clear();

	for (UINT i = 0; i < waterPokemonBags.size(); i++)
	{
		delete waterPokemonBags[i];
		waterPokemonBags[i] = nullptr;
	}
	waterPokemonBags.clear();

	for (UINT i = 0; i < electricPokemonBags.size(); i++)
	{
		delete electricPokemonBags[i];
		electricPokemonBags[i] = nullptr;
	}
	electricPokemonBags.clear();
}

void User::LosePokemon(int individualID)
{
	bool isFound = false;
	Type pkmType;
	int seq = 0;

	//删除总背包中的宝可梦
	for (UINT i = 0; i < totalPokemonBags.size() && !isFound; i++)
	{
		if (totalPokemonBags[i]->GetIndividualID() == individualID)
		{
			isFound = true;
			pkmType = totalPokemonBags[i]->GetType();

			seq = i;
		}
	}
	delete totalPokemonBags[seq];
	totalPokemonBags[seq] = nullptr;
	totalPokemonBags.erase(totalPokemonBags.begin() + seq);

	//删除类型背包中的宝可梦
	isFound = false;
	if (pkmType == Type::water)
	{
		for (UINT i = 0; i < waterPokemonBags.size() && !isFound; i++)
		{
			if (waterPokemonBags[i]->GetIndividualID() == individualID)
			{
				isFound = true;
				seq = i;
			}
			else;
		}
		delete waterPokemonBags[seq];
		waterPokemonBags[seq] = nullptr;
		waterPokemonBags.erase(waterPokemonBags.begin() + seq);
	}
	else if (pkmType == Type::fire)
	{
		for (UINT i = 0; i < firePokemonBags.size() && !isFound; i++)
		{
			if (firePokemonBags[i]->GetIndividualID() == individualID)
			{
				isFound = true;
				seq = i;
			}
			else;
		}
		delete firePokemonBags[seq];
		firePokemonBags[seq] = nullptr;
		firePokemonBags.erase(firePokemonBags.begin() + seq);
	}
	else if (pkmType == Type::grass)
	{
		for (UINT i = 0; i < grassPokemonBags.size() && !isFound; i++)
		{
			if (grassPokemonBags[i]->GetIndividualID() == individualID)
			{
				isFound = true;
				seq = i;
			}
			else;
		}
		delete grassPokemonBags[seq];
		grassPokemonBags[seq] = nullptr;
		grassPokemonBags.erase(grassPokemonBags.begin() + seq);
	}
	else//pkmType == Type::electric
	{
		for (UINT i = 0; i < electricPokemonBags.size() && !isFound; i++)
		{
			if (electricPokemonBags[i]->GetIndividualID() == individualID)
			{
				isFound = true;
				seq = i;
			}
			else;
		}
		delete electricPokemonBags[seq];
		electricPokemonBags[seq] = nullptr;
		electricPokemonBags.erase(electricPokemonBags.begin() + seq);
	}

	pokemonSum--;
	UpdateFullLevel();
}

void User::SetIsOnline(bool isonline)
{
	isOnline = isonline;
}

void User::SetBattleTimes(bool isWin)
{
	if (isWin)
	{
		winTimes++;
	}
	else
	{
		loseTimes++;
	}
}

void User::SetPkmSum(int sum)
{
	pokemonSum = sum;
}