#include <iostream>
#include "game.h"
#include <array>
#include <memory>
#include <algorithm>

//@todo: could be private
/*gets differences and returns vector and it's "length" (number of movements in this direction before reaching destination)
*/
directions game::get_directions(int difference_x, int difference_y) {
	int vector_x = 0;
	int vector_y = 0;
	int distance = 0;

	if (difference_x == 0) {
		vector_x = 0;
		distance = abs(difference_y);
		vector_y = difference_y / distance;
	}
	else if (difference_y == 0) {
		vector_y = 0;
		distance = abs(difference_x);
		vector_x = difference_x / distance;
	}
	else if (difference_x == difference_y || difference_x == -difference_y) {
		vector_x = difference_x / abs(difference_x);
		vector_y = difference_y / abs(difference_y);
		distance = abs(difference_x);
	}
	return directions(vector_x, vector_y, distance);
}

//@todo: could be private
/* checks path - starting at "to square" going backwards it checks if slide or jump_and slide is closer to "to square" than obstacle in path (friendly or enemy troop)
* the function is called only if there is no move on move board (or if "to square" is outside of the move board of "from square")
*/
types_of_moves game::check_path(int from_x, int from_y, int to_x, int to_y) {
	//std::cout << "called" << from_x <<" "<<from_y <<" "<<to_x<<" " <<to_y<<std::endl;
	//std::cout << difference_x << " " << difference_y << std::endl;
	//std::cout << direction.x << " " << direction.y << std::endl;

	int difference_x = to_x - from_x;
	int difference_y = to_y - from_y;
	auto direction = get_directions(difference_x, difference_y);

	if (direction.distance == 0) {
		return nothing;
	}

	for (int i = direction.distance - 1; i > 0; i--) {
		int tile_x = from_x + i * direction.x;
		int tile_y = from_y + i * direction.y;

		std::cout << "x:" << tile_x << " y:" << tile_y << std::endl;

		//get correct tile
		int move_board_x = board[from_x][from_y]->position_on_move_board.x + i * direction.x;
		int move_board_y = board[from_x][from_y]->position_on_move_board.y + i * direction.y;

		types_of_moves tile = board[from_x][from_y]->other_moves[move_board_x][move_board_y];
		if (board[from_x][from_y]->starting_moves) {
			tile = board[from_x][from_y]->starting_moves[move_board_x][move_board_y];
		}

		if (tile == jump_and_slide || tile == slide) {
			return tile;
			break;
		}
		else if (board[tile_x][tile_y] != NULL) {
			return nothing;
			break;
		}
	}
	return nothing;
}

/* checks if coordinates represent square on the board. 
*/
bool game::coordinates_on_board(int x, int y) {
	return x >= 0 && x <= 5 && y >= 0 && y <= 5;
}

/* checks if coordinates represent square on the move board of any figure.
*/
bool game::coordinates_on_move_board(int x, int y) {
	return x >= 0 && x <= 4 && y >= 0 && y <= 4;
}

bool game::check_command(int x, int y, int from_x, int from_y, int to_x, int to_y) {
	//figure: bool contains command, vector<x,y> command coordinates
	int from_x_on_board = from_x - x + board[x][y]->position_on_move_board.x;
	int from_y_on_board = from_y - y + board[x][y]->position_on_move_board.y;
	int to_x_on_board = to_x - x + board[x][y]->position_on_move_board.x;
	int to_y_on_board = to_y - y + board[x][y]->position_on_move_board.y;
	if (coordinates_on_move_board(from_x_on_board, from_y_on_board) && coordinates_on_move_board(to_x_on_board, to_y_on_board)) {
		if (board[from_x][from_y] != NULL && ((board[from_x][from_y]->owned_by_first_player && first_player_plays) || !board[from_x][from_y]->owned_by_first_player && !first_player_plays)) {
			if(board[to_x][to_y] == NULL || ((board[to_x][to_y]->owned_by_first_player && !first_player_plays) || !board[to_x][to_y]->owned_by_first_player && first_player_plays)) {

			auto move_board = board[x][y]->other_moves;
			if (board[x][y]->starting_position) {
				move_board = board[x][y]->starting_moves;
			}

			if ((move_board[from_x_on_board][from_y_on_board] == command || move_board[from_x_on_board][from_y_on_board] == walk_or_command) &&
				(move_board[to_x_on_board][to_y_on_board] == command || move_board[to_x_on_board][to_y_on_board] == walk_or_command)) {
				std::cout << "command ok";
				return true;
			}
		}
		}
	}
	std::cout << "command failed";
	return false;
}

