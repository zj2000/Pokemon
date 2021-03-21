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
	this->pokemonSum = 0;//ͨ��AddPokemonToBags���
	isOnline = false;
	fullLevelSum = fullLevel;
}


std::string User::GetPassword() const
{
	return password;
}

//��ĳ�����͵ı��������������
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


//��ʼ��ñ�����,ֱ����ӽ��û�������,����ֵΪ��õı����ε�����ID(id�����������id��ͬ)
int User::GainPokemon(int first, int second, int level)
{
	srand(clock());
	int size = pokemonLibrary.size();
	int speciesID = (rand() % size) / 3;//��ʼ����Ķ��ǳ��������Σ�ID��3�ı���
	while (speciesID == first || speciesID == second)
	{
		//���������ı�����ID��ǰ��ֻ��õ�һ�������������һ��
		speciesID = (rand() % size) / 3;
	}
	speciesID *= 3;

	//�������屦���ε�����ֵ���ٸ���ɸ��屦����
	int individualID = individualPokemon.size();
	Pokemon* newPokemon = new Pokemon(*pokemonLibrary[speciesID]);//��������
	//����
	newPokemon->InitIndividual(individualID,level);
	newPokemon->UpdateAttribute(level);

	//������屦���α���
	individualPokemon.push_back(newPokemon);
	
	//���ݱ�����������ӽ�����
	AddPokemonToBags(newPokemon);

	//����ӵı�����д����屦�����ļ�
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


//�ж�ĳ��Ҫд����ļ��Ƿ�Ϊ��
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


void User::AppendUserFile(std::string loc) const//�����û�
{
	/*
		�û��ļ���¼���ݼ������������£�
		 ID	| �û���	|  ����	| ��ʤ��	| ʧ����	| ӵ�б�������n	|������������	| ����������ID(n��)����	|
		 int| string| string|  int	|  int	|	  int		|    int	|		int			|
	*/

	bool isEmpty = CheckEmpty(loc);

	std::ofstream userFile;
	userFile.open(loc, std::ios::app | std::ios::out);
	if (userFile.is_open())
	{
		if (!isEmpty)//��Ϊһ�м�¼һ���û������ݣ���������ļ���Ϊ�գ����Ȼ���
		{
			userFile << '\n';
		}
		else;

		userFile << id << ' ' << name << ' ' << password << ' '
			<< winTimes << ' ' << loseTimes << ' ' << pokemonSum << ' ' << fullLevelSum;

		//��ӱ�����ID
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
		�����û���Ϣ��¼���ݼ������������£������ȫתΪstring����,�м��ÿո�����
		  �û���	|  ����/����	| ��ʤ��	| ʧ����	| ӵ�б�������n	|������������	| ����������ID(n��)����	|
		 string	|bool/string|  int	|  int	|	  int		|    int	|		int			|
	*/
	

	std::string info = name;//�û���
	if (isForFile)//������д�ļ���
	{
		info +=' ' + password;
	}
	else//���ڷ����û���
	{
		//����
		if (isOnline)
		{
			info += ' ' + std::to_string(1);
		}
		else
		{
			info += ' ' + std::to_string(0);
		}
	}

	info += ' ' + std::to_string(winTimes);//��ʤ��	
	info += ' ' + std::to_string(loseTimes);//ʧ����
	info += ' ' + std::to_string(pokemonSum);//ӵ�б�������
	info += ' ' + std::to_string(fullLevelSum);
	
	//������ID
	for (int i = 0; i < pokemonSum; i++)
	{
		info += ' ' + std::to_string(pokemonBags[i]->GetIndividualID());
	}

	return info;

}

//����ֵΪʣ��ı�������
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

	if (seq != -1)//�����д���Ҫɾ�ı�����
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