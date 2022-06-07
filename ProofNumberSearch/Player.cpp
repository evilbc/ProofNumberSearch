#include "Player.h"
#include <assert.h>

Player::Player() : value(Value::NONE) {
}

Player::Player(Value player) : value(player) {
}

Player::Player(int val) : value(getVal(val)) {
}

Player Player::getOpponent() const {
	assert(value != Player::NONE);
	return value == Player::FIRST ? Player::SECOND : Player::FIRST;
}

Player::Value Player::getVal(int val) {
	assert(val == 1 || val == 2 || val == 0);
	if (val == 0) {
		return Player::NONE;
	}
	return val == 1 ? Player::FIRST : Player::SECOND;
}

int Player::valueAsInt() const {
	switch (value) {
	case Player::NONE:
		return 0;
	case Player::FIRST:
		return 1;
	case Player::SECOND:
		return 2;
	}
	return 0;
}

bool operator==(const Player& left, const Player::Value& right) {
	return left.value == right;
}

bool operator==(const Player& left, const Player& right) {
	return left.value == right.value;
}

bool operator!=(const Player& left, const Player& right) {
	return left.value != right.value;
}

bool operator!=(const Player& left, const Player::Value& right) {
	return left.value != right;
}
