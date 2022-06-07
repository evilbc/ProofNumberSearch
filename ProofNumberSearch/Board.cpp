#define _CRT_SECURE_NO_WARNINGS

#include "Board.h"
#include <assert.h>
#include <cstdio>

Board::Board(int width, int height) : width(width), height(height), numOfEmptyFields(height* width) {
	board = new Player * [height];
	for (int i = 0; i < height; i++) {
		board[i] = new Player[width];
	}
}

Board::Board(const Board& other) : width(other.width), height(other.height), numOfEmptyFields(other.numOfEmptyFields) {
	board = new Player * [height];
	for (int i = 0; i < height; i++) {
		board[i] = new Player[width];
		for (int j = 0; j < width; j++) {
			board[i][j] = other.board[i][j];
		}
	}
}

Board::~Board() {
	for (int i = 0; i < height; i++) {
		delete[] board[i];
	}
	delete[] board;
}

bool Board::withinBounds(int x, int y) const {
	return x >= 0 && x < width&& y >= 0 && y < height;
}

const Player& Board::getPlayer(int x, int y) const {
	assert(withinBounds(x, y));
	return board[y][x];
}

void Board::setPlayer(int x, int y, Player player) {
	assert(withinBounds(x, y));
	Player& p = board[y][x];
	if (p == Player::NONE && player != Player::NONE) {
		numOfEmptyFields--;
	} else if (p != Player::NONE && player == Player::NONE) {
		numOfEmptyFields++;
	}
	board[y][x] = player;
}

int Board::getWidth() const {
	return width;
}

int Board::getHeight() const {
	return height;
}

bool Board::isFull() const {
	return numOfEmptyFields <= 0;
}

void Board::read() {
	numOfEmptyFields = 0;
	int input;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			scanf("%d", &input);
			board[i][j] = Player(input);
			if (board[i][j] == Player::NONE) {
				numOfEmptyFields++;
			}
		}
	}
}

void Board::write() const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%d ", board[i][j].valueAsInt());
		}
		printf("\n");
	}
}

void Board::makeAMove(Move& move) {
	setPlayer(move.x, move.y, move.player);
}

void Board::undoMove(Move& move) {
	setPlayer(move.x, move.y, Player::NONE);
}
