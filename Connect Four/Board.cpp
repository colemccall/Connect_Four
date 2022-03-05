#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>
#include "Board.h"
using namespace std;

Board::Board()
{
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 7; x++)
		{
			gameBoard[x + (y * COLUMNS)] = '-';	
		}
	}
}

char Board::getAt(int column, int row)
{
	//return gameBoard.at(column + (row * COLUMNS));
	return gameBoard[(column + (row * COLUMNS))];
}

char Board::getAt(int i)
{
	//return gameBoard.at(i);
	return gameBoard[i];
}

void Board::setAt(int column, int row, char set)
{
	//gameBoard.at(column + (row * COLUMNS)) = set;
	gameBoard[(column + (row * COLUMNS))] = set;
}

void Board::setAt(int i, char set)
{
	//gameBoard.at(i) = set;
	gameBoard[i] = set;
}

int Board::movesMade()
{
	//return gameBoard.size();
	return sizeof(gameBoard);
}
