#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <execution>
#include "Game.h"
#include "State.h"
#include "Omp.h"
using namespace std;


//set the game board upon construction
Game::Game()
{
	
}

bool Game::isGameOver(State* s)
{
	//if someone has won, return true
	int y = 0; 
	while (y < 6)
	{
		int x = 0;
		while (x < 7)
		{
			if (countHorizontalGroups(s, x, y, 'X') >= 4 || countHorizontalGroups(s, x, y, 'O') >= 4
				|| countVerticalGroups(s, x, y, 'X') >= 4 || countVerticalGroups(s, x, y, 'O') >= 4
				|| countDiagonalGroups(s, x, y, 'X') >= 4 || countDiagonalGroups(s, x, y, 'O') >= 4
				|| countOtherDiagonalGroups(s, x, y, 'X') >= 4 || countOtherDiagonalGroups(s, x, y, 'O') >= 4)
			{
				return true;
				break;
			}

			x++;
		}
		y++;
	}


	//if board is full, return true
	int i = 0;
	while (i < s->game->movesMade())
	{
		if (s->game->getAt(i) == '-')
		{
			return false;
			break;
		}
		i++;
	}


	//all else, return false
	return false;
}


void Game::startGame(State* s)
{
	bool findStartingPlayer = true;
	while (findStartingPlayer)
	{
		string temp;
		cout << "Who is starting first?" << endl
			<< "[1] - Player 1/Computer" << endl
			<< "[2] - Player 2/Opponent" << endl;
		getline(cin, temp);

		if (temp == "1")
		{
			findStartingPlayer = false;
			minMax(s);
		}
		else if (temp == "2")
		{
			findStartingPlayer = false;
			int update;
			bool valid = false;
			string input;
			s->returnBoard();
			while (!valid)
			{
				cout << endl << "What column did opponent select to play: ";
				getline(cin, input);
				try
				{
					update = stoi(input);
					break;
				}
				catch (exception e)
				{
					cout << endl << "Input could not be translated to int. Please try again" << endl;
				}
			}
			s->updateCurrentState(update, 'O');
			s->returnBoard();
			minMax(s);
		}
		else
		{
			cout << "Invalid response, please try again" << endl;
		}
	}
}


void Game::runNegaMax(State* s)
{
	cout << endl << "Connect Four game with NegaMax AI" << endl << endl;


	//figure out game board set up
	//find who goes first and set the board accordingly
	bool findStartingPlayer = true;
	while (findStartingPlayer)
	{
		string temp;
		cout << "Who is starting first?" << endl
			<< "[1] - Player 1/Computer" << endl
			<< "[2] - Player 2/Opponent" << endl;
		getline(cin, temp);

		if (temp == "1")
		{
			findStartingPlayer = false;
		}
		else if (temp == "2")
		{
			findStartingPlayer = false;
			int update;
			bool valid = false;
			string input;
			s->returnBoard();
			while (!valid)
			{
				cout << endl << "What column did opponent select to play: ";
				getline(cin, input);
				try
				{
					update = stoi(input);
					break;
				}
				catch (exception e)
				{
					cout << endl << "Input could not be translated to int. Please try again" << endl;
				}
			}
			s->updateCurrentState(update, 'O');
			//s->returnBoard();
		}
		else
		{
			cout << "Invalid response, please try again" << endl;
		}
	}


	//negamax game code
	while (!isGameOver(s))
	{
		cout << "Current board state" << endl;
		s->returnBoard();

		//start timer
		auto start = chrono::high_resolution_clock::now();

		int order[7] = { 3,2,4,1,5,0,6 };
		int bestColumn = 3;
		int maxMove = -100000;

		//check all branches and choose the best column based on the best available move
		for (int i = 0; i < 7; i++)
		{
			State* child = new State(s->game);
			if (s->isColumnAvailable(order[i]))
			{
				//update the branch to utilize our move and use the negamax to find potential moves
				child->updateCurrentState(order[i], 'X');
				int currentCol = negaMax(child, 1, -100000, 100000, true);
				child->~State();


				//cout << "Column " << order[i] << " finds " << currentCol << endl;

				//if the potential move is better than the current move, update the best column and max available value
				if (currentCol > maxMove)
				{
					maxMove = currentCol;
					bestColumn = order[i];
				}
			}
		}



		//check for win cases and loss cases
		int win = checkForWin(s);
		int prevent = preventOpponentWin(s, bestColumn);

		if (win < 7)
			bestColumn = win;
		else if (prevent < 7)
			bestColumn = prevent;


		//show the time it took to make a decision and show the decision
		auto finish = chrono::high_resolution_clock::now();
		chrono::duration<double> elapsed = finish - start;
		cout << "Elapsed time: " << elapsed.count() << " s " << endl;
		cout << endl << "NegaMax AI chooses to place piece in Column " << bestColumn << endl;
		s->updateCurrentState(bestColumn, 'X');
		s->returnBoard();

		if (isGameOver(s))
			break;
		else
		{
			string input;
			int update;
			bool valid = false;
			while (!valid)
			{
				cout << endl << "What column did opponent select to play: ";
				getline(cin, input);
				try
				{
					update = stoi(input);
					break;
				}
				catch (exception e)
				{
					cout << endl << "Input could not be translated to int. Please try again" << endl;
				}
			}
			s->updateCurrentState(update, 'O');
			s->returnBoard();
		}

	}
}


