#pragma once
#include "figures.h"
#include "player.h"
#include <array>
#include <memory>
#include <vector>
class player;

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
		//passing this to player may be redundant
		first_player = player(true,this,false);
		second_player = player(false, this,true);
		gameover = false;
	}

	std::unique_ptr<figure> board[6][6];

	bool first_player_plays;
	player first_player;
	player second_player;

	bool gameover;

	types_of_moves get_move(int from_x, int from_y, int to_x, int to_y);
	types_of_moves check_path(int from_x, int from_y, int to_x, int to_y);
	directions get_directions(int difference_x, int difference_y);

	bool add_new_figure(int to_x, int to_y, troop_name name_of_troop);
	bool remove_figure(int x, int y);

	void print_board();
	void print_packs();

	bool coordinates_on_board(int x,int y);
	bool coordinates_on_move_board(int x, int y);

	void move_troop(int from_x, int from_y, int to_x, int to_y);

	bool check_walk(int from_x, int from_y, int to_x, int to_y);
	bool check_command(int x, int y, int from_x, int from_y, int to_x, int to_y);

	bool is_next_to_duke(int x, int y);
	bool is_there_duke(int x, int y);

	bool player_play(player& current_player);
	void play();
};