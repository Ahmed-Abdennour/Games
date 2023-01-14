# This Sudoku does not take user's input, It just generates a random puzzle then solves it
# It uses the recursive backtracking principle
# You can use it if you want, just make sure to give some of credits ;)



#include <iostream>
#include <string>
#include <algorithm>
#include <array>
#include <random>
#include <time.h>
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace std;

#include <Windows.h>

const int PUZZLE_SIZE = 9;
const int EMPTY_VALUE = 0;
array<int , 9> PossibleValues{1, 2, 3, 4, 5, 6, 7, 8, 9 };
int numOfSolutions = 0;


void setCursorPosition(int x, int y)
{
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}

void sleep(int ms)
{
	this_thread::sleep_for(chrono::milliseconds(ms));
}

void printPuzzle(int puzzle[PUZZLE_SIZE][PUZZLE_SIZE], bool clear = true)
{
	if (clear)
	{
		setCursorPosition(0, 0);
	}
	string separator = "", text, padding;
	for (int i = 0; i < PUZZLE_SIZE; i++)
	{
		separator = " ---";
		text = "||";
		padding = "||";
		for (int j = 0; j < PUZZLE_SIZE; j++)
		{
			string caseValue = puzzle[i][j] == EMPTY_VALUE ? " " : to_string(puzzle[i][j]); 
			text += "  " + caseValue + "  |";
			separator += "------";
			padding += "     |";

			if (j % 3 == 2)
			{
				padding += "|";
				text += "|";
			}
		}

		if (i % 3 == 0)
		{
			replace(separator.begin(), separator.end(), '-', '=');
		}
		cout << separator << endl << padding << endl;
		cout << text << endl << padding << endl;
	}
	replace(separator.begin(), separator.end(), '-', '=');
	cout << separator << endl;
}

bool isValid(int puzzle[PUZZLE_SIZE][PUZZLE_SIZE], int row, int col, int caseValue)
{
	// Checking the current row
	for (int i = 0; i < PUZZLE_SIZE; i++)
		if (puzzle[i][col] == caseValue) return false;

	// Checking the current Column
	for (int j = 0; j < PUZZLE_SIZE; j++)
		if (puzzle[row][j] == caseValue) return false;

	// Checking the current 3*3 Box a.k.a Grid
	int startRow = floor(row / 3) * 3, startCol = floor(col / 3) * 3;
	for (int i = startRow; i < startRow + 3; i++)
		for (int j = startCol; j < startCol + 3; j++)
			if (puzzle[i][j] == caseValue) return false;

	// If none the previous 'return' occured then we shall return true by default
	return true;
}

bool EmptyCellRemaining(int puzzle[PUZZLE_SIZE][PUZZLE_SIZE])
{
	for (int i = 0; i < PUZZLE_SIZE; i++)
		for (int j = 0; j < PUZZLE_SIZE; j++)
			if (puzzle[i][j] == EMPTY_VALUE) return true;
	
	// No empty cell in the sudoku
	return false;
}

bool fillPuzzle(int(&puzzle)[PUZZLE_SIZE][PUZZLE_SIZE])
{
	int row, col;
	for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++)
	{
		row = floor(i / PUZZLE_SIZE);
		col = i % PUZZLE_SIZE;

		if (puzzle[row][col] == EMPTY_VALUE)
		{
			unsigned caseSeed = chrono::system_clock::now().time_since_epoch().count();
			shuffle(PossibleValues.begin(), PossibleValues.end(), default_random_engine(caseSeed));

			for (int j = 0; j < PUZZLE_SIZE; j++)
			{
				if (isValid(puzzle, row, col, PossibleValues[j]))
				{
					puzzle[row][col] = PossibleValues[j];

					if (!EmptyCellRemaining(puzzle) || fillPuzzle(puzzle))
						return true;
				}
			}
			break;
		}

	}
	puzzle[row][col] = EMPTY_VALUE;

	return false;
}

