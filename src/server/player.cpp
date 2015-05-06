#include "player.h"

using namespace std;
vector<Player *> Player::players = vector<Player *>();

int Player::setNickname(string nickname){
	if(nickname.length() < 3 || nickname.length() > 17){
		return 2;
	}

	for(auto &player : players){
		for(auto &c : nickname){
			if(!isalnum(c)){
				return 2;
			}
		}

		if(player->nickname == nickname){
			return 1;
		}
	}

	this->nickname = nickname;

	return 0;
};