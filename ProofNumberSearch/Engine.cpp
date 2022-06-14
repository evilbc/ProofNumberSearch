#include "Engine.h"

NmkEngine::NmkEngine(Board& board, int k, Player player) : board(board), minToWin(k), player(player) {
}

void NmkEngine::solve() {
	for (int y = 0; y < board.getHeight(); y++) {
		for (int x = 0; x < board.getWidth(); x++) {
			if (Player::NONE == board.getPlayer(x, y)) {
				continue;
			}
			if (isWinning(x, y)) {
				printf(board.getPlayer(x, y) == Player::FIRST ? MESSAGE_P1 : MESSAGE_P2);
				return;
			}
		}
	}
	LinkedMoveList* threatsAtStart = new LinkedMoveList();
	fillThreatsAtStart(*threatsAtStart);
	Move move = Move(player.getOpponent(), UNKNOWN_MOVE, UNKNOWN_MOVE);
	Node* root = new Node(nullptr, move, Type::OR, threatsAtStart);
	int result = proofNumberSearch(root);
	delete root;
	if (result == TIE) {
		printf(MESSAGE_TIE);
		return;
	}
	printf(getWinningPlayer(result) == Player::FIRST ? MESSAGE_P1 : MESSAGE_P2);
}

int NmkEngine::proofNumberSearch(Node* root) {
	evaluate(root);
	setProofAndDisproofNumbers(root);
	Node* currentNode = root;
	while (root->proof != 0 && root->disproof != 0) {
		Node* mostProvingNode = selectMostProvingNode(currentNode);
		expandNode(mostProvingNode);
		currentNode = updateAncestors(mostProvingNode, root);
	}
	if (root->proof == 0) {
		return WIN;
	}
	bool tie = root->value != Value::UNKNOWN ? root->value == Value::DRAWN : detectTie(root);
	return tie ? TIE : LOSS;
}

void NmkEngine::evaluate(Node* root) const {
	Player playerToMove = root->moveMade.player.getOpponent();
	if (moveWasWinning(root->moveMade, *root->threats)) {
		setNodeValue(root, root->moveMade.player);
		return;
	}
	removeBlockedThreats(root->moveMade, *root->threats);
	if (board.isFull()) {
		root->value = Value::DRAWN;
		return;
	}
	if (root->threats->sizeByPlayer(playerToMove) >= 1) {
		setNodeValue(root, playerToMove);
		return;
	}
	addThreats(root->moveMade, *root->threats);
	if (root->threats->sizeByPlayer(root->moveMade.player) >= 2) {
		setNodeValue(root, root->moveMade.player);
		return;
	}
}

void NmkEngine::setProofAndDisproofNumbers(Node* node) {
	if (!node->expanded) {
		switch (node->value) {
		case Value::DISPROVEN: case Value::DRAWN:
			node->proof = INFINTE;
			node->disproof = 0;
			break;
		case Value::PROVEN:
			node->proof = 0;
			node->disproof = INFINTE;
			break;
		case Value::UNKNOWN:
			node->proof = 1;
			node->disproof = 1;
			break;
		}
		return;
	}
	if (node->type == Type::AND) {
		node->proof = 0;
		node->disproof = INFINTE;
		for (int i = 0; i < node->childrenCount; i++) {
			Node* child = node->children[i];
			node->proof += child->proof;
			if (node->proof < 0) node->proof = INFINTE;
			if (child->disproof < node->disproof) {
				node->disproof = child->disproof;
			}
		}
	} else {
		node->proof = INFINTE;
		node->disproof = 0;
		for (int i = 0; i < node->childrenCount; i++) {
			Node* child = node->children[i];
			node->disproof += child->disproof;
			if (node->disproof < 0) node->disproof = INFINTE;
			if (child->proof < node->proof) {
				node->proof = child->proof;
			}
		}
	}
}

NmkEngine::Node* NmkEngine::selectMostProvingNode(Node* node) {
	while (node->expanded) {
		int i = 0;
		Node* child = node->children[i++];
		if (node->type == Type::OR) {
			while (node->proof != child->proof) {
				child = node->children[i++];
			}
		} else {
			while (node->disproof != child->disproof) {
				child = node->children[i++];
			}
		}
		node = child;
		board.makeAMove(node->moveMade);
	}
	return node;
}

void NmkEngine::expandNode(Node* node) {
	generateChildren(node);
	for (int i = 0; i < node->childrenCount; i++) {
		Node* child = node->children[i];
		board.makeAMove(child->moveMade);
		evaluate(child);
		setProofAndDisproofNumbers(child);
		board.undoMove(child->moveMade);
		if (node->type == Type::AND) {
			if (child->disproof == 0) break;
		} else {
			if (child->proof == 0) break;
		}
	}
	node->expanded = true;
}

