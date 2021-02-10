#include <iostream>
#include "game.h"
#include <array>
#include <memory>

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

//this is called only if it's outside move board  or move board on this tile is empty.
bool game::check_path(int from_x, int from_y, int to_x, int to_y) {
	//std::cout << "called" << from_x <<" "<<from_y <<" "<<to_x<<" " <<to_y<<std::endl;
	//std::cout << difference_x << " " << difference_y << std::endl;
	//std::cout << direction.x << " " << direction.y << std::endl;
	bool success = false;

	int difference_x = to_x - from_x;
	int difference_y = to_y - from_y;
	auto direction = get_directions(difference_x, difference_y);

	if (direction.distance == 0) {
		return false;
	}

	for (int i = direction.distance - 1; i > 0; i--) {
		int tile_x = from_x + i * direction.x;
		int tile_y = from_y + i * direction.y;

		std::cout << "x:" << tile_x << " y:" << tile_y << std::endl;

		//get correct tile
		int move_board_x = 2 + i * direction.x;
		int move_board_y = 2 + i * direction.y;
		types_of_moves tile = board[from_x][from_y]->other_moves[move_board_x][move_board_y];
		if (board[from_x][from_y]->starting_moves) {
			tile = board[from_x][from_y]->starting_moves[move_board_x][move_board_y];
		}

		if (tile == jump_and_slide || tile == slide) {
			success = true;
			std::cout << "suc" << std::endl;
			break;
		}
		else if (board[tile_x][tile_y] != NULL) {
			success = false;
			std::cout << "fail" << std::endl;
			break;
		}
	}
	return success;
}

bool game::is_on_board(int x,int y) {
	return x >= 0 && x <= 5 && y >= 0 && y <= 5;
}

//checks if we move friendly troop, and if move can be made - if path is clear (or we jump/shoot) and if the destination is not occupied by friendly troop
bool game::check_move(int from_x, int from_y, int to_x, int to_y) {
	//check if both tiles are on the board
	if (!is_on_board(from_x, from_y) || !is_on_board(to_x, to_y)) {
		std::cout << "not correct coordinates";
		return false;
	}

	//checks if the start is occupied by friendly troop
	if ((board[from_x][from_y]->owned_by_first_player && !first_player_plays) || (!board[from_x][from_y]->owned_by_first_player && first_player_plays)) {
		std::cout << "No rights for moving this troop";
		return false;
	}
	
	//checks if the destination is not occupied by friendly troop
	if (board[to_x][to_y] != NULL && ((board[to_x][to_y]->owned_by_first_player && first_player_plays) || (board[to_x][to_y]->owned_by_first_player && first_player_plays))) {
		std::cout << "There already is friendly troop on this tile";
		return false;
	}

	int difference_x = to_x - from_x;
	int difference_y = to_y - from_y;
	//kontrola preteceni z planku
	//checks if the destination is on troops move board
	if (abs(difference_x) <= 2 && abs(difference_y) <= 2) {

		//gets coordinations on move board 
		int cord_x = difference_x + 2;
		int cord_y = difference_y + 2;

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
			std::cout << "walk";
			break;
		case slide:
			std::cout << "slide";
			return true;
			break;
		case jump_and_slide:
			std::cout << "jump+slide";
			return true;
			break;
		case shoot:
			std::cout << "shoot";
			break;
		case jump:
			std::cout << "jump";
			return true;
			break;
		default:
			break;
		}
	}
	else {
		return check_path(from_x, from_y, to_x, to_y);
	}
	return true;
}

void game::print_board() {
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < 6; j++)
		{
			if (board[j][i] == NULL) {
				std::cout << "_ ";
			}
			else {
				std::cout<<board[j][i]->return_symbol()<<" ";
			}
		}
		std::cout << std::endl;

	}
}


bool game::add_new_figure(int to_x, int to_y, troop name_of_troop) {
	//board[to_x][to_y]->owned_by_first_player == first_players_turn
	if (board[to_x][to_y] != NULL) {
		return false;
	}
	
	switch(name_of_troop){
	case Duke:
	{
		board[to_x][to_y] = std::unique_ptr<figure>(new duke(to_x, to_y, first_player_plays));
		break;
	}
	default:

		break;
	}	
	return true;
}

void game::move_troop(int from_x,int from_y,int to_x,int to_y) {
	if (check_move(from_x, from_y, to_x, to_y)) {
		board[to_x][to_y] = std::move(board[from_x][from_y]);
		std::cout << "succesfully moved" <<std::endl;
	}
	else {
		std::cout << "not so succesfully moved"<<std::endl;
	}

}