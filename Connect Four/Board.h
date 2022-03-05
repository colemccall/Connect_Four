#ifndef BOARD
#define BOARD
#include <vector>


class Board
{
private:
	int COLUMNS = 7, ROWS = 6; 
	int gameBoard [42];
	

public:
	//constructor
	Board();
	
	//methods for setting and using the board
	void setAt(int, int, char);
	void setAt(int, char);
	char getAt(int, int);
	char getAt(int);
	/*bool isCounted(int, int);
	bool isCounted(int);
	void setCounted(int, int);
	void setCounted(int);*/
	int movesMade();

};

#endif