bool game::check_walk(int from_x, int from_y, int to_x, int to_y) {
	int difference_x = to_x - from_x;
	int difference_y = to_y - from_y;
	auto direction = get_directions(difference_x, difference_y);

	if (direction.distance == 0) {
		return false;
	}

	for (int i = direction.distance - 1; i > 0; i--) {
		int tile_x = from_x + i * direction.x;
		int tile_y = from_y + i * direction.y;
		if (board[tile_x][tile_y] != NULL) {
			return false;

		}
		std::cout << "x:" << tile_x << " y:" << tile_y << std::endl;
	}
	return true;
}
//checks if we move friendly troop, and if move can be made - if path is clear (or we jump/shoot) and if the destination is not occupied by friendly troop
types_of_moves game::get_move(int from_x, int from_y, int to_x, int to_y) {
	//check if both tiles are on the board
	if (!coordinates_on_board(from_x, from_y) || !coordinates_on_board(to_x, to_y)) {
		std::cout << "not correct coordinates";
		return nothing;
	}

	//checks if the start is occupied by friendly troop
	if (board[from_x][from_y] == NULL || (board[from_x][from_y]->owned_by_first_player && !first_player_plays) || (!board[from_x][from_y]->owned_by_first_player && first_player_plays)) {
		std::cout << "No rights for moving this troop";
		return nothing;
	}

	//checks if the destination is not occupied by friendly troop
	if (board[to_x][to_y] != NULL && ((board[to_x][to_y]->owned_by_first_player && first_player_plays) || (board[to_x][to_y]->owned_by_first_player && first_player_plays))) {
		std::cout << "There already is friendly troop on this tile";
		return nothing;
	}

	int difference_x = to_x - from_x;
	int difference_y = to_y - from_y;

	//checks if the destination is on troops move board
	if (abs(difference_x) <= 2 && abs(difference_y) <= 2) {

		//gets coordinations on move board 
		int cord_x = difference_x + board[from_x][from_y]->position_on_move_board.x;
		int cord_y = difference_y + board[from_x][from_y]->position_on_move_board.y;

		//chooses current side of the board
		types_of_moves tile = board[from_x][from_y]->other_moves[cord_x][cord_y];
		if (board[from_x][from_y]->starting_moves) {
			tile = board[from_x][from_y]->starting_moves[cord_x][cord_y];
		}

		switch (tile)
		{
		case nothing:
			return check_path(from_x, from_y, to_x, to_y);
			break;
		case walk:
		{
			if (check_walk(from_x, from_y, to_x, to_y)) {
				return walk;
			}
			return nothing;
			break;
		}
		case slide:
			std::cout << "slide";
			return slide;
			break;
		case jump_and_slide:
			std::cout << "jump+slide";
			return jump_and_slide;
			break;
		case shoot:
			if (board[to_x][to_y] == NULL) {
				return nothing;
			}
			std::cout << "shoot";
			return shoot;
			break;
		case jump:
			std::cout << "jump";
			return jump;
			break;
		case walk_or_command:
		{
			bool walkable = check_walk(from_x, from_y, to_x, to_y);
			break;
		}
		default:
			break;
		}
	}
	else {
		return check_path(from_x, from_y, to_x, to_y);
	}
}

void game::print_board() {
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < 6; j++)
		{
			if (board[j][i] == NULL) {
				std::cout << "__ ";
			}
			else {
				std::cout << board[j][i]->return_symbol() << " ";
			}
		}
		std::cout << std::endl;

	}
}