NmkEngine::Node* NmkEngine::updateAncestors(Node* node, Node* root) {
	do {
		int oldProof = node->proof;
		int oldDisproof = node->disproof;

		setProofAndDisproofNumbers(node);
		if (node->proof == oldProof && node->disproof == oldDisproof) {
			return node;
		}
		if (node == root) {
			return node;
		}
		board.undoMove(node->moveMade);
		node = node->parent;
	} while (true);
}

void NmkEngine::generateChildren(Node* node) {
	if (board.isFull()) {
		node->childrenCount = 0;
		return;
	}
	LinkedMoveList* possibleMoves = generatePossibleMoves(node->moveMade.player.getOpponent(), *node->threats);
	node->children = new Node * [possibleMoves->getSize()];
	node->childrenCount = possibleMoves->getSize();
	Type oppositeType = node->getOppositeType();
	int i = 0;
	for (LinkedMoveList::Iterator it = possibleMoves->start(); it.hasNext(); it.next()) {
		node->children[i++] = new Node(node, it.get(), oppositeType, new LinkedMoveList(*node->threats));
	}
	delete possibleMoves;
}

bool NmkEngine::detectTie(Node* node) {
	if (node->childrenCount == 0) {
		if (node->value == Value::UNKNOWN) {
			evaluate(node);
		}
		if (node->value == Value::UNKNOWN) {
			generateChildren(node);
			return detectTie(node);
		}
		return node->value == Value::DRAWN || node->value == Value::PROVEN;
	}
	if (node->type == Type::AND) {
		for (int i = 0; i < node->childrenCount; i++) {
			Node* child = node->children[i];
			board.makeAMove(child->moveMade);
			if (!detectTie(child)) {
				board.undoMove(child->moveMade);
				return false;
			}
			board.undoMove(child->moveMade);
		}
		return true;
	}
	for (int i = 0; i < node->childrenCount; i++) {
		Node* child = node->children[i];
		board.makeAMove(child->moveMade);
		if (detectTie(child)) {
			board.undoMove(child->moveMade);
			return true;
		}
		board.undoMove(child->moveMade);
	}
	return false;
}

void NmkEngine::setNodeValue(Node* node, Player& winningPlayer) const {
	node->value = winningPlayer == player ? Value::PROVEN : Value::DISPROVEN;
}

LinkedMoveList* NmkEngine::generatePossibleMoves(const Player& currPlayer, LinkedMoveList& threats) {
	LinkedMoveList* solutions = new LinkedMoveList();
	if (threats.sizeByPlayer(currPlayer.getOpponent()) > 0) {
		for (LinkedMoveList::Iterator it = threats.start(); it.hasNext(); it.next()) {
			if (it.get().player != currPlayer) {
				solutions->push(new Move(currPlayer, it.get().x, it.get().y));
			}
		}
		return solutions;
	}

	for (int y = 0; y < board.getHeight(); y++) {
		for (int x = 0; x < board.getWidth(); x++) {
			if (board.getPlayer(x, y) == Player::NONE) {
				solutions->push(new Move(currPlayer, x, y));
			}
		}
	}
	return solutions;
}

bool NmkEngine::isWinning(int x, int y) const {
	return isWinning(x, y, 1, 0)
		|| isWinning(x, y, 0, 1)
		|| isWinning(x, y, 1, 1)
		|| isWinning(x, y, 1, -1);
}

bool NmkEngine::isWinning(int startX, int startY, int dx, int dy) const {
	int counter = 1 + howManyInDirection(startX, startY, dx, dy) + howManyInDirection(startX, startY, -dx, -dy);
	return counter >= minToWin;
}

int NmkEngine::howManyInDirection(int startX, int startY, int dx, int dy) const {
	Player currPlayer = board.getPlayer(startX, startY);
	int counter = 0;
	int x = startX + dx;
	int y = startY + dy;
	while (board.withinBounds(x, y)) {
		if (board.getPlayer(x, y) != currPlayer) {
			break;
		}
		counter++;
		x += dx;
		y += dy;
	}
	return counter;
}

void NmkEngine::removeBlockedThreats(Move& currMove, LinkedMoveList& threats) const {
	for (LinkedMoveList::Iterator it = threats.start(); it.hasNext(); it.next()) {
		if (it.get().x == currMove.x && it.get().y == currMove.y && it.get().player != currMove.player) {
			it.remove();
		}
	}
}

