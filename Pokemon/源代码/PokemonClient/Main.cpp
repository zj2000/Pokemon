#include "MainHeader.h"

bool isLogOut;

clock_t currentTime;//��ǰʱ��
clock_t lastUpdateTime;//�ϴθ��»����ʱ��
State GUIState;
Weather battleWeather;//��ս����

int main()
{
	GUIState = State::start;
	isLogOut = false;
	currentTime = clock();
	lastUpdateTime = currentTime;

	srand(clock());//�������������

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