#pragma once
#include "figures.h"
#include "player.h"
#include <array>
#include <memory>
#include <vector>
#include <map>
#include <unordered_set>
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
		winning_states = std::unordered_set<std::string>();
		previous_hash = "start";
	}

	game(const game& t) {
		for (size_t i = 0; i < 6; i++)
		{
			for (size_t j = 0; j < 6; j++)
			{
				if (t.board[i][j] == NULL) {
					this->board[i][j] = NULL;
				}
				else {
					this->board[i][j] = t.board[i][j]->clone();
				}
			}
		}

		this->first_player = t.first_player;
		this->second_player = t.second_player;
		this->first_player_plays = t.first_player_plays;
	}

	game(bool first_player_plays_, player first, player second) {
		first_player_plays = first_player_plays_;
		first_player = first;
		second_player = second;
	}

	std::unique_ptr<figure> board[6][6];
	bool add_new_figure(int to_x, int to_y, troop_name name_of_troop);

	bool first_player_plays;

	void print_board();
	void print_packs();

	bool move_troop(coordinates from,coordinates to);

	
	void place_starting_troops();	
	void play();

	bool command_troop(int x, int y, int fx, int fy, int tx, int ty);

	std::vector<possible_move> possible_moves;
	void collect_all_possible_moves();
	std::string create_hash();
	void mark_winning_state();
	void mark_losing_state();
	void load_winning_states();
	std::unordered_set<std::string> winning_states;
	double evaluate_state();
	double evaluate_move(possible_move move);
	std::vector<std::string> get_hash_after_turn(possible_move move);

private:
	void collect_addition(int x, int y);
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

	types_of_moves get_move(coordinates from, coordinates to);
	
	void append_active_to_hash(bool first, std::string& hash);
	void append_passive_to_hash(bool first, std::string& hash);

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
	std::string previous_hash;
};