bool game::add_new_figure(int to_x, int to_y, troop_name name_of_troop) {
	if (board[to_x][to_y] != NULL) {
		return false;
	}

	bool was_available = false;

	if (first_player_plays) {
		was_available = first_player.deploy_troop(name_of_troop, to_x, to_y);
	}
	else {
		was_available = second_player.deploy_troop(name_of_troop, to_x, to_y);
	}

	if (was_available) {
		switch (name_of_troop) {
		case Duke:
			board[to_x][to_y] = std::unique_ptr<figure>(new duke(to_x, to_y, first_player_plays));
			break;
		case Pikeman:
			board[to_x][to_y] = std::unique_ptr<figure>(new pikeman(to_x, to_y, first_player_plays));
			break;
		case Marshall:
			board[to_x][to_y] = std::unique_ptr<figure>(new marshall(to_x, to_y, first_player_plays));
			break;
		case Ranger:
			board[to_x][to_y] = std::unique_ptr<figure>(new ranger(to_x, to_y, first_player_plays));
			break;
		case Assassin:
			board[to_x][to_y] = std::unique_ptr<figure>(new assassin(to_x, to_y, first_player_plays));
			break;
		case General:
			board[to_x][to_y] = std::unique_ptr<figure>(new general(to_x, to_y, first_player_plays));
			break;
		case Knight:
			board[to_x][to_y] = std::unique_ptr<figure>(new knight(to_x, to_y, first_player_plays));
			break;
		case Longbowman:
			board[to_x][to_y] = std::unique_ptr<figure>(new longbowman(to_x, to_y, first_player_plays));
			break;
		case Champion:
			board[to_x][to_y] = std::unique_ptr<figure>(new champion(to_x, to_y, first_player_plays));
			break;
		case Wizard:
			board[to_x][to_y] = std::unique_ptr<figure>(new wizard(to_x, to_y, first_player_plays));
			break;
		case Seer:
			board[to_x][to_y] = std::unique_ptr<figure>(new seer(to_x, to_y, first_player_plays));
			break;
		case Footman:
			board[to_x][to_y] = std::unique_ptr<figure>(new footman(to_x, to_y, first_player_plays));
			break;
		case Priest:
			board[to_x][to_y] = std::unique_ptr<figure>(new priest(to_x, to_y, first_player_plays));
			break;
		case Bowman:
			board[to_x][to_y] = std::unique_ptr<figure>(new bowman(to_x, to_y, first_player_plays));
			break;
		case Dragoon:
			board[to_x][to_y] = std::unique_ptr<figure>(new dragoon(to_x, to_y, first_player_plays));
			break;
		default:

			break;
		}
	}
	return true;
}

//@todo: case command
void game::move_troop(int from_x, int from_y, int to_x, int to_y) {
	auto move = get_move(from_x, from_y, to_x, to_y);
	switch (move) {
	case nothing:
		std::cout << "not so succesfully moved" << std::endl;
		break;
	case shoot:
		std::cout << "shoot";
		remove_figure(to_x, to_y);
		break;
	default:
		if (board[to_x][to_y] != NULL) {
			remove_figure(to_x, to_y);
		}
		board[to_x][to_y] = std::move(board[from_x][from_y]);
		if (first_player_plays) {
			first_player.change_coordinates(from_x, from_y, to_x, to_y);
		}
		else {
			second_player.change_coordinates(from_x, from_y, to_x, to_y);
		}
		std::cout << "succesfully moved" << std::endl;
		//change coordinates of moved troop in player actives
		break;
	}
}

//checks if coordinates are on the board and removes troop on this square
//if the square was empty nothing happens
bool game::remove_figure(int x, int y) {
	if (x < 0 || x>5 || y < 0 || y>5) {
		return false;
	}
	if (board[x][y]->owned_by_first_player) {
		first_player.kill_troop(x, y);
	}
	else {
		second_player.kill_troop(x, y);
	}
	board[x][y] = NULL;
	return true;
}


void game::print_packs() {
	std::cout << "--------------active--------------" << std::endl;
	first_player.print_active();
	second_player.print_active();
	std::cout << "--------------packs--------------" << std::endl;
	first_player.print_pack();
	second_player.print_pack();
	std::cout << "-----------------------------" << std::endl << std::endl;
}