int Game::preventOpponentWin(State* s, int currentBest)
{
	State* s0 = new State(s->game); int v0;	//edge column, 4th priority
	State* s1 = new State(s->game);	int v1;	//3rd priority
	State* s2 = new State(s->game);	int v2;	//2nd priority
	State* s3 = new State(s->game);	int v3;	//center column, 1st priority
	State* s4 = new State(s->game);	int v4;	//2nd priority
	State* s5 = new State(s->game);	int v5;	//3rd priority
	State* s6 = new State(s->game);	int v6;	//edge column, 4th priority
	State* nextMove = new State(s->game); int gotNext; //next column selected using min max
	vector<int> conditions;

	
	//add predicted move to the board and see what would happen when the opponent makes their next move
	if (s->isColumnAvailable(currentBest))
	{
		nextMove->updateCurrentState(currentBest, 'X');
		if (nextMove->isColumnAvailable(0))
			s0->updateCurrentState(0, 'O');
		if (nextMove->isColumnAvailable(1))
			s1->updateCurrentState(1, 'O');
		if (nextMove->isColumnAvailable(2))
			s2->updateCurrentState(2, 'O');
		if (nextMove->isColumnAvailable(3))
			s3->updateCurrentState(3, 'O');
		if (nextMove->isColumnAvailable(4))
			s4->updateCurrentState(4, 'O');
		if (nextMove->isColumnAvailable(5))
			s5->updateCurrentState(5, 'O');
		if (nextMove->isColumnAvailable(6))
			s6->updateCurrentState(6, 'O');
	}

	/*
	if (s->isColumnAvailable(0))
			s0->updateCurrentState(0, 'O');
	if (s->isColumnAvailable(1))
		s1->updateCurrentState(1, 'O');
	if (s->isColumnAvailable(2))
		s2->updateCurrentState(2, 'O');
	if (s->isColumnAvailable(3))
		s3->updateCurrentState(3, 'O');
	if (s->isColumnAvailable(4))
		s4->updateCurrentState(4, 'O');
	if (s->isColumnAvailable(5))
		s5->updateCurrentState(5, 'O');
	if (s->isColumnAvailable(6))
		s6->updateCurrentState(6, 'O'); 

	int current = evalOpponent(s);
	int evalS0 = evalOpponent(s0);
	int evalS1 = evalOpponent(s1);
	int evalS2 = evalOpponent(s2);
	int evalS3 = evalOpponent(s3);
	int evalS4 = evalOpponent(s4);
	int evalS5 = evalOpponent(s5);
	int evalS6 = evalOpponent(s6);
	
	if (current != -50 && evalS0 == -50)
		return 0;
	if (current == -50 && evalS0 < -50)
		return 0;
	else if (current != -50 && evalS1 == -50)
		return 1;
	else if (current == -50 && evalS1 < -50)
		return 1;
	else if (current != -50 && evalS2 == -50)
		return 2;
	else if (current == -50 && evalS2 < -50)
		return 2;
	else if (current != -50 && evalS3 == -50)
		return 3;
	else if (current == -50 && evalS3 < -50)
		return 3;
	else if (current != -50 && evalS4 == -50)
		return 4;
	else if (current == -50 && evalS4 == -100)
		return 4;
	else if (current != -50 && evalS5 == -50)
		return 5;
	else if (current == -50 && evalS5 < -50)
		return 5;
	else if (current != -50 && evalS6 == -50)
		return 6;
	else if (current == -50 && evalS6 < -50)
		return 6;
	else
		return 7;*/


	int current = evaluation(s);
	int evalS0 = evaluation(s0);
	int evalS1 = evaluation(s1);
	int evalS2 = evaluation(s2);
	int evalS3 = evaluation(s3);
	int evalS4 = evaluation(s4);
	int evalS5 = evaluation(s5);
	int evalS6 = evaluation(s6);

	if (evalS0 == -1000)
		return 0;
	else if (evalS1 == -1000)
		return 1;
	else if (evalS2 == -1000)
		return 2;
	else if (evalS3 == -1000)
		return 3;
	else if (evalS4 == -1000)
		return 4;
	else if (evalS5 == -1000)
		return 5;
	else if (evalS6 == -1000)
		return 6;
	else
		return 7;


	s0->~State(); s1->~State(); s2->~State(); s3->~State(); s4->~State(); s5->~State(); s6->~State(); nextMove->~State();
}


