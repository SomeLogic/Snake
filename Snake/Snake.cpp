#include <iostream>
#include <vector>
#include <conio.h>
#include <chrono>
#include <thread>
#include <memory>
#include <random>
#include <fstream>
#include <Windows.h>

#define row_size 20
#define column_size 15

using matrix = std::vector<std::vector<char>>;
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

struct Apple {
	int x, y;
};

struct Coord {
	int x, y;

	Coord(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

struct Snake {
	int x, y;
	std::unique_ptr<Snake> next;

	Snake() {
		x = 0;
		y = 0;
		next = nullptr;
	}

	Snake(int x, int y) {
		this->x = x;
		this->y = y;
		next = nullptr;
	}
};

int startGame(const int difficulty);
void printMenu(int &difficulty, int highscore, int& failCounter);
void chooseDifficulty(int &difficulty);
void showControls();
matrix defaultVector();
matrix updateGameWindowVector(std::unique_ptr<Snake> &snake, Apple &apple, matrix &gameWindow, char &c, int &score);
void printGameWindow(const matrix &gameWindow, const int &score);
void addToSnake(std::unique_ptr<Snake> &snake, int x, int y);
void deleteFromSnake(std::unique_ptr<Snake> &snake, matrix &gameWindow);
void insertSnake(std::unique_ptr<Snake> &snake, matrix &gameWindow);
int randomIntGenerator(int min, int max);
Apple genApplePos(Apple &apple);
bool isAppleCollected(const std::unique_ptr<Snake> &snake, const Apple &apple);
bool isPartOfSnake(const std::unique_ptr<Snake> &snake, const Apple &apple);
bool isLose(std::unique_ptr<Snake> &snake, std::vector<Coord> &coord);
void gotoxy(int x, int y);
void addMoveToSnake(std::unique_ptr<Snake> &snake, int x, int y, char c);
void setcursor(bool visible, DWORD size);
void clearScreen();
void pause();
void saveHighscore(int& highscore, int& failCounter);
void readHighscore(int& highscore, int& failCounter);

int main() {
	int highscore = 0, failCounter = 0;
	readHighscore(highscore, failCounter);

	setcursor(0, 0);//(1, 10) - normal cursor, (0, 0) - invisible cursor
	int difficulty = 125;
	printMenu(difficulty, highscore, failCounter);
}

void pause() {
	std::cin.ignore();
}

void printMenu(int &difficulty, int highscore, int& failCounter) {
	clearScreen();
	gotoxy(0, 0);
	std::cout << "Press right key to choose option:\n"
		<< "1. Choose difficulty level (currently - ";
	switch (difficulty) {
	case 170: std::cout << "easy"; break;
	case 125: std::cout << "normal"; break;
	case 80: std::cout << "hard"; break;
	}
	std::cout << ").\n"
		<< "2. Show controls.\n"
		<< "3. Run game.\n"
		<< "Escape to leave.\n\n"
		<< "Highscore (hard level only): " << highscore
		<< "\nAttempts since last highscore: " << failCounter;

	char m;
	int score;
	switch (m = _getwch()) {
	case '1': {
		chooseDifficulty(difficulty); 
		printMenu(difficulty, highscore, failCounter); break;
	case '2': showControls(); 
		printMenu(difficulty, highscore, failCounter); break;
	case '3':
	case 13: score = startGame(difficulty);
		if (score > highscore && difficulty == 80) {
			highscore = score;
			failCounter = 0;
		}
		else if (score < highscore && difficulty == 80) failCounter++;
		printMenu(difficulty, highscore, failCounter); break;
	case 27: saveHighscore(highscore, failCounter); break;
	default: std::cout << "Wrong option, press any key to try again...";
		_getwch(); 
		printMenu(difficulty, highscore, failCounter); break;
	}
	}
}

void chooseDifficulty(int &difficulty) {
	clearScreen();
	gotoxy(0, 0);
	std::cout << "Choose difficulty level:\n"
		<< "1. - easy\n"
		<< "2. - normal\n"
		<< "3. - hard\n"
		<< "Press escape to return.\n";
	char d = _getwch();
	switch (d) {
	case '1': std::cout << "Difficulty level - easy."; difficulty = 170; break;
	case '2': std::cout << "Difficulty level - normal."; difficulty = 125; break;
	case '3': std::cout << "Difficulty level - hard."; difficulty = 80; break;
	case '27': break;
	default:
		std::cout << "Couldn't recognize difficulty level. Press any key to try again...";
		_getwch();
		chooseDifficulty(difficulty); break;
	}
}

void showControls() {
	clearScreen();
	gotoxy(0, 0);
	std::cout << "W or UP Arrow - move up.\n"
		<< "A or Left Arrow - move left.\n"
		<< "S or Down Arrow - move down.\n"
		<< "D or Right Arrow - move right.\n"
		<< "Escape - leave to main menu.\n\n"
		<< "Press any key to return...";
	_getwch();
}

int startGame(const int difficulty) {
	clearScreen();
	int t = 1;

	while (_kbhit())
		_getche();

	gotoxy(0, 0);
	std::unique_ptr<Snake> snake;
	Apple apple = genApplePos(apple);
	addToSnake(snake, column_size / 2, row_size / 2 - 2);
	addToSnake(snake, column_size / 2, row_size / 2 - 1);
	addToSnake(snake, column_size / 2, row_size / 2);
	addToSnake(snake, column_size / 2, row_size / 2 + 1);
	matrix gameWindow = defaultVector();
	char c = 'd';
	int score = 0;
	updateGameWindowVector(snake, apple, gameWindow, c, score);
	printGameWindow(gameWindow, score);
	int i = 1;
	_getwch();
	std::cout << "\nPress any key to start...";
	
	float diff_time;
	while (c != 27) {
		char tmp = c;
		clock_t start = clock();
		if (_kbhit()) {
			c = _getwch();
		}
		if (c == -32) {
			c = _getwch();
		}

		switch (tmp) {
		case 72: if (c == 80) c = tmp; break;
		case 75: if (c == 77) c = tmp; break;
		case 80: if (c == 72) c = tmp; break;
		case 77: if (c == 75) c = tmp; break;
		}
			
		gotoxy(0, 0);
		gameWindow = updateGameWindowVector(snake, apple, gameWindow, c, score);
		printGameWindow(gameWindow, score);
		std::cout << "\n                                      ";
		gameWindow.at(snake->x).at(snake->y) = ' ';
		std::vector<Coord> coord;
		if (isLose(snake, coord)) {
			std::cout << "\nYou lost! :(\n\n";
			pause();
			return score;
		}
		diff_time = clock() - (float)start;
		while (diff_time < difficulty) diff_time = clock() - (float)start;
	}
	return score;
}

//called once in the beginning of our game, it generates vector which 
//includes map, snake and apple
matrix defaultVector() {
	matrix gameWindow;
	for (int i = 0; i < column_size; i++) { //number of columns
		std::vector<char> row;
		for (int j = 0; j < row_size; j++) { //number of rows
			//inserting border in 1st and last row
			(i == 0 || i == column_size - 1) ? row.push_back('~') :
				//inserting border on 1st and last row, inserting x inside borders
				(j == 0 || j == row_size - 1) ? row.push_back('|') : row.push_back(' ');
		}
		gameWindow.push_back(row);
		row.resize(0);
	}
	return gameWindow;
}

//game needs default vector with game window for later functions #worksFine
matrix updateGameWindowVector(std::unique_ptr<Snake> &snake, Apple &apple, matrix &gameWindow, char &c, int &score) {
	//delete unnecessary tail if apple wasn't collected
	if (!isAppleCollected(snake, apple)) {
		gameWindow.at(snake->x).at(snake->y) = ' ';
		deleteFromSnake(snake, gameWindow);
	}
	else {
		//generate apple until it's on different position than snake
		gameWindow.at(apple.x).at(apple.y) = ' ';
		score++;
		Beep(1500, 50);
		while (isPartOfSnake(snake, apple)) {
			apple = genApplePos(apple);
		}
	}	

	//add new head 
	addMoveToSnake(snake, snake->x, snake->y, c);

	//inserting snake to game window
	insertSnake(snake, gameWindow);

	//insert apple into game window
	gameWindow.at(apple.x).at(apple.y) = '*';

	return gameWindow;
}

//this function is just printing already made and updated vector with game window inside it #worksFine
void printGameWindow(const matrix &gameWindow, const int &score) {
	std::cout << std::endl; //<-for better visual look in console
	std::cout << "   Current score: " << score << std::endl;
	for (int i = 0; i < column_size; i++) {
		std::cout << "   "; //<-for better visual look of window in console
		for (int j = 0; j < row_size; j++) {
			// i - column index, j - row index
			std::cout << gameWindow.at(i).at(j) << " ";
		}
		std::cout << std::endl;
	}
}

//generate random apple position #worksFine
Apple genApplePos(Apple &apple) {
	apple.x = randomIntGenerator(1, 13); //row
	apple.y = randomIntGenerator(1, 18); //column
	return apple;
}

//check if apple is on the same position as snake is #worksFine
bool isAppleCollected(const std::unique_ptr<Snake> &snake, const Apple &apple) {
	if (snake != NULL && snake->next == NULL) {
		if (apple.x == snake->x && apple.y == snake->y) return true;
	}
	//if current snake part is not same as apple position, check next snake part
	else if (isAppleCollected(snake->next, apple) == true) return true;
	return false;
}

bool isPartOfSnake(const std::unique_ptr<Snake> &snake, const Apple &apple) {
	if (snake != NULL) {
		if (apple.x == snake->x && apple.y == snake->y) return true;
	}
	//if current snake part is not same as apple position, check next snake part
	if (snake->next != NULL) 
		if (isPartOfSnake(snake->next, apple) == true) return true;
	return false;
}

//check if snake is on position of game window's border
bool isLose(std::unique_ptr<Snake> &snake, std::vector<Coord> &coord) {
	if (snake != NULL) {
		if(snake->next != NULL) coord.insert(coord.end(), { snake->x, snake->y });
		if (snake->x == 0 || snake->x == 14 || snake->y == 0 || snake->y == 19) return true;
	}
	if (snake != NULL && snake->next == NULL) {
		for (int i = 0; i < coord.size(); i++) {
			if (coord.at(i).x == snake->x && coord.at(i).y == snake->y) return true;
		}
	}
	if (snake != NULL && isLose(snake->next, coord) == true) return true;
	else return false;
}

//add coordinates manually #WorksFine
void addToSnake(std::unique_ptr<Snake> &snake, int x, int y) {
	if (snake == NULL) snake = std::make_unique<Snake>(x, y);
	else addToSnake(snake->next, x, y);
}

//insert into snake new coordinates to make it longer #whatTheFuckIsGoingOnHere
void addMoveToSnake(std::unique_ptr<Snake> &snake, int x, int y, char c) {
	if (snake == NULL) snake = std::make_unique<Snake>(x, y);
	else switch (c) {
	case 'w': 
	case 'W':
	case 72:{
		addMoveToSnake(snake->next, snake->x - 1, snake->y, c); break;
	}
	case 'a':
	case 'A':
	case 75: {
		addMoveToSnake(snake->next, snake->x, snake->y - 1, c); break;
	}
	case 's':
	case 'S':
	case 80:{
		addMoveToSnake(snake->next, snake->x + 1, snake->y, c); break;
	}
	case 'd':
	case 'D':
	case 77: {
		addMoveToSnake(snake->next, snake->x, snake->y + 1, c); break;
	}
	default: break;
	}
}

//delete first element of "snake" which is tail of our snake
void deleteFromSnake(std::unique_ptr<Snake> &snake, matrix &gameWindow) {
	if (snake->next != NULL) {
		snake->x = snake->next->x;
		snake->y = snake->next->y;
		deleteFromSnake(snake->next, gameWindow);
	}
	else {
		snake = NULL;
	}
}

//insert snake into game window #itWorksFine
void insertSnake(std::unique_ptr<Snake> &snake, matrix &gameWindow) {
	if (snake->next != NULL) insertSnake(snake->next, gameWindow);
	if(snake->next == NULL ) gameWindow.at(snake->x).at(snake->y) = 'Q';
	else gameWindow.at(snake->x).at(snake->y) = 'o';
}

//generate random integer between min and max
int randomIntGenerator(int min, int max) {
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> dis(min, max);
	return dis(gen);
}

void gotoxy(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(console, pos);
}

void setcursor(bool visible, DWORD size) // set bool visible = 0 - invisible, bool visible = 1 - visible
{
	if (size == 0)
	{
		size = 20;	// default cursor size Changing to numbers from 1 to 20, decreases cursor width
	}
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = size;
	SetConsoleCursorInfo(console, &lpCursor);
}

void clearScreen() {
	gotoxy(0, 0);
	std::vector<char> clear;
	for (int i = 0; i < 100; i++)
		clear.push_back(' ');

	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < clear.size(); j++)
			std::cout << clear.at(i);
		std::cout << std::endl;
	}
}

void saveHighscore(int& highscore, int& failCounter) {
	std::ofstream output ("C:\\Games\\Highscore.dat" , std::ios::binary);
	output.write(reinterpret_cast<char*> (&highscore), sizeof(&highscore));
	output.write(reinterpret_cast<char*> (&failCounter), sizeof(&failCounter));
	output.close();
}

void readHighscore(int& highscore, int& failCounter) {
	std::ifstream input("C:\\Games\\Highscore.dat", std::ios::binary);
	if (input.good()) {
		input.read(reinterpret_cast<char*> (&highscore), sizeof(&highscore));
		input.read(reinterpret_cast<char*> (&failCounter), sizeof(&failCounter));
	}
	input.close();
}