bool solveSudoku(int puzzle[PUZZLE_SIZE][PUZZLE_SIZE], bool visualizeAlgorithm = false)
{
	int row, col;
	for (int i = 0; i < PUZZLE_SIZE * PUZZLE_SIZE; i++)
	{
		row = floor(i / PUZZLE_SIZE);
		col = i % PUZZLE_SIZE;

		if (puzzle[row][col] == EMPTY_VALUE)
		{
			for (int caseValue = 1; caseValue <= PUZZLE_SIZE; caseValue++)
			{
				if (isValid(puzzle, row, col, caseValue))
				{
					puzzle[row][col] = caseValue;

					if (visualizeAlgorithm)
					{
						sleep(50);
						printPuzzle(puzzle);
					}

					if (!EmptyCellRemaining(puzzle))
					{
						numOfSolutions++;
						
						if (visualizeAlgorithm)
						{
							sleep(50);
							printPuzzle(puzzle);
							return true;
						}
						break;
					}
					else if (solveSudoku(puzzle, visualizeAlgorithm))
						return true;
				}
			}
			break;
		}
	}
	puzzle[row][col] = EMPTY_VALUE;

	if (visualizeAlgorithm)
	{
		sleep(50);
		printPuzzle(puzzle);
	}

	return false;
}

void generatePuzzle(int(&puzzle)[PUZZLE_SIZE][PUZZLE_SIZE], int difficulty = 5)
{
	for (int i = 0; i < PUZZLE_SIZE; i++)
		for (int j = 0; j < PUZZLE_SIZE; j++)
			puzzle[i][j] = EMPTY_VALUE;

	fillPuzzle(puzzle);

	int solvingAttempt = difficulty;
	srand((unsigned)time(NULL));
	while (solvingAttempt > 0)
	{
		int row = floor(rand() % PUZZLE_SIZE),
			col = floor(rand() % PUZZLE_SIZE);

		while (puzzle[row][col] == EMPTY_VALUE)
		{
			row = floor(rand() % PUZZLE_SIZE);
			col = floor(rand() % PUZZLE_SIZE);
		}

		int savedValue = puzzle[row][col];
		puzzle[row][col] = EMPTY_VALUE;

		numOfSolutions = 0;
		solveSudoku(puzzle);

		if (numOfSolutions != 1)
		{
			puzzle[row][col] = savedValue;
			solvingAttempt--;
		}
	}
}

int main(int, char**)
{
	int puzzle[PUZZLE_SIZE][PUZZLE_SIZE];
	string req, fs;
	system("cls");
	// if your system is linux run this command below
	// system("clear");
	generatePuzzle(puzzle);
	printPuzzle(puzzle, true);
	cout << "Do you wish to see this Sudoku Puzzle get solved with a wonderful algorithm :D ? (y/n)  ";
	cin >> req;
	if (req != "y" && req != "Y")
	{
		cout.flush();
		cout << "Shame on you kid !" << endl;
		cin.get();
	}
	else 
	{
		cout << "Before that make sure you're in full screen mode" << endl;
		cout << "Because otherwise you will be bothered by flushes :(" << endl;
		cout << "You can do that by pressing Alt + Enter   ;D" << endl;
		cout << "Are you in full screen mode now ? (y/n)  : ";
		cin >> fs;
		while (fs != "y" && fs != "Y")
		{
			cout << "Please put full screen mode on :( (y/n)  : ";
			cin >> fs;
		}
		cout << endl << "Now see the beauty of recursive backtracking ! :D" << endl;
		int countdown = 3;
		while (countdown >= -1)
		{
			sleep(1000);
			if (countdown != -1) cout << countdown << endl;
			countdown--;
		}
		system("cls");
		solveSudoku(puzzle, true);
		cout.flush();
		cout << endl << "   PRETTY COOL HUH ?" << endl << endl;
		cin.get();
	}
}