int Game::checkForWin(State* s)
{
	State* s0 = new State(s->game); int eval0 = 0;	//edge column, 4th priority
	State* s1 = new State(s->game);	int eval1 = 0;	//3rd priority
	State* s2 = new State(s->game);	int eval2 = 0;	//2nd priority
	State* s3 = new State(s->game);	int eval3 = 0;	//center column, 1st priority
	State* s4 = new State(s->game);	int eval4 = 0;	//2nd priority
	State* s5 = new State(s->game);	int eval5 = 0;	//3rd priority
	State* s6 = new State(s->game);	int eval6 = 0;	//edge column, 4th priority
	
	//add predicted move to the board and see what would happen when the opponent makes their next move
	if (s->isColumnAvailable(0))
		s0->updateCurrentState(0, 'X');
	if (s->isColumnAvailable(1))
		s1->updateCurrentState(1, 'X');
	if (s->isColumnAvailable(2))
		s2->updateCurrentState(2, 'X');
	if (s->isColumnAvailable(3))
		s3->updateCurrentState(3, 'X');
	if (s->isColumnAvailable(4))
		s4->updateCurrentState(4, 'X');
	if (s->isColumnAvailable(5))
		s5->updateCurrentState(5, 'X');
	if (s->isColumnAvailable(6))
		s6->updateCurrentState(6, 'X');
	
	int y = 0;
	while (y < 6)
	{
		int x = 0;
		while (x < 7)
		{
			if (countHorizontalGroups(s0, x, y, 'X') >= 4 || countVerticalGroups(s0, x, y, 'X') >= 4
				|| countDiagonalGroups(s0, x, y, 'X') >= 4 || countOtherDiagonalGroups(s0, x, y, 'X') >= 4)
			{
				return 0;
			}


			if (countHorizontalGroups(s1, x, y, 'X') >= 4 || countVerticalGroups(s1, x, y, 'X') >= 4
				|| countDiagonalGroups(s1, x, y, 'X') >= 4 || countOtherDiagonalGroups(s1, x, y, 'X') >= 4)
			{
				return 1;
			}


			if (countHorizontalGroups(s2, x, y, 'X') >= 4 || countVerticalGroups(s2, x, y, 'X') >= 4
				|| countDiagonalGroups(s2, x, y, 'X') >= 4 || countOtherDiagonalGroups(s2, x, y, 'X') >= 4)
			{
				return 2;
			}



			if (countHorizontalGroups(s3, x, y, 'X') >= 4 || countVerticalGroups(s3, x, y, 'X') >= 4
				|| countDiagonalGroups(s3, x, y, 'X') >= 4 || countOtherDiagonalGroups(s3, x, y, 'X') >= 4)
			{
				return 3;
			}


			if (countHorizontalGroups(s4, x, y, 'X') >= 4 || countVerticalGroups(s4, x, y, 'X') >= 4
				|| countDiagonalGroups(s4, x, y, 'X') >= 4 || countOtherDiagonalGroups(s4, x, y, 'X') >= 4)
			{
				return 4;
			}


			if (countHorizontalGroups(s5, x, y, 'X') >= 4 || countVerticalGroups(s5, x, y, 'X') >= 4
				|| countDiagonalGroups(s5, x, y, 'X') >= 4 || countOtherDiagonalGroups(s5, x, y, 'X') >= 4)
			{
				return 5;
			}


			if (countHorizontalGroups(s6, x, y, 'X') >= 4 || countVerticalGroups(s6, x, y, 'X') >= 4
				|| countDiagonalGroups(s6, x, y, 'X') >= 4 || countOtherDiagonalGroups(s6, x, y, 'X') >= 4)
			{
				return 6;
			}


			
			x++;
		}
		y++;
	}
	
	return 7;


	s0->~State(); s1->~State(); s2->~State(); s3->~State(); s4->~State(); s5->~State(); s6->~State(); 
}


