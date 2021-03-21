#pragma once

#include "Client.h"
#include <fstream>
#include <graphics.h>
#include <conio.h>



const std::string POKEMON_SPECIES_LOC = "./res/pokemon.dat";//ֻ���ļ������޸�
const std::string POKEMON_INDIVIDUAL_LOC = "./res/pokemonIndividual.dat";//ֻ���ļ������޸�
const std::string MOVE_LOC = "./res/move.dat";//ֻ���ļ������޸�

extern std::vector<Move*> movesLibrary;
extern std::vector<Pokemon*> pokemonLibrary;
extern std::vector<Pokemon*> individualPokemon;//���屦���Σ������û��ĺ�ϵͳ���ɵ�

//���ͼ�й�
extern State GUIState;

extern IMAGE imgStart;
extern IMAGE imgMainUI;
extern IMAGE imgSeeUser;
extern IMAGE imgSeePokemon;
extern IMAGE imgBattle;
extern IMAGE imgChoose[5];

extern IMAGE imgWaiting;
extern IMAGE imgFailed;

extern IMAGE imgWin[2];
extern IMAGE imgLose[2];

extern IMAGE imgList[2];//�б�ͼ��

extern clock_t currentTime;//��ǰʱ��
extern clock_t lastUpdateTime;//�ϴθ��»����ʱ��
	
extern LOGFONT font;//����

extern int levelEXP[15];
extern bool isLogOut;


const clock_t UPDATE_INTERVAL = 50;//ÿ50����ˢ��һ�λ���
const int SERVER_PORT = 8000;
const int MAX_BUF_LEN = 1024;

const std::string imgOriginalFrontLoc = "./res/images/original/front/";
const std::string imgCoverFrontLoc = "./res/images/cover/front/";
const std::string imgOriginalBackLoc = "./res/images/original/back/";
const std::string imgCoverBackLoc = "./res/images/cover/back/";

