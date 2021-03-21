#ifndef USER_H
#define USER_H

#include "Pokemon.h"

class Medal
{
	std::string medalName;
};

class User
{
	int id;
	int winTimes;	//获胜次数
	int loseTimes;	//失败次数

	int pokemonSum;//拥有宝可梦数
	int fullLevelSum;//拥有高级宝可梦数（15级）

	bool isOnline;//该用户当前是否在线

	std::string name; //用户名，必须唯一
	std::string password; // 密码，是否要限定标准

	std::vector<Pokemon*> pokemonBags;//该用户的精灵背包

public:
	User(int id, std::string name, std::string password);//构造
	User(int id, std::string name, std::string password,int winTimes, int loseTimes,int fulllevel);

	int GainPokemon(int first, int second, int level);//初始获得宝可梦,返回值为获得的宝可梦的种族ID
	std::string GetName()const;//获取该用户的名字
	int GetID()const;//获取该用户的ID
	std::string GetPassword() const;//获取该用户的密码
	std::string GetUserInfo(bool isForFile) const;//获取该用户的信息

	void SetIsOnline(bool isUserOnline);//设置该用户在线
	void SetBattleTimes(bool isWin);//更新胜利/失败次数

	void AddPokemonToBags(Pokemon* newPokemon);//给该用户的背包添加宝可梦
	int LosePokemon(int pkmID);//用户战败失去宝可梦

	//文件
	bool CheckEmpty(std::string loc)const;
	void AppendUserFile(std::string loc) const;//增加用户
	
};

#endif // !USER_H