int Game::negaMax(State* s, int depth, int alpha, int beta, bool isMax)
{
	//check to see if max depth has been reached
	if (depth == MAX_DEPTH)
		return evaluation(s);

	//check to see if all columns are full
	else if (!s->isColumnAvailable(0) && !s->isColumnAvailable(1) && !s->isColumnAvailable(2)
		&& !s->isColumnAvailable(3) && !s->isColumnAvailable(4) && !s->isColumnAvailable(5) && !s->isColumnAvailable(6))
		return evaluation(s);

	//maximzer (user)
	else if (isMax)
	{
		int maxEvaluation = -10000;
		int order[7] = { 3,2,4,1,5,0,6 };
		for (int i = 0; i < 7; i++)
		{
			State* child = new State(s->game);
			if (s->isColumnAvailable(i))
				child->updateCurrentState(i, 'X');

			//evaluate looking for the max value
			int eval = negaMax(child, depth + 1, alpha, beta, false);
			child->~State();

			//check to see if current branch contains largest
			if (eval > maxEvaluation)
				maxEvaluation = eval;

			if (eval > alpha)
				alpha = eval;
		}

		return maxEvaluation;
	}


	//minimizer (opponent)
	else if (!isMax)
	{
		int minEvaluation = 10000;
		int order[7] = { 3,2,4,1,5,0,6 };
		for (int i = 0; i < 7; i++)
		{
			State* child = new State(s->game);
			if (s->isColumnAvailable(i))
				child->updateCurrentState(i, 'O');

			//evaluate looking for the max value
			int eval = negaMax(child, depth + 1, alpha, beta, true);
			child->~State();
			if (eval < minEvaluation)
				minEvaluation = eval;

			if (eval < alpha)
				alpha = eval;
		}

		if (beta <= alpha)
			return evaluation(s);

		return minEvaluation;
	}
}


//add a check to see if current = highest, which one plays better defense
//also consider redoing the group counting, so that if the pieces are surrounded, the value goes back to 0
void Game::minMax(State* s)
{
	while(!isGameOver(s))
	{
		//start timer
		auto start = chrono::high_resolution_clock::now();

			
		vector <int> possibleMoves;
		int highest = -1000;
		int nextMove = 0;

		int order[7] = { 3,2,4,1,5,0,6 };
		vector<thread> threads;


		for (int i = 0; i < 7; i++)
		{
			checkBranches(s, nextMove, highest, order[i]);
		}

		//thread t1(Game::checkBranches, s, ref(nextMove), ref(highest), order[i]);

		//end timer and display my move
		auto finish = chrono::high_resolution_clock::now();
		
		int win = checkForWin(s);
		
		if(win < 7)
		{
			cout << "Player 1 chooses to drop piece in Column  " << win << endl;
			s->updateCurrentState(win, 'X');
			s->returnBoard();
		}

		else
		{
			int prevent = preventOpponentWin(s, nextMove);
			if (prevent < 7)
			{
				cout << "Player 1 chooses to drop piece in Column  " << prevent << endl;
				s->updateCurrentState(prevent, 'X');
				s->returnBoard();
			}

			else if (isGameOver(s))
			{
				cout << endl << "No move to make, game over" << endl;
			}

			else
			{
				cout << endl << "Player 1 chooses to drop piece in Column " << nextMove << endl;
				s->updateCurrentState(nextMove, 'X');
				s->returnBoard();
			}
		}

		//show time to find move		
		chrono::duration<double> elapsed = finish - start;


		/*if (elapsed.count() < 10)
			MAX_DEPTH = 9;*/
		cout << "Elapsed time: " << elapsed.count() << " s " << endl;


		//check to see if my move ends the game
		//wait for opponent to make move
		if (isGameOver(s))
		{
			cout << "GAME OVER" << endl;
			break;
		}
		else
		{
			string input;
			int update;
			bool valid = false;
			while (!valid)
			{
				cout << endl << "What column did opponent select to play: ";
				getline(cin, input);
				try
				{
					update = stoi(input);
					break;
				}
				catch (exception e)
				{
					cout << endl << "Input could not be translated to int. Please try again" << endl;
				}
			}
			s->updateCurrentState(update, 'O');
			s->returnBoard();
		}
	}
}


void Game::checkBranches(State* s, int &currentBestColumn, int &currentHighestEval, int potentialColumn)
{
	State* sPrime = new State(s->game);
	if (s->isColumnAvailable(potentialColumn))
	{
		sPrime->updateCurrentState(potentialColumn, 'X');
		int potentialHeuristic = max(sPrime, 1);
		cout << "Column " << potentialColumn << " finds " << potentialHeuristic << endl;

		if (potentialHeuristic > currentHighestEval)
		{
			currentBestColumn = potentialColumn;
			currentHighestEval = potentialHeuristic;
		}
	}
	sPrime->~State();
}


