#include "MainHeader.h"
#include <ctime>

User::User(int id, std::string name, std::string password)
{
	this->id = id;
	this->name = name;
	this->password = password;
	isOnline = false;

	winTimes = 0;
	loseTimes = 0;
	pokemonSum = 0;
	fullLevelSum = 0;
}

User::User(int id, std::string name, std::string password, int winTimes, int loseTimes, int fullLevel)
{
	this->id = id;
	this->name = name;
	this->password = password;
	this->winTimes = winTimes;
	this->loseTimes = loseTimes;
	this->pokemonSum = 0;//通过AddPokemonToBags添加
	isOnline = false;
	fullLevelSum = fullLevel;
}


std::string User::GetPassword() const
{
	return password;
}

//将某种类型的宝可梦添加至背包
void User::AddPokemonToBags(Pokemon* newPokemon)
{
	pokemonSum++;
	pokemonBags.push_back(newPokemon);
	if (newPokemon->GetLevel() == 15)
	{
		fullLevelSum++;
	}
	else
		;
}


//初始获得宝可梦,直接添加进用户背包中,返回值为获得的宝可梦的种族ID(id不能与参数的id相同)
int User::GainPokemon(int first, int second, int level)
{
	srand(clock());
	int size = pokemonLibrary.size();
	int speciesID = (rand() % size) / 3;//初始分配的都是初级宝可梦，ID是3的倍数
	while (speciesID == first || speciesID == second)
	{
		//如果随机出的宝可梦ID跟前两只获得的一样，则重新随机一次
		speciesID = (rand() % size) / 3;
	}
	speciesID *= 3;

	//拷贝种族宝可梦的属性值，再改造成个体宝可梦
	int individualID = individualPokemon.size();
	Pokemon* newPokemon = new Pokemon(*pokemonLibrary[speciesID]);//拷贝种族
	//改造
	newPokemon->InitIndividual(individualID,level);
	newPokemon->UpdateAttribute(level);

	//加入个体宝可梦表中
	individualPokemon.push_back(newPokemon);
	
	//根据宝可梦类型添加进背包
	AddPokemonToBags(newPokemon);

	//新添加的宝可梦写入个体宝可梦文件
	newPokemon->AppendIndividualFile();

	return speciesID;
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


//判断某个要写入的文件是否为空
bool User::CheckEmpty(std::string loc)const
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
	}
	else
	{
		isEmpty = true;
	}
	in.close();

	return isEmpty;
}


void User::AppendUserFile(std::string loc) const//增加用户
{
	/*
		用户文件记录内容及数据类型如下：
		 ID	| 用户名	|  密码	| 获胜数	| 失败数	| 拥有宝可梦数n	|满级宝可梦数	| ……宝可梦ID(n个)……	|
		 int| string| string|  int	|  int	|	  int		|    int	|		int			|
	*/

	bool isEmpty = CheckEmpty(loc);

	std::ofstream userFile;
	userFile.open(loc, std::ios::app | std::ios::out);
	if (userFile.is_open())
	{
		if (!isEmpty)//因为一行记录一个用户的数据，所以如果文件不为空，则先换行
		{
			userFile << '\n';
		}
		else;

		userFile << id << ' ' << name << ' ' << password << ' '
			<< winTimes << ' ' << loseTimes << ' ' << pokemonSum << ' ' << fullLevelSum;

		//添加宝可梦ID
		int size = pokemonBags.size();
		for (int i = 0; i < size; i++)
		{
			userFile << ' ' << pokemonBags[i]->GetIndividualID();
		}
		
	}
}	

void User::SetIsOnline(bool isUserOniline)
{
	isOnline = isUserOniline;
}

std::string User::GetName() const
{
	return name;
}

int User::GetID() const
{
	return id;
}

std::string User::GetUserInfo(bool isForFile) const
{
	/*
		返回用户信息记录内容及数据类型如下：（最后全转为string返回,中间用空格间隔）
		  用户名	|  在线/密码	| 获胜数	| 失败数	| 拥有宝可梦数n	|满级宝可梦数	| ……宝可梦ID(n个)……	|
		 string	|bool/string|  int	|  int	|	  int		|    int	|		int			|
	*/
	

	std::string info = name;//用户名
	if (isForFile)//用于重写文件的
	{
		info +=' ' + password;
	}
	else//用于发给用户的
	{
		//在线
		if (isOnline)
		{
			info += ' ' + std::to_string(1);
		}
		else
		{
			info += ' ' + std::to_string(0);
		}
	}

	info += ' ' + std::to_string(winTimes);//获胜数	
	info += ' ' + std::to_string(loseTimes);//失败数
	info += ' ' + std::to_string(pokemonSum);//拥有宝可梦数
	info += ' ' + std::to_string(fullLevelSum);
	
	//宝可梦ID
	for (int i = 0; i < pokemonSum; i++)
	{
		info += ' ' + std::to_string(pokemonBags[i]->GetIndividualID());
	}

	return info;

}

//返回值为剩余的宝可梦数
int User::LosePokemon(int pkmID)
{
	bool isFound = false;
	int seq = -1;
	for (int i = 0; i < pokemonSum && !isFound; i++)
	{
		if (pokemonBags[i]->GetIndividualID() == pkmID)
		{
			isFound = true;
			seq = i;
		}
	}

	if (seq != -1)//背包中存在要删的宝可梦
	{
		if (pokemonBags[seq]->GetLevel() == 15)
		{
			fullLevelSum--;
		}
		else
			;
		pokemonBags.erase(pokemonBags.begin() + seq);
		pokemonSum--;
		
	}
	else
		;

	return pokemonSum;
}