bool NmkEngine::moveWasWinning(Move& currMove, LinkedMoveList& threats) {
	return threats.contains(currMove.player, currMove.x, currMove.y);
}

void NmkEngine::addThreats(Move& currMove, LinkedMoveList& threats) const {
	if (!currMove.moveIsKnown()) {
		return;
	}
	addThreats(currMove, threats, 1, 0);
	addThreats(currMove, threats, 0, 1);
	addThreats(currMove, threats, 1, 1);
	addThreats(currMove, threats, 1, -1);
}

void NmkEngine::addThreats(Move& currMove, LinkedMoveList& threats, int dx, int dy) const {
	int counterNormal = 0;
	int counterReversed = 0;
	int skipCounterNormal = 0;
	int skipCounterReversed = 0;
	Move* skipNormal = howManyInDirectionWithSkip(currMove, dx, dy, counterNormal, skipCounterNormal);
	Move* skipReversed = howManyInDirectionWithSkip(currMove, -dx, -dy, counterReversed, skipCounterReversed);
	int counter = counterNormal + counterReversed;
	if (skipNormal != nullptr) {
		if (counter + skipCounterNormal + 2 >= minToWin && !threats.contains(skipNormal->player, skipNormal->x, skipNormal->y)) {
			threats.push(skipNormal);
		} else {
			delete skipNormal;
		}
	}
	if (skipReversed != nullptr) {
		if (counter + skipCounterReversed + 2 >= minToWin && !threats.contains(skipReversed->player, skipReversed->x, skipReversed->y)) {
			threats.push(skipReversed);
		} else {
			delete skipReversed;
		}
	}
	if (counter + 2 >= minToWin) {
		int startX = currMove.x;
		int startY = currMove.y;
		int normalX = startX + dx * (counterNormal + 1);
		int normalY = startY + dy * (counterNormal + 1);
		int reversedX = startX - dx * (counterReversed + 1);
		int reversedY = startY - dy * (counterReversed + 1);
		Player currPlayer = currMove.player;
		if (board.withinBounds(normalX, normalY) && board.getPlayer(normalX, normalY) == Player::NONE && !threats.contains(currPlayer, normalX, normalY)) {
			threats.push(new Move(currPlayer, normalX, normalY));
		}
		if (board.withinBounds(reversedX, reversedY) && board.getPlayer(reversedX, reversedY) == Player::NONE && !threats.contains(currPlayer, reversedX, reversedY)) {
			threats.push(new Move(currPlayer, reversedX, reversedY));
		}
	}
}

Move* NmkEngine::howManyInDirectionWithSkip(Move& currMove, int dx, int dy, int& counter, int& skipCounter) const {
	Move* skip = nullptr;
	int x = currMove.x + dx;
	int y = currMove.y + dy;
	Player currPlayer = currMove.player;
	Player opponent = currPlayer.getOpponent();
	while (board.withinBounds(x, y)) {
		Player pl = board.getPlayer(x, y);
		if (pl == opponent) {
			break;
		}
		if (pl == Player::NONE) {
			if (!(board.withinBounds(x + dx, y + dy) && board.getPlayer(x + dx, y + dy) == currPlayer)) {
				break;
			}
			skip = new Move(currPlayer, x, y);
			x += dx;
			y += dy;
			while (board.withinBounds(x, y) && board.getPlayer(x, y) == currPlayer) {
				skipCounter++;
				x += dx;
				y += dy;
			}
			break;
		}
		counter++;
		x += dx;
		y += dy;
	}
	return skip;
}

void NmkEngine::fillThreatsAtStart(LinkedMoveList& threats) const {
	for (int y = 0; y < board.getHeight(); y++) {
		for (int x = 0; x < board.getWidth(); x++) {
			if (board.getPlayer(x, y) != Player::NONE) {
				Move move(board.getPlayer(x, y), x, y);
				addThreats(move, threats);
			}
		}
	}
}

Player NmkEngine::getWinningPlayer(int result) const {
	return result == WIN ? player : player.getOpponent();
}


NmkEngine::Node::Node(Node* parent, Move move, Type type, LinkedMoveList* threats) : parent(parent), children(nullptr), proof(1), disproof(1), childrenCount(0), type(type), expanded(false), moveMade(move), value(Value::UNKNOWN), threats(threats) {
}

NmkEngine::Node::~Node() {
	for (int i = 0; i < childrenCount; i++) {
		delete children[i];
	}
	delete[] children;
	delete threats;
}

NmkEngine::Type NmkEngine::Node::getOppositeType() const {
	return type == Type::AND ? Type::OR : Type::AND;
}