int Game::compareTiedEvals(int bestNext, int current, State* s)
{
	if (s->isColumnAvailable(bestNext) && s->isColumnAvailable(current))
	{
		State* currentBest = new State(s->game);
		State* possibleBetter = new State(s->game);
		currentBest->updateCurrentState(bestNext, 'O');
		possibleBetter->updateCurrentState(current, 'O');

		if (evaluation(possibleBetter) > evaluation(currentBest))
			return current;
		else
			return bestNext;

		currentBest->~State();
		possibleBetter->~State();
	}
	else
		return bestNext;
}


int Game::evaluation(State* s)
{
	//opponent score meaures
	int opponentGroupFour = 0;	//automatic win for opponent, if this case is found, exit and return this number
	int	opponentGroupThree = 0;	//count groups of three	for opp		(medium weight)
	int opponentGroupTwo = 0;	//count groups of two for opp		(lowest weight)
	
							
	//AI user score measures
	int groupCountFour = 0;		//count groups of four				(highest weight)
	int groupCountThree = 0;	//count groups of three				(medium weight)
	int groupCountTwo = 0;		//count groups of two				(lowest weight)
	
	
	//increment through the game board counting all groups
	int y = 0;
	while (y < 6)
	{
		int x = 0;
		while (x < 7)
		{
			
			
			//check to see if the opponent has won, if they have then end the loop and return -10000
			if (countHorizontalGroups(s, x, y, 'O') >= 4 || countVerticalGroups(s, x, y, 'O') >= 4
				|| countDiagonalGroups(s, x, y, 'O') >= 4 || countOtherDiagonalGroups(s, x, y, 'O') >= 4)
			{
				return -1000;
				opponentGroupFour++;
				break;
			}

			//also check to see how many groups of 2 and 3 both teams will have
			if (countHorizontalGroups(s, x, y, 'X') >= 4 || countVerticalGroups(s, x, y, 'X') >= 4
				|| countDiagonalGroups(s, x, y, 'X') >= 4 || countOtherDiagonalGroups(s, x, y, 'X') >= 4)
			{
				groupCountFour++;
			}

			if (countHorizontalGroups(s, x, y, 'O') == 3 || countVerticalGroups(s, x, y, 'O') == 3
				|| countDiagonalGroups(s, x, y, 'O') == 3 || countOtherDiagonalGroups(s, x, y, 'O') == 3)
			{
				opponentGroupThree++;
			}

			if (countHorizontalGroups(s, x, y, 'X') == 3 || countVerticalGroups(s, x, y, 'X') == 3
				|| countDiagonalGroups(s, x, y, 'X') == 3 || countOtherDiagonalGroups(s, x, y, 'X') == 3)
			{
				groupCountThree++;
			}

			if (countHorizontalGroups(s, x, y, 'O') == 2 || countVerticalGroups(s, x, y, 'O') == 2
				|| countDiagonalGroups(s, x, y, 'O') == 2 || countOtherDiagonalGroups(s, x, y, 'O') == 2)
			{
				opponentGroupTwo++;
			}

			if (countHorizontalGroups(s, x, y, 'X') == 2 || countVerticalGroups(s, x, y, 'X') == 2
				|| countDiagonalGroups(s, x, y, 'X') == 2 || countOtherDiagonalGroups(s, x, y, 'X') == 2)
			{
				groupCountTwo++;
			}

			x++;
		}
		y++;
	}
	
	
	//if the opponent gets four return worst case number
	if (opponentGroupFour > 0)
	{
		return -1000;
	}

	else
	{
		int opponentStateScore = (opponentGroupThree * 10) + opponentGroupTwo;
		int personalStateScore = (groupCountFour * 100) + (groupCountThree * 10) + groupCountTwo;

		return personalStateScore - opponentStateScore;
	}
}


