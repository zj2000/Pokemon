#include "MainHeader.h"

//std::vector<Pokemon*> pokemonLibrary;//宠物小精灵图鉴
//std::vector<Move*> MovesTable;//宝可梦可以学习的招式表

int levelEXP[15] = {0, 274, 2195, 7408, 17561,
					34300, 59270, 94119, 140492, 200037,
					274400, 365226, 474163, 602856, 750000};//每个等级所需要的经验值


int main()
{
	Server server;
	bool isSuccess = server.InitServer();
	if (isSuccess)
	{
		server.Run();
	}
	else
		;
}