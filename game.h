#pragma once
#include "figures.h"
#include "player.h"
#include <array>
#include <memory>
#include <vector>
class player;

enum operation{add_it,move_it,command_it};

class possible_move {
public:
	possible_move(std::vector<coordinates> coords_, operation operation_) {
		coords = coords_;
		op = operation_;
	}
	std::vector<coordinates> coords;
	operation op;
};

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
		first_player = player(true,true);
		second_player = player(false,true);
		gameover = false;
	}

	std::unique_ptr<figure> board[6][6];
	bool add_new_figure(int to_x, int to_y, troop_name name_of_troop);

	bool first_player_plays;

	void print_board();
	void print_packs();

	bool move_troop(int from_x, int from_y, int to_x, int to_y);

	
	void place_starting_troops();	
	void play();

	bool command_troop(int x, int y, int fx, int fy, int tx, int ty);

	std::vector<possible_move> possible_moves;
	void collect_all_possible_moves();

	
private:
	directions get_directions(int difference_x, int difference_y);
	types_of_moves check_path(int from_x, int from_y, int to_x, int to_y);

	void user_add_footman();
	void user_add_duke();
	void add_duke();
	void add_footman();
	void computer_add_footman();
	void computer_add_duke();

	bool user_play();
	void computer_play();

	int get_safely_next_number(std::stringstream& stream);

	types_of_moves get_move(int from_x, int from_y, int to_x, int to_y);


	bool remove_figure(int x, int y);

	bool coordinates_on_board(int x, int y);
	bool coordinates_on_move_board(int x, int y);

	bool check_walk(int from_x, int from_y, int to_x, int to_y);
	bool check_command(int x, int y, int from_x, int from_y, int to_x, int to_y);

	bool is_next_to_duke(int x, int y);
	bool is_there_duke(int x, int y);

	bool check_duke_placement(int x, int y, bool first);


	void collect_commands(int x, int y);
	bool belongs_to_current_player(int x, int y);

	player first_player;
	player second_player;
	bool gameover;
};