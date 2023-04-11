#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include<stdio.h>
#include<cstring>
#include "conio2.h"
#include "stone.h"

//starting position of drawing the board
#define BOARDSTARTX 60
#define BOARDSTARTY 7

#define DIRECTIONS 4

struct adjacentpoints { int direction[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} }; };

void drawlegend(int _zero, char* _txt, int _zn, char* _cursorxy, char* _turninfo, int _whiteplayerscore, int _blackplayerscore);
void drawboard(int _isgamestarted, int _cursorx, int _cursory, stone** _stonemap, char _turn, int _attr, int _back, int _boardsizex, int _boardsizey, int _turnnum);
int getliberties(stone** _stonemap, int _posx, int _posy, char _player, int _boardsizex, int _boardsizey);
void savetofile(stone** _stonemap, stone** _maphistory, char _currentturn, int _whiteplayerscore, int _blackplayerscore, int _boardsizex, int _boardsizey, int _turnnum);
void loadfromfile(stone** _stonemap, stone** _maphistory, char& _currentturn, int& _whiteplayerscore, int& _blackplayerscore, int _isboardallocated, int _boardsizex, int _boardsizey, int& _turnnum);
void startnewgame(stone** _stonemap, int& _isgamestarted, int& _blackplayerscore, int& _whiteplayerscore, char& _turn, int& _isstartingmenu, int _isboardallocated, int _boardsizex, int _boardsizey, int& _turnnum);
void resetstones(stone** _stonemap, int _boardsizex, int _boardsizey, int _isboardallocated);
void setstone(stone** _stonemap, char& _turn, int& _blackplayerscore, int& _whiteplayerscore, int _x, int _y, int _boardsizex, int _boardsizey, int& _turnnum, stone** _maphistory);
void startingmenu(int& _isstartingmenu, int& _isgamestarted, int _zn, int& _boardsizex, int& _boardsizey, int& _ifallocateboard, int& _cursorx, int& _cursory);
void gamestateeditor(stone** _stonemap, int _x, int _y, int& _isgamestateeditor, int _zn);
void updatemaphistory(stone** _stonemap, stone** _maphistory, int _boardsizex, int _boardsizey, int _turnnum);

