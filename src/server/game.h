#pragma once
class Game;

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include "player.h"
#include "../shared/components.h"

using namespace std;

class Game{
public:

	Game(Player *player){
		id = games.size();
		games.push_back(this);

		players.push_back(player);
		onTurnIndex =-1;
	};

	~Game(){
		games[id]=NULL;
	};

	bool addPlayer(Player *player);
	int removePlayer(Player *player);
	Player * getLeader();
	bool isSomeoneReady();

	bool save(std::string filename);

	std::string getGameList();
	bool loadGame(std::string filename);
	bool createGame(std::string settings);


	void sendInit();
	void sendUpdate(std::string);

	void nextTurn();
	bool isOnTurn(Player *p){ return p == players[onTurnIndex]; }

	bool doRotate();
	bool doShift(std::string data);
	bool doMove(std::string data);

	bool isWin();

	void cancel(); //leave all

private:

	int onTurnIndex;
	vector<Player *> players;
	int id;
	Board * board;
	Pack * pack;


	static vector<Game *> games;
};