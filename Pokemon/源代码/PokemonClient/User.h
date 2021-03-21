#ifndef USER_H
#define USER_H

#include "PokemonType.h"

//客户端的用户类
class User
{
	int userID;

	bool isOnline;//是否在线

	int winTimes;
	int loseTimes;

	int pokemonSum;
	int fullLevelSum;

	std::string name; //用户名，必须唯一

	std::vector<Pokemon*>totalPokemonBags;
	std::vector<WaterPokemon*> waterPokemonBags;//水系宝可梦背包
	std::vector<FirePokemon*> firePokemonBags;//火系宝可梦背包
	std::vector<GrassPokemon*> grassPokemonBags;//草系宝可梦背包
	std::vector<ElectricPokemon*> electricPokemonBags;//电系宝可梦背包
public:
	User();
	User& operator=(const User&);
	void SetUser(std::vector<std::string> recvReponse);//根据服务端传来的数据设置用户的相关信息
	void SetIsOnline(bool isonline);//设置用户在线情况
	void ClearBags();//清空背包
	void AddPokemonToBags(Pokemon* newPokemon);//把宝可梦添加到总背包和类别背包里
	void LosePokemon(int individualID);//删除背包中战败的失去的宝可梦
	void UpdateFullLevel();//更新拥有的满级宝可梦数
	void SetBattleTimes(bool isWin);//更新胜利/失败次数
	void SetPkmSum(int sum);//设置用户拥有的宝可梦的总数

	double GetWinRate() const; //获取该用户的胜率
	int HasPokemon(Type type) const;//返回某背包中的宝可梦数
	int GetID() const;//获取用户的ID
	Pokemon* GetCurPkm(Type type, int seq) const;//获取当前查看的宝可梦
	int GetPkmSum() const;//获取用户拥有的宝可梦数
	Pokemon* GetTotalBagPkm(int seq)const;///获取宝可梦

	void DrawUserInfo(int localID, int state) const;//打印用户信息到界面上
	void DrawUserList(int curPage, int total) const;//打印用户列表到界面

	int RandPickLosePkm(int first, int second) const;//随机选取要失去的宝可梦
};

#endif // !USER_H
