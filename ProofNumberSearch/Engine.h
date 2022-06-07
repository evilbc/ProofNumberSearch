#pragma once

#include "Board.h"
#include "LinkedMoveList.h"
#include "Player.h"
#include <algorithm>

#define MESSAGE_TIE "BOTH_PLAYERS_TIE\n"
#define MESSAGE_P1 "FIRST_PLAYER_WINS\n"
#define MESSAGE_P2 "SECOND_PLAYER_WINS\n"

#define P1_WIN 1
#define P1_LOSS -1
#define TIE 0

#define INFINTE INT_MAX

class Board;
class LinkedMoveList;
class Player;
struct Move;

class NmkEngine {
	enum class Type {
		AND, OR
	};
	enum class Value {
		DISPROVEN, PROVEN, UNKNOWN, DRAWN
	};
	struct Node {
		explicit Node(Node* parent, Move move, Type type, LinkedMoveList* threats);
		~Node();
		Node* parent;
		Node** children;
		int proof;
		int disproof;
		int childrenCount;
		Type type;
		bool expanded;
		Move moveMade;
		Value value;
		LinkedMoveList* threats;
		Type getOppositeType();
	};
public:
	NmkEngine(Board& board, int k, Player player);
	void solve();
private:
	Board& board;
	int minToWin;
	Player player;
	int proofNumberSearch(Node* root, int maxNodes);
	void evaluate(Node* root);
	void setProofAndDisproofNumbers(Node* node);
	Node* selectMostProvingNode(Node* node);
	void expandNode(Node* node);
	Node* updateAncestors(Node* node, Node* root);
	void generateChildren(Node* node);
	void setNodeValue(Node* node, Player& winningPlayer);
	bool isWinning(const Player& currPlayer) const;
	bool isWinning(int x, int y) const;
	bool isWinning(int startX, int startY, int dx, int dy) const;
	int howManyInDirection(int startX, int startY, int dx, int dy) const;
	void addThreats(Move& currMove, LinkedMoveList& threats) const;
	void addThreats(Move& currMove, LinkedMoveList& threats, int dx, int dy) const;
	Move* howManyInDirectionWithSkip(Move& currMove, int dx, int dy, int& counter, int& skipCounter) const;
	LinkedMoveList* generatePossibleMoves(const Player& currPlayer, LinkedMoveList& threats);
	bool isOver(const Player& currPlayer, int lastX = UNKNOWN_MOVE, int lastY = UNKNOWN_MOVE) const;
	void removeBlockedThreats(Move& currMove, LinkedMoveList& threats) const;
	static bool moveWasWinning(Move& currMove, LinkedMoveList& threats);
	static int resultByPlayer(Player pl);
	void fillThreatsAtStart(LinkedMoveList& threats) const;
	bool detectDraw(Node* node);
};
