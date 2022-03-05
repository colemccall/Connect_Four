#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>
#include "State.h"
#include "Board.h"
using namespace std;



//set the game board upon construction
State::State()
{
	game = new Board();
}

State::~State()
{
	delete[] game;
}

State::State(Board* b)
{
	game = new Board();
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < COLUMNS; x++)
		{
			game->setAt(x, y, b->getAt(x, y));
		}
	}

}


bool State::isColumnAvailable(int column)
{
	int y = 5;
	bool isCol = false;
	while (y >= 0)
	{
		if (game->getAt(column + y * COLUMNS) == '-')
		{
			isCol = true;
			break;
		}
		else
			y--;
	}

	return isCol;
}


void State::returnBoard()
{
	cout << "0 1 2 3 4 5 6" << endl;
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < COLUMNS; x++)
		{
			cout << game->getAt(x + y * COLUMNS) << " ";
		}
		cout << endl;
	}
}

void State::updateCurrentState(int column, char playerType)
{
	int y = 5;
	while (y >= 0)
	{
		if (game->getAt(column + y * COLUMNS) == '-')
		{
			game->setAt(column, y, playerType);
			break;
		}
		else
			y--;
	}
}