#include "MainHeader.h"

//std::vector<Pokemon*> pokemonLibrary;//����С����ͼ��
//std::vector<Move*> MovesTable;//�����ο���ѧϰ����ʽ��

int levelEXP[15] = {0, 274, 2195, 7408, 17561,
					34300, 59270, 94119, 140492, 200037,
					274400, 365226, 474163, 602856, 750000};//ÿ���ȼ�����Ҫ�ľ���ֵ


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