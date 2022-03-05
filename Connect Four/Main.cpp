#include <iostream>
#include <math.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <deque>          
#include <list>           
#include <queue> 
#include <stack>
#include <chrono>
#include "Game.h"
using namespace std;

//prototypes
void instructions();

int main()
{
	instructions();

	Game* connectFour = new Game();
	State* start = new State();

	string gameType;
	bool startGameLoop = true;
	while (startGameLoop)
	{
		cout << "Would you like to play with a friend, or against the AI" << endl
			<< "[A] Manual Input (Human vs Human)" << endl
			<< "[B] MinMax AI" << endl
			<< "[C] negaMax AI" << endl;
		getline(cin, gameType);

		if (gameType == "A" || gameType == "a")
		{
			startGameLoop = false;
			connectFour->manualUpdate();
		}
		else if (gameType == "B" || gameType == "b")
		{
			startGameLoop = false;
			connectFour->startGame(start);
		}
		else if (gameType == "C" || gameType == "c")
		{
			startGameLoop = false;
			connectFour->runNegaMax(start);
		}
		else
		{
			cout << endl << "Invalid response, please try again" << endl;
			instructions();
		}
	}
}

void instructions()
{
	cout << endl << "AI Connect Four Agent" << endl
		<< endl << "Press 1 to run human vs human - two humans playing connect four vs one another"
		<< endl << "Press 2 to run minimax algorithm - slower and used for test and comparision"
		<< endl << "Press 3 to run negamax algorithm - used for competition" << endl << endl;
}