int Game::min(State* s, int depth)
{
	//if state s is end goal (board full, a win, or a loss)
		//return value
	if (isGameOver(s))
	{
		//return heuristicEval(s);
		return evaluation(s);		
	}


	//if depth is max depth
		//return eval(s)
	else if (depth == MAX_DEPTH)
	{
		//return heuristicEval(s); 
		return evaluation(s);
	}

	else if (!s->isColumnAvailable(0) && !s->isColumnAvailable(1) && !s->isColumnAvailable(2)
		&& !s->isColumnAvailable(3) && !s->isColumnAvailable(4) && !s->isColumnAvailable(5) && !s->isColumnAvailable(6))
		return evaluation(s);


	//else
		//for each possible move, create new states with that move
			//s1 = s + m, or new state = existing state with new move
			//max move = max(s1, d+1)
		//return worst value (minimum)
	else
	{
		State* s0 = new State(s->game); 	int v0;	//edge column, 4th priority
		State* s1 = new State(s->game); 	int v1;	//3rd priority
		State* s2 = new State(s->game); 	int v2;	//2nd priority
		State* s3 = new State(s->game); 	int v3;	//center column, 1st priority
		State* s4 = new State(s->game); 	int v4;	//2nd priority
		State* s5 = new State(s->game); 	int v5;	//3rd priority
		State* s6 = new State(s->game); 	int v6;	//edge column, 4th priority
		vector <int> possibleMoves;


		/*int lowest = 100;
		int nextColumn = 0;

		int order[7] = { 3,2,4,1,5,0,6 };
		for (int i = 0; i < 7; i++)
		{
			checkMinBranches(s, nextColumn, lowest, order[i], depth);
		}*/
		

		//try to add piece to the middle column, and work outward
		if (s->isColumnAvailable(3))
		{
			//s3->updateCurrentState(3, 'X');
			s3->updateCurrentState(3, 'O');
			v3 = max(s3, depth + 1);
			possibleMoves.push_back(v3);
		}

		//2nd priority columns
		if (s->isColumnAvailable(2))
		{
			//s2->updateCurrentState(2, 'X');
			s2->updateCurrentState(2, 'O');
			v2 = max(s2, depth + 1);
			possibleMoves.push_back(v2);
		}
		if (s->isColumnAvailable(4))
		{
			//s4->updateCurrentState(4, 'X');
			s4->updateCurrentState(4, 'O');
			v4 = max(s4, depth + 1);
			possibleMoves.push_back(v4);
		}

		//3rd priority columns
		if (s->isColumnAvailable(1))
		{
			//s1->updateCurrentState(1, 'X');
			s1->updateCurrentState(1, 'O');
			v1 = max(s1, depth + 1);
			possibleMoves.push_back(v1);
		}
		if (s->isColumnAvailable(5))
		{
			//s5->updateCurrentState(5, 'X');
			s5->updateCurrentState(5, 'O');
			v5 = max(s5, depth + 1);
			possibleMoves.push_back(v5);
		}


		//4th priority columns
		if (s->isColumnAvailable(0))
		{
			//s0->updateCurrentState(0, 'X');
			s0->updateCurrentState(0, 'O');
			v0 = max(s0, depth + 1);
			possibleMoves.push_back(v0);
		}
		if (s->isColumnAvailable(6))
		{
			//s6->updateCurrentState(6, 'X');
			s6->updateCurrentState(6, 'O');
			v6 = max(s6, depth + 1);
			possibleMoves.push_back(v6);
		}

		//clear memory
		s0->~State();
		s1->~State();
		s2->~State();
		s3->~State();
		s4->~State();
		s5->~State();
		s6->~State();
		return returnMinValue(possibleMoves);
		//return lowest;
	}

	return 0;
}


