#ifndef GAME
#define GAME
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "State.h"




class Game
{
private:
	int MAX_DEPTH = 8, ROWS = 6, COLUMNS = 7;


public:
	Game();
	
	//starting state of gameboard
	State* gameStartState = new State();

	//min max search code
	void minMax(State*);
	void startGame(State*);
	int min(State*, int);
	int max(State*, int);
	void checkBranches(State*, int&, int&, int);

	//evaluation functions
	int returnMaxValue(std::vector<int> &);
	int returnMinValue(std::vector<int> &);
	int preventOpponentWin(State*, int);
	int checkForWin(State*);
	int compareTiedEvals(int, int, State*);
	int evaluation(State* s);


	//test game code
	void manualUpdate();

	//code to find consecutive pieces
	int countDiagonalGroups(State*, int, int, char);
	int countOtherDiagonalGroups(State*, int, int, char);
	int countVerticalGroups(State*, int, int, char);
	int countHorizontalGroups(State*, int, int, char);
	void showExistingGroups(State*, char, int);

	//code to find end game scenarios
	bool isGameOver(State*);
	//bool playerOneWin(State*);


	//alpha beta pruning
	int negaMax(State*, int, int, int, bool);
	void runNegaMax(State*);

};

#endif