int main() {
	int zn = 0, x = BOARDSTARTX, y = BOARDSTARTY, attr = 7, back = 0, zero = 0, whiteplayerscore = 0, blackplayerscore = 0, turnnum;
	int isgamestateeditor = 0, isstartingmenu = 0, ifallocateboard = 0, isboardallocated = 0, isgamestarted = 0, arestonesplaced = 0;		//flags
	char cursorinfo[32], txt[32], turninfo[32];
	char turn = 'b';
	int boardsizex = NULL;
	int boardsizey = NULL;
	stone** stonemap = nullptr;					//stores current state of the game
	stone** maphistory = nullptr;				//stores state of the game 2 turns back; used to support KO rule

#ifndef __cplusplus
	Conio2_Init();
#endif
	settitle("Jan, Dziecielski, 191702");		//settitle sets the window title

	_setcursortype(_NOCURSOR);					//hide the blinking cursor
	do {
		drawlegend(zero, txt, zn, cursorinfo, turninfo, whiteplayerscore, blackplayerscore);
		if (isstartingmenu) startingmenu(isstartingmenu, isgamestarted, zn, boardsizex, boardsizey, ifallocateboard, x, y);
		if (isgamestarted) {
			if (ifallocateboard) {
				stonemap = new stone * [boardsizex];
				maphistory = new stone * [boardsizex];
				for (int i = 0; i < boardsizex; i++) {
					stonemap[i] = new stone[boardsizey];
					maphistory[i] = new stone[boardsizey];
				}

				for (int y = 0; y < boardsizey; y++) {
					for (int x = 0; x < boardsizex; x++) {
						stonemap[x][y] = stone();
						maphistory[x][y] = stone();
					}
				}

				ifallocateboard = 0;
				isboardallocated = 1;
				arestonesplaced = 0;
			}

			if (isgamestateeditor) gamestateeditor(stonemap, x, y, isgamestateeditor, zn);

			if (isboardallocated) drawboard(isgamestarted, x, y, stonemap, turn, attr, back, boardsizex, boardsizey, turnnum);
		}

		zero = 0;
		zn = getch();

		//handling controls
		if (zn == 0) {
			zero = 1;			//if this is the case then we read
			zn = getch();		//the next code knowing that this
			if (zn == 0x48 && y != BOARDSTARTY) y--;							//move up
			else if (zn == 0x50 && y != boardsizey + BOARDSTARTY - 1) y++;		//move down
			else if (zn == 0x4b && x != BOARDSTARTX) x--;						//move left
			else if (zn == 0x4d && x != boardsizex + BOARDSTARTX - 1) x++;		//move right
		}
		else if (zn == ' ') attr = (attr + 1) % 16;
		else if (zn == 0x63) back = (back + 1) % 16;		//enter key is 0x0d or '\r'
		else if (zn == 0x6e) {								//new game
			isgamestateeditor = 0;
			startnewgame(stonemap, isgamestarted, blackplayerscore, whiteplayerscore, turn, isstartingmenu, isboardallocated, boardsizex, boardsizey, turnnum);
		}
		else if (zn == 0x69 && isgamestarted) {				//placing stones (i command)
			arestonesplaced = 1;
			setstone(stonemap, turn, blackplayerscore, whiteplayerscore, x, y, boardsizex, boardsizey, turnnum, maphistory);	
		}
		else if (zn == 0x66) {								//finish the game
			if (isboardallocated) {
				for (int i = 0; i < boardsizex; i++) {
					delete stonemap[i];
					delete maphistory[i];
				}
				delete[] stonemap;
				delete[] maphistory;
			}
			isgamestarted = 0;
			isboardallocated = 0;
		}
		else if (zn == 0x73 && isgamestarted) savetofile(stonemap, maphistory, turn, whiteplayerscore, blackplayerscore, boardsizex, boardsizey, turnnum);		//save game
		else if (zn == 0x6c && isgamestarted) loadfromfile(stonemap, maphistory, turn, whiteplayerscore, blackplayerscore, isboardallocated, boardsizex, boardsizey, turnnum);	//load game
		else if (zn == 0x65 && isgamestarted && !arestonesplaced) isgamestateeditor = 1;		//game state editor (handicap)
	
	} while (zn != 'q');

	if (isboardallocated) {
		for (int i = 0; i < boardsizex; i++) {
			delete stonemap[i];
			delete maphistory[i];
		}
		delete[] stonemap;
		delete[] maphistory;
	}	

	_setcursortype(_NORMALCURSOR);	// show the cursor so it will be
					// visible after the program ends
	return 0;
}

//drawing legend on the leftside of the window
void drawlegend(int _zero, char* _txt, int _zn, char* _cursorxy, char* _turninfo, int _whiteplayerscore, int _blackplayerscore) {
	char whiteplayerinfo[32];
	char blackplayerinfo[32];
	// we set black to be the background color
	// check conio2.h for available colors
	textbackground(BLACK);
	// clear the screen: we fill it out with spaces with
	// the specified background color
	clrscr();
	// we set the text color (7 == LIGHTGRAY)
	textcolor(7);
	// we move the coursor to column 48 and row 1
	// rows and column are numbered starting with 1
	gotoxy(1, 1);
	cputs("Jan Dziecielski, index: 191702");
	gotoxy(1, 2);
	// we print out a text at a given cursor position
	// the cursor will move by the length of the text
	cputs("q       = exit");
	gotoxy(1, 3);
	cputs("cursors = moving");
	gotoxy(1, 4);
	cputs("space   = change color");
	gotoxy(1, 5);
	cputs("c       = change background color");
	// print out the code of the last key pressed
	if (_zero) sprintf(_txt, "key code: 0x00 0x%02x", _zn);
	else sprintf(_txt, "key code: 0x%02x", _zn);
	gotoxy(1, 6);
	cputs("n       = start a new game");
	gotoxy(1, 7);
	cputs("i       = place a stone on the board");
	gotoxy(1, 8);
	cputs("s       = save the game state");
	gotoxy(1, 9);
	cputs("l       = load the game state");
	gotoxy(1, 10);
	cputs("f       = finish the game");
	gotoxy(1, 11);
	cputs("e       = game state editor");
	gotoxy(1, 12);
	sprintf(blackplayerinfo, "Black player score: %d", _blackplayerscore);
	cputs(blackplayerinfo);
	gotoxy(1, 13);
	sprintf(whiteplayerinfo, "White player score: %d", _whiteplayerscore);
	cputs(whiteplayerinfo);
	gotoxy(1, 14);
	cputs(_txt);
}

