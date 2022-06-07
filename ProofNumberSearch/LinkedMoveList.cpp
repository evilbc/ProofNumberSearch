#include "LinkedMoveList.h"
#include <assert.h>

LinkedMoveList::LinkedMoveList() : head(nullptr), tail(nullptr), size(0) {
}

LinkedMoveList::LinkedMoveList(const LinkedMoveList& other) : head(nullptr), tail(nullptr), size(0) {
	if (other.size == 0) {
		return;
	}
	Node* current = other.head;
	while (current != nullptr) {
		push(new Move(current->move->player, current->move->x, current->move->y));
		current = current->next;
	}
}

LinkedMoveList::~LinkedMoveList() {
	Node* current = head;
	while (current != nullptr) {
		Node* prev = current;
		current = current->next;
		delete prev;
	}
}

size_t LinkedMoveList::getSize() const {
	return size;
}

void LinkedMoveList::push(Move* move) {
	Node* last = tail;
	tail = new Node(move);
	if (head == nullptr) {
		head = tail;
	} else {
		last->next = tail;
		tail->previous = last;
	}
	size++;
}

bool LinkedMoveList::isEmpty() const {
	return size == 0;
}

LinkedMoveList::Iterator LinkedMoveList::start() {
	return Iterator(head, *this);
}

void LinkedMoveList::remove(Iterator& it) {
	assert(!isEmpty());
	Node* prev = it.current->previous;
	Node* next = it.current->next;
	if (prev != nullptr) {
		prev->next = next;
	} else {
		head = head->next;
		if (head != nullptr) {
			head->previous = nullptr;
		}
	}
	if (next != nullptr) {
		next->previous = prev;
	} else {
		tail = tail->previous;
		if (tail != nullptr) {
			tail->next = nullptr;
		}
	}
	delete it.current;
	it.current = next;

	size--;
}

LinkedMoveList::Node::Node(Move* move) : next(nullptr), previous(nullptr), move(move) {
}

LinkedMoveList::Node::~Node() {
	delete move;
}

LinkedMoveList::Iterator::Iterator(Node* current, LinkedMoveList& list) : current(current), list(list), removed(false) {
}

Move& LinkedMoveList::Iterator::get() {
	return *(current->move);
}

void LinkedMoveList::Iterator::next() {
	current = removed ? current : current->next;
	removed = false;
}

bool LinkedMoveList::Iterator::hasNext() const {
	return current != nullptr;
}

void LinkedMoveList::Iterator::remove() {
	list.remove(*this);
	removed = true;
}

bool LinkedMoveList::contains(Player player, int x, int y) const {
	if (size == 0) {
		return false;
	}
	Node* current = head;
	while (current != nullptr) {
		if (current->move->player == player && current->move->x == x && current->move->y == y) {
			return true;
		}
		current = current->next;
	}
	return false;
}

size_t LinkedMoveList::sizeByPlayer(Player player) const {
	if (size == 0) {
		return 0;
	}
	size_t result = 0;
	Node* current = head;
	while (current != nullptr) {
		if (current->move->player == player) {
			result++;
		}
		current = current->next;
	}
	return result;
}