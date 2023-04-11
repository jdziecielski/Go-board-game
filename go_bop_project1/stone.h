#pragma once

class stone {
private:
	char player;
	int removedinturn;
public:
	stone() {
		removedinturn = INT_MAX;
		player = '0';
	}
	stone(char _player) {
		removedinturn = INT_MAX;
		player = _player;
	}

	int getremovedinturn() { return removedinturn; }
	void setremovedinturn(int _turnnum) { removedinturn = _turnnum; }

	char getplayer() { return player; }
	void setplayer(char _player) { this->player = _player; }
};