//drawing board, stones, cursor, score, turn information
void drawboard(int _isgamestarted, int _cursorx, int _cursory, stone** _stonemap, char _turn, int _attr, int _back, int _boardsizex, int _boardsizey, int _turnnum) {
	char cursorinfo[32], turninfo[32];
	int iscursorposition = 0;

	//drawing board
	for (int y = BOARDSTARTY; y < BOARDSTARTY + _boardsizey; y++) {
		for (int x = BOARDSTARTX; x < BOARDSTARTX + _boardsizex; x++) {
			
			if (x == _cursorx && y == _cursory)
				iscursorposition = 1;

			gotoxy(x, y);
			if (!iscursorposition) {
				if (y == BOARDSTARTY || y == (BOARDSTARTY + _boardsizey - 1))
					cputs("_");
				else if (x == BOARDSTARTX || x == (BOARDSTARTX + _boardsizex - 1))
					cputs("|");
				else
					cputs(".");
			}
			iscursorposition = 0;	
		}
	}

	if (_isgamestarted) {
		//cursor's position
		gotoxy(1, 18);
		cputs("The game has started!");
		sprintf(cursorinfo, "X: %d Y: %d", _cursorx - BOARDSTARTX + 1, _cursory - BOARDSTARTY + 1);
		gotoxy(BOARDSTARTX, BOARDSTARTY - 2);
		cputs(cursorinfo);

		//turn information
		sprintf(turninfo, "Turn: %d", _turnnum);
		gotoxy(BOARDSTARTX, BOARDSTARTY - 3);
		cputs(turninfo);
		if (_turn == 'w')
			sprintf(turninfo, "%s player turn...", "White");
		else if (_turn == 'b')
			sprintf(turninfo, "%s player turn...", "Black");
		gotoxy(BOARDSTARTX, BOARDSTARTY - 1);
		cputs(turninfo);

		//drawing stones
		for (int y = 0; y < _boardsizey; y++) {
			for (int x = 0; x < _boardsizex; x++) {
				gotoxy(x + BOARDSTARTX, y + BOARDSTARTY);
				if (_stonemap[x][y].getplayer() == 'w') cputs("W");
				else if (_stonemap[x][y].getplayer() == 'b') cputs("B");
			}
		}

		//drawing cursor
		gotoxy(_cursorx, _cursory);
		textcolor(_attr);
		textbackground(_back);
		// putch prints one character and moves the cursor to the right
		putch('o');
	}
}

//evaluate liberties of stone at given posx posy
int getliberties(stone** _stonemap, int _posx, int _posy, char _player, int _boardsizex, int _boardsizey) {
	int liberties = 4, x, y;
	adjacentpoints directions;
	for (int i = 0; i < DIRECTIONS; i++) {
		x = _posx + directions.direction[i][0];
		y = _posy + directions.direction[i][1];
		//if a stone is placed next to the wall OR is placed next to the stone of opposite player then liberties--
		if (_player == 'w' && (x == -1 || y == -1 || x == _boardsizex || y == _boardsizey || _stonemap[x][y].getplayer() == 'b')) liberties--; 
		else if (_player == 'b' && (x == -1 || y == -1 || x == _boardsizex || y == _boardsizey || _stonemap[x][y].getplayer() == 'w')) liberties--;
	}
	return liberties;
}

