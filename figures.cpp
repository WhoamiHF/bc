#include "figures.h"
#include <iostream>

/*cooardinates figure::project_to_move_board(int x,int y) {
	int distance_squared = x * x + y * y;
	if (distance_squared < 9 || x == 0 || y == 0 || x + y == 0 || x == y) {
		int move_x = x+2;
		int move_y = y+2;
		if (x < 0) {
			move_x = 0;
		}
		if (x > 4) {
			move_x = 4;
		}
		if (y < 0) {
			move_y = 0;
		}
		if (y > 4) {
			move_y = 4;
		}
		return cooardinates(move_x, move_y, true);
		
	}
	return cooardinates(0, 0, false);
}*/

/*bool figure::is_slide_between(int x, int y) {
	int between_x = 2 + (x - 2) / 2;
	int between_y = 2 + (y - 2) / 2;
	types_of_moves tile = other_moves[between_x][between_y];
	if (starting_moves) {
		tile = starting_moves[between_x][between_y];
	}
	if (tile == slide || tile == jump_and_slide) {
		return true;
	}
	return false;
}*/

