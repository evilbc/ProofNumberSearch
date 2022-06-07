#define _CRT_SECURE_NO_WARNINGS

#include "InputHandler.h"
#include "Board.h"
#include "Engine.h"
#include <assert.h>
#include "Player.h"
#include <cstdio>
#include <string.h>


#define MAX_INPUT_LENGTH 50
#define SOLVE_COMMAND "SOLVE_GAME_STATE"

void InputHandler::handle() {
	char input[MAX_INPUT_LENGTH];
	while (true) {
		scanf("%s", input);
		if (feof(stdin) != 0) {
			break;
		}
		int height;
		int width;
		int minToWin;
		int playerNum;
		scanf("%d %d %d %d", &height, &width, &minToWin, &playerNum);
		Player player = Player(playerNum);
		Board board = Board(width, height);
		board.read();
		NmkEngine engine = NmkEngine(board, minToWin, player);
		if (strcmp(input, SOLVE_COMMAND) == 0) {
			engine.solve();
		} else {
			printf("Invalid command: %s\n", input);
		}
	}
}
