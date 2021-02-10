#pragma once
#include "figures.h"
#include <array>
#include <memory>

enum troop{Duke,Ranger,Assassin,Wizard,General,Bowman,Champion,Knight,Marshall,Priest,Seer,Dragoon,Pikeman,Footman,Longbowman};

class game {
public:
	game() {
		first_player_plays = true;
		for (size_t i = 0; i < 6; i++)
		{
			for (size_t j = 0; j < 6; j++)
			{
				board[i][j] = NULL;

			}
		}
	}
	std::unique_ptr<figure> board[6][6];
	bool first_player_plays;

	bool check_move(int from_x, int from_y, int to_x, int to_y);
	bool check_path(int from_x, int from_y, int to_x, int to_y);
	directions get_directions(int difference_x, int difference_y);
	bool add_new_figure(int to_x, int to_y, troop name_of_troop);
	void print_board();
	bool is_on_board(int x,int y);
	void move_troop(int from_x, int from_y, int to_x, int to_y);
};