//saving a gamestate to a given file
void savetofile(stone** _stonemap, stone** _maphistory ,char _currentturn, int _whiteplayerscore, int _blackplayerscore, int _boardsizex, int _boardsizey, int _turnnum) {
	FILE* file;
	int i = 5, removedinturn;
	char player;
	char textline[64];
	char filepath[32] = "saves/";
	char filetosave[32];
	char c;
	gotoxy(BOARDSTARTX, 1);
	cputs("Enter the name of the file to save the game status... \n");
	gotoxy(BOARDSTARTX, 2);
	do {
		i++;
		c = getche();
		strncat(filepath, &c, 1);
	} while (c != '\r');
	filepath[i] = '\0';
	sprintf(filetosave, "%s%s", filepath, ".txt");
	file = fopen(filetosave, "w");
	if (file != NULL) {
		sprintf(textline, "%d %d %d %c\n", _blackplayerscore, _whiteplayerscore, _turnnum, _currentturn);
		fputs(textline, file);
		for (int y = 0; y < _boardsizey; y++) {
			for (int x = 0; x < _boardsizex; x++) {
				if (_stonemap[x][y].getplayer() != '0') {
					player = _stonemap[x][y].getplayer();
					sprintf(textline, "%c %d %d %c\n", 'O', x, y, player);
					fputs(textline, file);
				}
				
				if (_maphistory[x][y].getplayer() != '0') {
					player = _maphistory[x][y].getplayer();
					removedinturn = _maphistory[x][y].getremovedinturn();
					sprintf(textline, "%c %d %d %d %c\n", 'B', x, y, removedinturn, player);
					fputs(textline, file);
				}
			}
		}
		fclose(file);
	}
}

//loading a gamestate from a given file
void loadfromfile(stone** _stonemap, stone** _maphistory, char& _currentturn, int& _whiteplayerscore, int& _blackplayerscore, int _isboardallocated, int _boardsizex, int _boardsizey, int& _turnnum) {
	FILE* pFile;
	char line[16], player, c, filetosave[32];
	char filepath[32] = "saves/";
	char* linehelp;
	char toconvert[8];
	int linenum = 0, j = 0, x, y, i = 6, removedinturn;
	gotoxy(BOARDSTARTX, 1);
	cputs("Enter the name of the file to load the game status...");
	gotoxy(BOARDSTARTX, 2);	
	do {
		i++;
		c = getche();
		strncat(filepath, &c, 1);
	} while (c != '\r');
	filepath[i - 1] = '\0';
	sprintf(filetosave, "%s%s", filepath, ".txt");
	filetosave[i + 4] = NULL;
	pFile = fopen(filetosave, "r");
	if (pFile != NULL) {
		resetstones(_stonemap, _boardsizex, _boardsizey, _isboardallocated);
		while (fgets(line, 16, pFile) != NULL) {
			linehelp = strtok(line, " ");
			while (linehelp != NULL) {
				if (linenum == 0) {					//loading information related to score and turn
					if (j == 0) _blackplayerscore = atoi(linehelp);
					else if (j == 1) _whiteplayerscore = atoi(linehelp);
					else if (j == 2) _turnnum = atoi(linehelp);
					else if (j == 3) _currentturn = linehelp[0];
					j++;
				}
				else {								// loading information related to stones
					if (line[0] == 'O') {			// O stands for original map
						if (j == 1) x = atoi(linehelp);
						else if (j == 2) y = atoi(linehelp);
						else if (j == 3 && x < _boardsizex && y < _boardsizey) _stonemap[x][y].setplayer(linehelp[0]);
						j++;
					}
					else if (line[0] == 'B') {		// B stands for backup map
						if (j == 1) x = atoi(linehelp);
						else if (j == 2) y = atoi(linehelp);
						else if (j == 3) removedinturn = atoi(linehelp);
						else if (j == 4 && x < _boardsizex && y < _boardsizey) {
							player = linehelp[0];
							_maphistory[x][y].setplayer(player);
							_maphistory[x][y].setremovedinturn(removedinturn);
						}
						j++;
					}
					
				}
				linehelp = strtok(NULL, " ");
			}
			j = 0;
			linenum++;
		}
		updatemaphistory(_stonemap, _maphistory, _boardsizex, _boardsizey, _turnnum);
		fclose(pFile);
	}
}

