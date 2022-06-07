#pragma once

class Player {
public:
	enum Value {
		FIRST, SECOND, NONE
	};
	Player();
	Player(Value player);
	Player(int val);
	Player getOpponent() const;

	friend bool operator==(const Player& left, const Player::Value& right);
	friend bool operator==(const Player& left, const Player& right);
	friend bool operator!=(const Player& left, const Player& right);
	friend bool operator!=(const Player& left, const Player::Value& right);

	int valueAsInt() const;
private:
	Value value;
	static Value getVal(int val);
};

