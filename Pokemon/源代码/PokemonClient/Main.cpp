#include "MainHeader.h"

bool isLogOut;

clock_t currentTime;//当前时间
clock_t lastUpdateTime;//上次更新画面的时间
State GUIState;
Weather battleWeather;//对战天气

int main()
{
	GUIState = State::start;
	isLogOut = false;
	currentTime = clock();
	lastUpdateTime = currentTime;

	srand(clock());//设置随机数种子

	Client client;
	client.InitWSA();
	while (!isLogOut)
	{
		client.HandleClickEvent();
		client.RecvServerMsg();

		if (GUIState == State::battle)
		{
			client.AutoBattle();
		}
		else
			;

		client.Draw();
	}
}