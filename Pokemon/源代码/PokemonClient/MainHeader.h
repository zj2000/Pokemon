#pragma once

#include "Client.h"
#include <fstream>
#include <graphics.h>
#include <conio.h>



const std::string POKEMON_SPECIES_LOC = "./res/pokemon.dat";//只读文件不许修改
const std::string POKEMON_INDIVIDUAL_LOC = "./res/pokemonIndividual.dat";//只读文件不许修改
const std::string MOVE_LOC = "./res/move.dat";//只读文件不许修改

extern std::vector<Move*> movesLibrary;
extern std::vector<Pokemon*> pokemonLibrary;
extern std::vector<Pokemon*> individualPokemon;//个体宝可梦，包括用户的和系统生成的

//与绘图有关
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

extern IMAGE imgList[2];//列表图标

extern clock_t currentTime;//当前时间
extern clock_t lastUpdateTime;//上次更新画面的时间
	
extern LOGFONT font;//字体

extern int levelEXP[15];
extern bool isLogOut;


const clock_t UPDATE_INTERVAL = 50;//每50毫秒刷新一次画面
const int SERVER_PORT = 8000;
const int MAX_BUF_LEN = 1024;

const std::string imgOriginalFrontLoc = "./res/images/original/front/";
const std::string imgCoverFrontLoc = "./res/images/cover/front/";
const std::string imgOriginalBackLoc = "./res/images/original/back/";
const std::string imgCoverBackLoc = "./res/images/cover/back/";

