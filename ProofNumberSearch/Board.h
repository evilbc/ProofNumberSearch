#pragma once

#include "Player.h"
#include "LinkedMoveList.h"

class Player;
struct Move;

class Board {
public:
	Board(int width, int height);
	Board(const Board& other);
	~Board();
	const Player& getPlayer(int x, int y) const;
	void setPlayer(int x, int y, Player player);
	int getWidth() const;
	int getHeight() const;
	bool withinBounds(int x, int y) const;
	bool isFull() const;
	void makeAMove(Move& move);
	void undoMove(Move& move);

	void read();
	void write() const;
private:
	Player** board;
	int width;
	int height;
	int numOfEmptyFields;
};