int Game::max(State* s, int depth)
{
	//if state s is end goal (board full, a win, or a loss
		//return value
	if (isGameOver(s))
	{
		//return heuristicEval(s); 
		return evaluation(s);
	}

	
	//if depth is max depth
		//return eval(s)
	else if (depth == MAX_DEPTH)
	{
		//return heuristicEval(s); 
		return evaluation(s);
	}

	else if (!s->isColumnAvailable(0) && !s->isColumnAvailable(1) && !s->isColumnAvailable(2)
		&& !s->isColumnAvailable(3) && !s->isColumnAvailable(4) && !s->isColumnAvailable(5) && !s->isColumnAvailable(6))
		return evaluation(s);

	//else
		//for each possible move, create new states with that move
			//s1 = s + m, or new state = existing state with new move
			//min move = min(s1, d+1)
		//return best value (maximum)
	else
	{
		State* s0 = new State(s->game);		int v0;	//edge column, 4th priority
		State* s1 = new State(s->game);		int v1;	//3rd priority
		State* s2 = new State(s->game); 	int v2;	//2nd priority
		State* s3 = new State(s->game); 	int v3;	//center column, 1st priority
		State* s4 = new State(s->game);		int v4;	//2nd priority
		State* s5 = new State(s->game);		int v5;	//3rd priority
		State* s6 = new State(s->game); 	int v6;	//edge column, 4th priority
		vector <int> possibleMoves;
		/*int highest = -100;
		int nextColumn = 0;
		
		int order[7] = { 3,2,4,1,5,0,6 };
		for (int i = 0; i < 7; i++)
		{
			checkMaxBranches(s, nextColumn, highest, order[i], depth);
		}*/
		

		
		//try to add piece to the middle column, and work outward
		if (s->isColumnAvailable(3))
		{
			s3->updateCurrentState(3, 'X');
			v3 = min(s3, depth + 1);
			possibleMoves.push_back(v3);
		}

		//2nd priority columns
		if (s->isColumnAvailable(2))
		{
			s2->updateCurrentState(2, 'X');
			v2 = min(s2, depth + 1);
			possibleMoves.push_back(v2);
		}
		if (s->isColumnAvailable(4))
		{
			s4->updateCurrentState(4, 'X');
			v4 = min(s4, depth + 1);
			possibleMoves.push_back(v4);
		}
		
		//3rd priority columns
		if (s->isColumnAvailable(1))
		{
			s1->updateCurrentState(1, 'X');
			v1 = min(s1, depth + 1);
			possibleMoves.push_back(v1);
		}
		if (s->isColumnAvailable(5))
		{
			s5->updateCurrentState(5, 'X');
			v5 = min(s5, depth + 1);
			possibleMoves.push_back(v5);
		}


		//4th priority columns
		if (s->isColumnAvailable(0))
		{
			s0->updateCurrentState(0, 'X');
			v0 = min(s0, depth + 1);
			possibleMoves.push_back(v0);
		}
		if (s->isColumnAvailable(6))
		{
			s6->updateCurrentState(6, 'X');
			v6 = min(s6, depth + 1);
			possibleMoves.push_back(v6);
		}

		//clear memory
		s0->~State();
		s1->~State();
		s2->~State();
		s3->~State();
		s4->~State();
		s5->~State();
		s6->~State();

		return returnMaxValue(possibleMoves);
		//return highest;
	}
}


int Game::returnMaxValue(vector<int> &moves)
{
	int max = moves.at(0);
	for (int i = 0; i < moves.size(); i++)
	{
		if (moves.at(i) > max)
			max = moves.at(i);
	}
	
	//cout << "max value returned" << endl; 
	return max;
}


int Game::returnMinValue(vector<int> &moves)
{
	int min = moves.front();
	for (int i = 0; i < moves.size(); i++)
	{
		if (moves.at(i) < min)
			min = moves.at(i);
	}
	
	//cout << "min value returned" << endl;
	return min;
}


void Game::manualUpdate()
{	
	State* board = gameStartState; 
	while (!isGameOver(board))
	{
		bool validColumn = false;
		int response;
		while (!validColumn)
		{
			cout << endl << endl << "Player 1's Turn" << endl
				<< "Please select which column to place your piece" << endl;

			board->returnBoard();

			string temp;
			cout << "Select: ";
			cin >> temp;

			if (board->isColumnAvailable(stoi(temp)) && stoi(temp) < 7 && stoi(temp) >= 0)
			{
				response = stoi(temp);
				validColumn = true;
			}
			else
				cout << endl << "Column is already full or does not exist, please try again" << endl;
		}

		board->updateCurrentState(response, 'X');
		board->returnBoard();


		if (isGameOver(board))
		{
			break;
		}

		//show groups for player one
		cout << "Existing lines over 2 for Player 1" << endl;
		showExistingGroups(board, 'X', 2);

		bool validColumnPlayerTwo = false;
		int responseTwo;
		while (!validColumnPlayerTwo)
		{
			cout << endl << endl << "Player 2's Turn" << endl
				<< "Please select which column to place your piece" << endl;

			board->returnBoard();

			string temp;
			cout << "Select: ";
			cin >> temp;
			if (board->isColumnAvailable(stoi(temp)) && stoi(temp) < 7 && stoi(temp) >= 0)
			{
				responseTwo = stoi(temp);
				validColumnPlayerTwo = true;
			}
			else
				cout << endl << "Column is already full or does not exist, please try again" << endl;
		}
		board->updateCurrentState(responseTwo, 'O');
		board->returnBoard();


		//show groups for playert two
		cout << "Existing lines over 2 for Player 2" << endl;
		showExistingGroups(board, 'O', 2);
	}
}


