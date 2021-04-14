#pragma once
#include "figures.h"
#include "player.h"
#include <array>
#include <memory>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>

#define DEPTH 4
#define PART 4

class player;

enum operation { add_it, move_it, command_it };

class possible_move {
public:
	possible_move() {
		coords = std::vector<coordinates>();
		op = add_it;
	}
	possible_move(std::vector<coordinates> coords_, operation operation_) {
		coords = coords_;
		op = operation_;
	}
	std::vector<coordinates> coords;
	operation op;
};


class evaluation_depth_and_move {
public:
	evaluation_depth_and_move(double eval, possible_move move_, int depth_) {
		evaluation = eval;
		move = move_;
		depth = depth_;
	}
	double evaluation;
	possible_move move;
	int depth;
};

class evaluation_and_move {
public:
	evaluation_and_move(double eval, possible_move move_) {
		evaluation = eval;
		move = move_;
	}
	double evaluation;
	possible_move move;
};

typedef std::unordered_map<std::string, evaluation_depth_and_move> considered_states_t;
typedef std::vector< std::vector<std::vector<coordinates>>> single_troop_sheet_t;
typedef std::map<troop_name, single_troop_sheet_t> all_troops_sheet_t;

class game {
public:
	game(all_troops_sheet_t* _sheet_odd,all_troops_sheet_t* _sheet_even) {
		first_player_plays = true;
		for (size_t i = 0; i < 6; i++)
		{
			for (size_t j = 0; j < 6; j++)
			{
				board[i][j] = NULL;

			}
		}
		first_player = player(true, true);
		second_player = player(false, true);
		gameover = false; //@todo game_states enum?
		sheet_even = _sheet_even;
		sheet_odd = _sheet_odd;
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
		this->sheet_even = t.sheet_even;
		this->sheet_odd = t.sheet_odd;
	}
	bool add_new_figure(coordinates to, troop_name name_of_troop,bool anywhere);

	bool first_player_plays; //@todo: private

	void print_board();
	void print_packs();

	types_of_moves move_troop(coordinates from, coordinates to);

	void play();
	void collect_all_possible_moves(std::vector<possible_move>& moves);

	std::unique_ptr<figure> board[6][6];
	void prepare_possible_moves(all_troops_sheet_t& sheet_odd, all_troops_sheet_t& sheet_even);
private:
	void undo();
	evaluation_and_move minimax(int depth, bool maximize, double alpha, double beta);
	void place_starting_troops();
	void user_add_footman();
	void user_add_duke();
	void computer_add_footman();
	void computer_add_duke();
	void add_duke();
	void add_footman();


	bool user_play();
	int get_safely_next_number(std::stringstream& stream);
	bool check_duke_placement(int x, int y, bool first);


	void computer_play(considered_states_t& states);
	void collect_addition(int x, int y, std::vector<coordinates>& squares);
	void collect_commands(int x, int y, std::vector<possible_move>& possible_moves);

	double evaluate_state(bool maximize);
	//double evaluate_all_possible_moves(int depth, considered_states_t& states);
	evaluation_and_move evaluate_move(possible_move move, int depth, bool maximize, double alpha, double beta);
	std::string create_hash();
	void append_active_to_hash(bool first, std::string& hash);
	void append_passive_to_hash(bool first, std::string& hash);

	void play_specific_move(possible_move move);
	bool command_troop(coordinates base, coordinates from, coordinates to);
	types_of_moves get_move(coordinates from, coordinates to,bool shoot_anywhere);
	bool remove_figure(int x, int y);

	directions get_directions(coordinates difference);
	types_of_moves check_path(coordinates from, coordinates to);
	bool coordinates_on_board(int x, int y);
	bool coordinates_on_move_board(int x, int y);

	bool check_walk(coordinates from, coordinates to);
	bool check_command(coordinates base, coordinates from, coordinates to);

	bool is_next_to_duke(int x, int y);
	bool is_there_duke(int x, int y);

	bool belongs_to_current_player(int x, int y);
	
	player first_player;
	player second_player;
	bool gameover;
	all_troops_sheet_t* sheet_odd;
	all_troops_sheet_t* sheet_even;
	//std::string previous_hash;
	//bool can_the_duke_be_taken();
	/*void mark_winning_state();
	void mark_losing_state();
	void load_winning_states();
	std::unordered_set<std::string> winning_states;
	std::vector<std::string> get_hash_after_turn(possible_move move);
	*/
	/*game(bool first_player_plays_, player first, player second) {
	first_player_plays = first_player_plays_;
	first_player = first;
	second_player = second;
	}*/
};

class precomputed {
public:
	precomputed(){
		sheet_even = all_troops_sheet_t();
		sheet_odd = all_troops_sheet_t();

		game tmp = game(&sheet_odd, &sheet_even);
		tmp.prepare_possible_moves(sheet_odd,sheet_even);
	}
	all_troops_sheet_t sheet_odd;
	all_troops_sheet_t sheet_even;
};