//starting a new game
void startnewgame(stone** _stonemap, int& _isgamestarted, int& _blackplayerscore, int& _whiteplayerscore, char& _turn, int& _isstartingmenu, int _isboardallocated, int _boardsizex, int _boardsizey, int& _turnnum) {
	_turn = 'b';
	_isstartingmenu = 1;
	_turnnum = 1;
	// clean-up stones before starting a new game
	if (_isboardallocated)
		resetstones(_stonemap, _boardsizex, _boardsizey, _isboardallocated);

	_whiteplayerscore = 0;
	_blackplayerscore = 0;
}

//setting all stones to '0' value so they won't be detected as any of the players'
void resetstones(stone** _stonemap, int _boardsizex, int _boardsizey, int _isboardallocated) {
	if (_isboardallocated) {
		for (int y = 0; y < _boardsizey; y++)
			for (int x = 0; x < _boardsizex; x++)
				_stonemap[x][y].setplayer('0');
	}
}

//placing the stone on the board and simple capturing of stones
void setstone(stone** _stonemap, char &_turn, int& _blackplayerscore, int& _whiteplayerscore, int _x, int _y, int _boardsizex, int _boardsizey, int& _turnnum, stone** _maphistory) {
	int posx = _x - BOARDSTARTX, posy = _y - BOARDSTARTY, x, y;
	int liberties = getliberties(_stonemap, posx, posy, _turn, _boardsizex, _boardsizey);
	int wasstonecaptured = 0;
	adjacentpoints directions;

	char oppositeplayer = '0';
	if (_turn == 'b') oppositeplayer = 'w';
	else if (_turn == 'w') oppositeplayer = 'b';

	if (_stonemap[posx][posy].getplayer() == '0') {

		if (_maphistory[posx][posy].getplayer() != _turn)	//applying KO rule
			_stonemap[posx][posy] = stone(_turn);

		//capturing
		for (int i = 0; i < DIRECTIONS; i++) {
			x = posx + directions.direction[i][0];
			y = posy + directions.direction[i][1];

			if (x >= 0 && y >= 0 && x < _boardsizex && y < _boardsizey) {
				if (_stonemap[x][y].getplayer() == oppositeplayer && getliberties(_stonemap, x, y, oppositeplayer, _boardsizex, _boardsizey) == 0) {
					_maphistory[x][y].setremovedinturn(_turnnum);
					_stonemap[x][y] = stone();
					wasstonecaptured = 1;
				}
			}
		}

		//if -> preventing obvious suicide, else -> changing the turn, incrementing score of the right player
		if (getliberties(_stonemap, posx, posy, _turn, _boardsizex, _boardsizey) == 0 && !wasstonecaptured) _stonemap[posx][posy] = stone();
		else {

			if (wasstonecaptured) {
				if (_turn == 'w') _whiteplayerscore++;
				else if (_turn == 'b') _blackplayerscore++;
			}

			updatemaphistory(_stonemap, _maphistory, _boardsizex, _boardsizey, _turnnum);
			_maphistory[posx][posy] = stone(_turn);
			_turnnum++;
			if (_turn == 'w') _turn = 'b';
			else if (_turn == 'b') _turn = 'w';
		}
	}
}

