#ifndef STATE
#define STATE
#include <string>
#include <vector>
#include "Board.h"


class State
{
private:
	int COLUMNS = 7, ROWS = 6;
	

public:
	//default constructor
	State();
	State(Board*);

	~State();


	//functions
	void returnBoard();
	void updateCurrentState(int, char);
	bool isColumnAvailable(int);

	Board* game;

};

#endif