void Game::showExistingGroups(State* board, char type, int size)
{
	//shows all existing groups of 2
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 7; x++)
		{
			int hGroupCounter = countHorizontalGroups(board, x, y, type);
			int vGroupCounter = countVerticalGroups(board, x, y, type);
			int diagonalCounter = countDiagonalGroups(board, x, y, type);
			int ddCounter = countOtherDiagonalGroups(board, x, y, type);
			if (hGroupCounter > size)

				cout << "Group of " << hGroupCounter << " horizontally including (" << x << "," << y << ")" << endl;

			if (vGroupCounter > size)

				cout << "Group of " << vGroupCounter << " vertically including (" << x << "," << y << ")" << endl;

			if (diagonalCounter > size)

				cout << "Group of " << diagonalCounter << " diagonally including (" << x << "," << y << ")" << endl;

			if (ddCounter > size)

				cout << "Group of " << ddCounter << " diagonally including (" << x << "," << y << ")" << endl;

		}
	}
}


int Game::countHorizontalGroups(State* current, int column, int row, char type)
{
	int count = 0;
	char opponent;

	if (type == 'X')
		opponent = 'O';
	else
		opponent = 'X';
	
	//check to see if the current spot belongs to the player being examined
	char currentChar = current->game->getAt(column, row);
		if (currentChar == type)
			count++;
		else
			return 0;


	//check to see how many of the same type exist to the left
	int checkLeft = column - 1;
	while (checkLeft >= 0)
	{
		char west = current->game->getAt(checkLeft, row);
		if (west == type)
		{
			
			count++;
			checkLeft--;
		}
		else
			break;
	}


	//check to see how many of the same type exist to the left
	int checkRight = column + 1;
	while (checkRight < COLUMNS)
	{
		char east = current->game->getAt(checkRight, row);
		if (east == type)
		{
			count++;
			checkRight++;
		}
		else
			break;
	}

	return count;
}


int Game::countVerticalGroups(State* current, int column, int row, char type)
{
	int count = 0;
	char opponent;

	if (type == 'X')
		opponent = 'O';
	else
		opponent = 'X';


	//check to see if the current spot belongs to the player being examined
	char currentChar = current->game->getAt(column, row);
	if (currentChar == type)
		count++;
	else
		return 0;


	//check to see how many of the same type exists above
	int checkUp = row - 1;
	while (checkUp >= 0)
	{
		char north = current->game->getAt(column, checkUp);
		if (north == type)
		{
			count++;
			checkUp--;
		}
		else
			break;
	}


	//check to see how many of the same type exists below
	int checkDown = row + 1;
	while (checkDown < ROWS)
	{
		char south = current->game->getAt(column, checkDown);
		if (south == type)
		{
			count++;
			checkDown++;
		}
		else
			break;
	}

	return count;
}


int Game::countDiagonalGroups(State* current, int column, int row, char type)
{
	int count = 0;

	//check to see if the current spot belongs to the player being examined
	char currentChar = current->game->getAt(column, row);
	if (currentChar == type)
		count++;
	else
		return 0;


	//check to see how many of the same type exist to the upper left
	int checkLeft = column - 1;
	int checkUp = row - 1;
	while (checkLeft >= 0 && checkUp >= 0)
	{
		char northWest = current->game->getAt(checkLeft, checkUp);
		if (northWest == type)
		{
			count++;
			checkLeft--;
			checkUp--;
		}
		else
			break;
	}


	//check to see how many of the same type exist to the left
	int checkRight = column + 1;
	int checkDown = row + 1;
	while (checkRight < COLUMNS && checkDown < ROWS)
	{
		char southEast = current->game->getAt(checkRight, checkDown);
		if (southEast == type)
		{
			count++;
			checkRight++;
			checkDown++;
		}
		else
			break;
	}

	return count;
}


int Game::countOtherDiagonalGroups(State* current, int column, int row, char type)
{
	int count = 0;

	//check to see if the current spot belongs to the player being examined
	char currentChar = current->game->getAt(column, row);
	if (currentChar == type)
		count++;
	else
		return 0;


	//check to see how many of the same type exist to the upper left
	int checkLeft = column - 1;
	int checkDown = row + 1;
	while (checkLeft >= 0 && checkDown < ROWS)
	{
		char southWest = current->game->getAt(checkLeft, checkDown);
		if (southWest == type)
		{
			count++;
			checkLeft--;
			checkDown++;
		}
		else
			break;
	}


	//check to see how many of the same type exist to the left
	int checkRight = column + 1;
	int checkUp = row - 1;
	while (checkRight < COLUMNS && checkUp >= 0)
	{
		char northEast = current->game->getAt(checkRight, checkUp);
		if (northEast == type)
		{
			count++;
			checkRight++;
			checkUp--;
		}
		else
			break;
	}

	return count;
}