//menu for selecting the size of the board
void startingmenu(int& _isstartingmenu, int& _isgamestarted, int _zn, int& _boardsizex, int& _boardsizey, int& _ifallocateboard, int& _cursorx, int& _cursory) {
	int issizechosen = 0;
	int ifshowoptions = 1;

	if (!issizechosen) {
		if (_zn == 0x31) {				//9x9 option
			_boardsizex = 9;
			_boardsizey = 9;
			issizechosen = 1;
			ifshowoptions = 0;
		}
		else if (_zn == 0x32) {			//13x13 option
			_boardsizex = 13;
			_boardsizey = 13;
			issizechosen = 1;
			ifshowoptions = 0;
		}
		else if (_zn == 0x33) {			//19x19 option
			_boardsizex = 19;
			_boardsizey = 19;
			issizechosen = 1;
			ifshowoptions = 0;
		}
		else if (_zn == 0x34) {			//custom size option
			ifshowoptions = 0;
			int ifxchosen = 0, ifychosen = 0, i = 0;
			char sizex[16] = { "0" };
			char sizey[16] = { "0" };
			char c;

			//selecting width of the board
			if (!ifxchosen) {
				gotoxy(60, 1);
				cputs("Enter the width of the board (x)...");
				gotoxy(60, 2);
				do {
					c = getche();
					sizex[i] = c;
					i++;
				} while (c != '\r');
				sizex[i - 1] = '\0';
				i = 0;
				ifxchosen = 1;
			}

			//selecting height of the board
			if (!ifychosen && ifxchosen) {
				gotoxy(60, 1);
				cputs("Enter the height of the board (y)...");
				gotoxy(60, 2);
				do {
					c = getche();
					sizey[i] = c;
					i++;
				} while (c != '\r');
				sizey[i - 1] = '\0';
				i = 0;
				ifychosen = 1;
			}

			//setting board size for custom size option
			if (ifxchosen && ifychosen) {
				_boardsizex = atoi(sizex);
				_boardsizey = atoi(sizey);
				issizechosen = 1;
				ifshowoptions = 0;
			}	
		}
	}
	
	//resetting cursor position, disabling menu for choosing size, enabling flags for program to continue
	if (issizechosen) {
		_cursorx = BOARDSTARTX;
		_cursory = BOARDSTARTY;
		_isstartingmenu = 0;
		_isgamestarted = 1;
		_ifallocateboard = 1;
	}

	//printing legend to the screen
	if (ifshowoptions) {
		gotoxy(60, 1);
		cputs("Press a number to choose the size of the board...");
		gotoxy(60, 3);
		cputs("1. 9x9");			//0x31
		gotoxy(60, 4);
		cputs("2. 13x13");			//0x32
		gotoxy(60, 5);
		cputs("3. 19x19");			//0x33
		gotoxy(60, 6);
		cputs("4. Enter custom size...");
	}
}

//editing game state for black player
void gamestateeditor(stone** _stonemap, int _x, int _y, int& _isgamestateeditor, int _zn) {
	int posx, posy;

	if (_zn == 0x0d) _isgamestateeditor = 0;

	if (_isgamestateeditor) {
		
		gotoxy(60, 1);
		cputs("Game state editor...");
		gotoxy(60, 2);
		cputs("Press P to place a stone, enter to exit editor...");

		if (_zn == 0x70) {
			posx = _x - BOARDSTARTX;
			posy = _y - BOARDSTARTY;

			if (_stonemap[posx][posy].getplayer() == '0') _stonemap[posx][posy] = stone('b');
		}
	}
}

//update map helping with applying KO rule
void updatemaphistory(stone** _stonemap, stone** _maphistory, int _boardsizex, int _boardsizey, int _turnnum) {
	for (int y = 0; y < _boardsizey; y++) {
		for (int x = 0; x < _boardsizex; x++) {
			if (_stonemap[x][y].getplayer() == '0' && _maphistory[x][y].getremovedinturn() <= _turnnum - 2) _maphistory[x][y] = stone();
		}
	}
}