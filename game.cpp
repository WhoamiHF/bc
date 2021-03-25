#include <iostream>
#include "game.h"
#include <array>
#include <memory>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

/*gets differences and returns normalized vector and it's "length" (number of movements in this direction before reaching destination)
* @param difference_x - represents x part of non-normalized vector (from start to finish)
* @param difference_y - represents y part of non-normalized vector (from start to finish)
*/
directions game::get_directions(coordinates difference) {
	int vector_x = 0;
	int vector_y = 0;
	int distance = 0;

	if (difference.x == 0) {
		vector_x = 0;
		distance = abs(difference.y);
		vector_y = difference.y / distance;
	}
	else if (difference.y == 0) {
		vector_y = 0;
		distance = abs(difference.x);
		vector_x = difference.x / distance;
	}
	else if (difference.x == difference.y || difference.x == -difference.y) {
		vector_x = difference.x / abs(difference.x);
		vector_y = difference.y / abs(difference.y);
		distance = abs(difference.x);
	}
	return directions(vector_x, vector_y, distance);
}

/* checks path - going backwards it checks if slide or jump_and slide is closer to "to square" than obstacle in path (friendly or enemy troop)
* the function is called only if there is no move on move board (or if "to square" is outside of the move board of "from square")
*/
types_of_moves game::check_path(coordinates from, coordinates to) {
	int difference_x = to.x - from.x;
	int difference_y = to.y - from.y;

	auto direction = get_directions(coordinates(difference_x, difference_y));

	if (direction.distance == 0) {
		return nothing;
	}

	for (int i = direction.distance - 1; i > 0; i--) {
		int tile_x = from.x + i * direction.x;
		int tile_y = from.y + i * direction.y;

		//check if there is troop in the way
		if (board[tile_x][tile_y] != NULL) {
			return nothing;
			break;
		}

		//get correct tile on move board
		int move_board_x = board[from.x][from.y]->position_on_move_board.x + i * direction.x;
		int move_board_y = board[from.x][from.y]->position_on_move_board.y + i * direction.y;

		//first player plays from up to down, left to right so we need to mirror difference around x and y axis
		if (first_player_plays) {
			move_board_x = board[from.x][from.y]->position_on_move_board.x - i * direction.x;
			move_board_y = board[from.x][from.y]->position_on_move_board.y - i * direction.y;
		}

		if (coordinates_on_move_board(move_board_x, move_board_y)) {
			types_of_moves tile = board[from.x][from.y]->other_moves[move_board_x][move_board_y];
			if (board[from.x][from.y]->starting_position) {
				tile = board[from.x][from.y]->starting_moves[move_board_x][move_board_y];
			}

			if (tile == jump_and_slide || tile == slide) {
				return tile;
				break;
			}
		}
	}
	return nothing;
}

/* checks if coordinates represent square on the board - if they are within the boundries of the board
*/
bool game::coordinates_on_board(int x, int y) {
	return x >= 0 && x <= 5 && y >= 0 && y <= 5;
}

/* checks if coordinates represent square on the move board of any figure - if they are within the boundries of a move board
*/
bool game::coordinates_on_move_board(int x, int y) {
	return x >= 0 && x <= 4 && y >= 0 && y <= 4;
}

/* checks if figure on [x,y] can command figure to move from "from square" to "to square"
* this means that the figure has command (or walk_or_command) on move board (on both squares) and "from square" is occupied by friendly troop whilst "to square" is not.
*
*/
bool game::check_command(coordinates base, coordinates from, coordinates to) {
	//checks if there is friendly troop on "from square"
	if (board[from.x][from.y] == NULL || !belongs_to_current_player(from.x, from.y)) {
		return false;
	}

	//@todo: figure: bool contains command, vector<x,y> command coordinates
	int difference_x_from = from.x - base.x;
	int difference_y_from = from.y - base.y;
	int difference_x_to = to.x - base.x;
	int difference_y_to = to.y - base.y;

	//first player plays from up to down, left to right so we need to mirror difference around x and y axis, this can be done only because difference is not used for absolute positions on board
	if (first_player_plays) {
		difference_x_from = -difference_x_from;
		difference_x_to = -difference_x_to;
		difference_y_from = -difference_y_from;
		difference_y_to = -difference_y_to;
	}

	int from_x_on_board = difference_x_from + board[base.x][base.y]->position_on_move_board.x;
	int from_y_on_board = difference_y_from + board[base.x][base.y]->position_on_move_board.y;
	int to_x_on_board = difference_x_to + board[base.x][base.y]->position_on_move_board.x;
	int to_y_on_board = difference_y_to + board[base.x][base.y]->position_on_move_board.y;


	//checks if coordinates of all three are on board
	if (!coordinates_on_board(base.x, base.y) || !coordinates_on_board(from.x, from.y) || !coordinates_on_board(to.x, to.y)) {
		return false;
	}

	//checks if "from square" and "to_square" are on move board of the troop on position [x][y]
	if (!coordinates_on_move_board(from_x_on_board, from_y_on_board) || !coordinates_on_move_board(to_x_on_board, to_y_on_board)) {
		return false;
	}

	//checks if there is not friendly troop on "to square"
	if (board[to.x][to.y] == NULL || !belongs_to_current_player(to.x, to.y)) {

		auto move_board = board[base.x][base.y]->other_moves;
		if (board[base.x][base.y]->starting_position) {
			move_board = board[base.x][base.y]->starting_moves;
		}

		if ((move_board[from_x_on_board][from_y_on_board] == command || move_board[from_x_on_board][from_y_on_board] == walk_or_command) &&
			(move_board[to_x_on_board][to_y_on_board] == command || move_board[to_x_on_board][to_y_on_board] == walk_or_command)) {
			return true;
		}
	}
	return false;
}

/* checks if there is obstacle between from and to square.
* by obstacle we mean any troop - friendly or enemy
*
*/
bool game::check_walk(coordinates from, coordinates to) {
	int difference_x = to.x - from.x;
	int difference_y = to.y - from.y;

	auto direction = get_directions(coordinates(difference_x, difference_y));

	if (direction.distance == 0) {
		return false;
	}

	for (int i = direction.distance - 1; i > 0; i--) {
		int tile_x = from.x + i * direction.x;
		int tile_y = from.y + i * direction.y;

		if (board[tile_x][tile_y] != NULL) {
			return false;

		}
	}
	return true;
}

//checks if we move friendly troop, and if move can be made - if path is clear (or we jump/shoot) and if the destination is not occupied by friendly troop
types_of_moves game::get_move(coordinates from, coordinates to) {
	//check if both tiles are on the board
	if (!coordinates_on_board(from.x, from.y) || !coordinates_on_board(to.x, to.y)) {
		return nothing;
	}

	//checks if the start is occupied by friendly troop
	if (board[from.x][from.y] == NULL || !belongs_to_current_player(from.x, from.y)) {
		return nothing;
	}

	//checks if the destination is not occupied by friendly troop
	if (board[to.x][to.y] != NULL && belongs_to_current_player(to.x, to.y)) {
		return nothing;
	}

	int difference_x = to.x - from.x;
	int difference_y = to.y - from.y;
	if (first_player_plays) {
		difference_x = -difference_x;
		difference_y = -difference_y;
	}

	//gets coordinations on move board 
	int cord_x = difference_x + board[from.x][from.y]->position_on_move_board.x;
	int cord_y = difference_y + board[from.x][from.y]->position_on_move_board.y;

	//checks if the destination is on troop's move board
	if (coordinates_on_move_board(cord_x, cord_y)) {

		//chooses current side of the board
		auto move_board = board[from.x][from.y]->other_moves;
		if (board[from.x][from.y]->starting_position) {
			move_board = board[from.x][from.y]->starting_moves;
		}

		switch (move_board[cord_x][cord_y])
		{
		case nothing:
			return check_path(from, to);
			break;
		case walk:
		{
			if (check_walk(from, to)) {
				return walk;
			}
			return nothing;
			break;
		}
		case slide:
			return slide;
			break;
		case jump_and_slide:
			return jump_and_slide;
			break;
		case shoot:
			if (board[to.x][to.y] == NULL) {
				return nothing;
			}
			return shoot;
			break;
		case jump:
			return jump;
			break;
		case walk_or_command:
		{
			//since command is treated separately, this results only to walk
			if (check_walk(from, to)) {
				return walk;
			}
			return nothing;
			break;
		}
		case command:
			//since command is treated separately, this results to nothing
			return check_path(from, to);
			break;
		default:
			break;
		}
	}
	else {
		return check_path(from, to);
	}
}

/* provides basic print of the current state of the board
*/
void game::print_board() {
	std::cout << "  0  1  2  3  4  5 " << std::endl;
	for (size_t i = 0; i < 6; i++)
	{
		std::cout << i << " ";
		for (size_t j = 0; j < 6; j++)
		{
			if (board[j][i] == NULL) {
				std::cout << "__ ";
			}
			else {
				auto tmp = board[j][i]->return_symbol();
				if (!board[j][i]->starting_position) {
					tmp[0] = tolower(tmp[0]);
				}
				std::cout << tmp << " ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

/* checks if troop on given square belongs to currently playing player. Checking for null is responsibility of caller
*/
bool game::belongs_to_current_player(int x, int y) {
	return (board[x][y]->owned_by_first_player && first_player_plays) || (!board[x][y]->owned_by_first_player && !first_player_plays);
}

/*checks if given square contains friendly Duke
*/
bool game::is_there_duke(int x, int y) {
	if (coordinates_on_board(x, y) && board[x][y] != NULL && board[x][y]->name == "Duke" && belongs_to_current_player(x, y)) {
		return true;
	}
	return false;
}

/*checks if given square is next to friendly duke
*/
bool game::is_next_to_duke(int x, int y) {
	return is_there_duke(x - 1, y) || is_there_duke(x + 1, y) || is_there_duke(x, y - 1) || is_there_duke(x, y + 1);

}

/* adds new figure with specific name to given square, only if the troop is available, if there is friendly duke next to the square and if the square is empty
* does not change current player
*/
bool game::add_new_figure(coordinates to, troop_name name_of_troop) {
	if (!coordinates_on_board(to.x, to.y) || board[to.x][to.y] != NULL) {
		return false;
	}

	if (name_of_troop != Duke && !is_next_to_duke(to.x, to.y)) {
		std::cout << "add new troop next to your Duke" << std::endl;
		return false;
	}

	bool was_available = false;
	if (first_player_plays) {
		was_available = first_player.packs.deploy_troop(name_of_troop, to.x, to.y);
	}
	else {
		was_available = second_player.packs.deploy_troop(name_of_troop, to.x, to.y);
	}

	if (was_available) {
		switch (name_of_troop) {
		case Duke:
			board[to.x][to.y] = std::unique_ptr<figure>(new duke(first_player_plays));
			break;
		case Pikeman:
			board[to.x][to.y] = std::unique_ptr<figure>(new pikeman(first_player_plays));
			break;
		case Marshall:
			board[to.x][to.y] = std::unique_ptr<figure>(new marshall(first_player_plays));
			break;
		case Ranger:
			board[to.x][to.y] = std::unique_ptr<figure>(new ranger(first_player_plays));
			break;
		case Assassin:
			board[to.x][to.y] = std::unique_ptr<figure>(new assassin(first_player_plays));
			break;
		case General:
			board[to.x][to.y] = std::unique_ptr<figure>(new general(first_player_plays));
			break;
		case Knight:
			board[to.x][to.y] = std::unique_ptr<figure>(new knight(first_player_plays));
			break;
		case Longbowman:
			board[to.x][to.y] = std::unique_ptr<figure>(new longbowman(first_player_plays));
			break;
		case Champion:
			board[to.x][to.y] = std::unique_ptr<figure>(new champion(first_player_plays));
			break;
		case Wizard:
			board[to.x][to.y] = std::unique_ptr<figure>(new wizard(first_player_plays));
			break;
		case Seer:
			board[to.x][to.y] = std::unique_ptr<figure>(new seer(first_player_plays));
			break;
		case Footman:
			board[to.x][to.y] = std::unique_ptr<figure>(new footman(first_player_plays));
			break;
		case Priest:
			board[to.x][to.y] = std::unique_ptr<figure>(new priest(first_player_plays));
			break;
		case Bowman:
			board[to.x][to.y] = std::unique_ptr<figure>(new bowman(first_player_plays));
			break;
		case Dragoon:
			board[to.x][to.y] = std::unique_ptr<figure>(new dragoon(first_player_plays));
			break;
		default:
			std::cout << "tried to add troop which was not recognized" << std::endl;
			return false;
			break;
		}
	}
	return true;
}

/* main function for moving troop. Gets which move can be done and does it.
* does not change current player
*/
bool game::move_troop(coordinates from, coordinates to) {
	auto move = get_move(from, to);
	switch (move) {
	case nothing:
		//std::cout << "Move from [" << from.x << "," << from.y << "] to [" << to.x << "," << to.y << "] was not succesful" << std::endl;
		return false;
		break;
	case shoot:
		//std::cout << "Succesful shoot from [" << from.x << "," << from.y << "] to [" << to.x << "," << to.y << "]" << std::endl;
		remove_figure(to.x, to.y);
		board[from.x][from.y]->starting_position = !board[from.x][from.y]->starting_position; //troop stays on same position, but changes moves
		break;
	default:
		//std::cout << "Succesful move from [" << from.x << "," << from.y << "] to [" << to.x << "," << to.y << "]" << std::endl;
		if (board[to.x][to.y] != NULL) {
			remove_figure(to.x, to.y);
		}
		board[to.x][to.y] = std::move(board[from.x][from.y]);
		if (first_player_plays) {
			first_player.change_coordinates(from.x, from.y, to.x, to.y);
		}
		else {
			second_player.change_coordinates(from.x, from.y, to.x, to.y);
		}
		board[to.x][to.y]->starting_position = !board[to.x][to.y]->starting_position; //troop has moved
		break;
	}
	return true;
}

/* checks if coordinates are on the board and removes troop on this square from the board and from player's active pack
*  if the square was empty nothing happens
*/
bool game::remove_figure(int x, int y) {
	if (!coordinates_on_board(x, y) || board[x][y] == NULL) {
		return false;
	}
	if (board[x][y]->name == "Duke") {
		gameover = true;
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

/* prints active troops and backup troops
*/
void game::print_packs() {
	std::cout << "--------------active--------------" << std::endl;
	first_player.print_active();
	second_player.print_active();
	std::cout << "--------------backup--------------" << std::endl;
	first_player.print_backup();
	second_player.print_backup();
	std::cout << "-----------------------------" << std::endl << std::endl;
}

/** this function is used for interaction with the user for getting next coordinate
*
*/
int game::get_safely_next_number(std::stringstream& stream) {
	std::string num;
	if (stream >> num) {
		return std::stoi(num);
	}
	else {
		return -1;
	}
}

/* deals with user's turn - asks for input, checks validity and does corresponding event - moving troop, commanding troop or adding troop
*
*/
bool game::user_play() {
	std::string row = "";
	std::cout << "Please insert command and arguments separated by spaces" << std::endl;
	std::getline(std::cin, row);
	std::stringstream s(row);
	std::string command;
	if (!(s >> command)) {
		return false;
	}
	if (command == "move") {
		int fx, fy, tx, ty;
		fx = get_safely_next_number(s);
		fy = get_safely_next_number(s);
		tx = get_safely_next_number(s);
		ty = get_safely_next_number(s);
		if (coordinates_on_board(fx, fy) && coordinates_on_board(tx, ty)) {
			return move_troop(coordinates(fx, fy), coordinates(tx, ty));
		}
		else {
			return false;
		}
	}
	else if (command == "add") {
		int tx, ty;
		tx = get_safely_next_number(s);
		ty = get_safely_next_number(s);
		troop_name name;

		if (!coordinates_on_board(tx, ty)) {
			return false;
		}

		if (first_player_plays) {
			name = first_player.pick_random_backup_figure();
		}
		else {
			name = second_player.pick_random_backup_figure();
		}
		return add_new_figure(coordinates(tx, ty), name);
	}
	else if (command == "command") {
		int x, y, fx, fy, tx, ty;
		s >> command;
		x = get_safely_next_number(s);
		y = get_safely_next_number(s);
		fx = get_safely_next_number(s);
		fy = get_safely_next_number(s);
		tx = get_safely_next_number(s);
		ty = get_safely_next_number(s);
		return command_troop(coordinates(x, y), coordinates(fx, fy), coordinates(tx, ty));
	}
	/*else if (command == "print") {
		collect_all_possible_moves();
		for (auto& item : possible_moves) {

		}
		return false;
	}*/
	else {
		return false;
	}

}

/* checks if commanding is possible via check_command and moves the troop, removing enemy if necessary
* does not change current player
*/
bool game::command_troop(coordinates base, coordinates from, coordinates to) {
	if (board[base.x][base.y] == NULL) {
		return false;
	}
	if (check_command(base, from, to)) {
		if (board[to.x][to.y] != NULL) {
			remove_figure(to.x, to.y);
		}

		board[to.x][to.y] = std::move(board[from.x][from.y]);

		if (first_player_plays) {
			first_player.change_coordinates(from.x, from.y, to.x, to.y);
		}
		else {
			second_player.change_coordinates(from.x, from.y, to.x, to.y);
		}

		board[base.x][base.y]->starting_position = !board[base.x][base.y]->starting_position; //troop has commanded

		return true;
	}
	return false;
}

double game::evaluate_state() {
	/*if (can_the_duke_be_taken()) {
		return -100;
	}*/

	double value = 0;

	for (auto& troop : first_player.packs.active) {
		if (troop.name == Duke) {
			value += 100;
		}
		value += 2;
	}
	for (auto& troop : second_player.packs.active) {
		if (troop.name == Duke) {
			value -= 100;
		}
		value -= 2;
	}

	//@todo: coeficient active/backup
	for (auto& troop : first_player.packs.backup) {
		value += 1;
	}

	for (auto& troop : second_player.packs.backup) {
		value -= 1;
	}

	if (first_player_plays) {
		value = -value;
	}
	return value;
}

/* evaluates all possible moves with lookahead of specified depth and returns best score
*
*/
double game::evaluate_all_possible_moves(int depth, considered_states_t& states) {
	std::string hash = create_hash();
	auto it = states.find(hash);
	//if is in states and depth is correct then we know the svaluation
	if (it != states.end() && it->second.depth >= depth) {
		return it->second.evaluation;
	}

	/*if (depth == 0) {
		return -evaluate_state();
	}*/

	std::vector<possible_move> possible_moves;
	collect_all_possible_moves(possible_moves);

	if (possible_moves.empty()) {
		return 0;
	}

	possible_move best_move;
	bool first = true;
	double best_score = 0;
	for (auto& item : possible_moves) {
		double score = evaluate_move(item, depth,states);
		if (first) {
			first = false;
			best_score = score;
			best_move = item;
		}
		else if (score > best_score) {
			best_score = score;
			best_move = item;
		}
	}
	states.emplace(std::pair<const std::string, evaluation_depth_and_move>(hash, evaluation_depth_and_move(best_score, best_move, DEPTH)));
	return best_score;
}

	
double game::evaluate_move(possible_move move, int depth, considered_states_t& states) {
	double return_value = 0;
	switch (move.op) {
	case move_it:
	{
		game tmp = *this;
		coordinates from = move.coords[0];
		coordinates to = move.coords[1];
		tmp.move_troop(from, to);
		tmp.first_player_plays = !first_player_plays;
		if (depth == 1) {
			return_value = tmp.evaluate_state();
		}
		else {
			return_value = -tmp.evaluate_all_possible_moves(depth - 1,states);
		}
		break;
	}
	case command_it:
	{
		game tmp = *this;
		coordinates base = move.coords[0];
		coordinates from = move.coords[1];
		coordinates to = move.coords[2];
		tmp.command_troop(base, from, to);
		tmp.first_player_plays = !first_player_plays;
		if (depth == 1) {
			return_value = tmp.evaluate_state();
		}
		else {
			return_value = -tmp.evaluate_all_possible_moves(depth - 1,states);
		}
	}
	case add_it:
	{
		coordinates to = move.coords[0];
		double sum = 0;
		auto collection = &second_player.packs.backup;
		if (first_player_plays) {
			collection = &first_player.packs.backup;
		}
		for (auto& item : *collection) {
			game tmp = *this;
			tmp.add_new_figure(to, item);
			/*if (first_player_plays) {
				tmp.first_player.packs.deploy_troop(item, to.x, to.y); //@todo: add to board
			}
			else {
				tmp.second_player.packs.deploy_troop(item, to.x, to.y); //@todo: add to board
			}*/
			tmp.first_player_plays = !first_player_plays;
			if (depth == 1) {
				sum += tmp.evaluate_state();
			}
			else {
				sum -= tmp.evaluate_all_possible_moves(depth - 1,states);
			}
		}

		return_value = sum / collection->size();
		break;
	}
	}
	return return_value;

}

void game::play_specific_move(possible_move move) {
	switch (move.op) {
	case move_it:
	{
		coordinates from = move.coords[0];
		coordinates to = move.coords[1];
		move_troop(from, to);
		break;
	}
	case add_it:
	{
		coordinates to = move.coords[0];
		troop_name name;
		if (first_player_plays) {
			name = first_player.pick_random_backup_figure();
		}
		else {
			name = second_player.pick_random_backup_figure();
		}
		add_new_figure(to, name);
		break;
	}
	case command_it:
	{
		coordinates base = move.coords[0];
		coordinates from = move.coords[1];
		coordinates to = move.coords[2];
		command_troop(base, from, to);
		break;
	}
	}
}

/* if current player is played by computer this function is called and chooses random move which is then done
*
*/
void game::computer_play(considered_states_t& states) {
	//create hash
	std::string hash = create_hash();
	auto it = states.find(hash);
	//if is in states and depth is correct then play it
	if (it != states.end() && it->second.depth >= DEPTH) {
		play_specific_move(it->second.move);
	}
	//else search for best move and add hash to parameter
	else {
		std::vector<possible_move> possible_moves = std::vector<possible_move>();
		collect_all_possible_moves(possible_moves);

		if (possible_moves.empty()) {
			return;
		}

		std::vector<possible_move> best_moves = std::vector<possible_move>();
		double best_score = 0;
		bool first = true;
		for (auto& item : possible_moves) {
			double score = evaluate_move(item, DEPTH, states);
			if (best_moves.empty()) {
				best_moves.push_back(item);
				best_score = score;
			}
			else if (score >= best_score) {
				if (score > best_score) {
					best_score = score;
					best_moves.clear();
				}
				best_moves.push_back(item);
			}
		}


		int index = rand() % best_moves.size();
		auto best_move = best_moves[index];
		states.emplace(std::pair<const std::string, evaluation_depth_and_move>(hash, evaluation_depth_and_move(best_score, best_move, DEPTH)));
		play_specific_move(best_move);
	}
}

/* main function - while a player didn't win it will ask players to play and print the board
*
*/
void game::play() {
	place_starting_troops();
	considered_states_t considered_states = considered_states_t();
	gameover = false;
	while (!gameover) {
		//previous_hash = create_hash();
		print_board();
		print_packs();
		std::cout << "player one" << std::endl;
		first_player_plays = true;
		if (first_player.played_by_pc) {
			computer_play(considered_states);
		}
		else {
			while (!user_play()) {}
		}
		print_board();
		first_player.sort_active_pack();
		print_packs();
		if (gameover) {
			std::cout << "player one won!" << std::endl;
			break;
		}

		//previous_hash = create_hash();
		std::cout << "player two" << std::endl;
		first_player_plays = false;

		if (second_player.played_by_pc) {
			computer_play(considered_states);
		}
		else {
			while (!user_play()) {}
		}

		if (gameover) {
			print_board();
			std::cout << "player two won!" << std::endl;
			break;
		}
		second_player.sort_active_pack();
	}
}

void game::place_starting_troops() {
	add_duke();
	add_duke();
	add_footman();
	add_footman();
	add_footman();
	add_footman();
}

/* checks whatever is playing computer or user and calls corresponding function for adding the duke
*
*/
void game::add_duke() {
	if ((first_player_plays && first_player.played_by_pc) || !first_player_plays && second_player.played_by_pc) {
		computer_add_duke();
	}
	else {
		user_add_duke();
	}
	print_board();
	first_player_plays = !first_player_plays;
}

/* checks whatever is playing computer or user and calls corresponding function for adding the footman
*
*/
void game::add_footman() {
	if ((first_player_plays && first_player.played_by_pc) || !first_player_plays && second_player.played_by_pc) {
		computer_add_footman();
	}
	else {
		user_add_footman();
	}
	print_board();
	first_player_plays = !first_player_plays;
}

/* randomly chooses position for the duke and places it there
*/
void game::computer_add_duke() {
	std::vector<coordinates> possibilites_first_player{ coordinates(2,0),coordinates(3,0) };
	std::vector<coordinates> possibilites_second_player{ coordinates(2,5),coordinates(3,5) };
	srand(time(NULL));
	int random_index = rand() % possibilites_first_player.size();;
	if (first_player_plays) {
		add_new_figure(possibilites_first_player[random_index], Duke);
	}
	else {
		add_new_figure(possibilites_second_player[random_index], Duke);
	}
}


/* randomly chooses position for the footman and places it there
*/
void game::computer_add_footman() {
	std::vector<coordinates> possibilites;
	if (first_player_plays) {
		possibilites = std::vector<coordinates>{ coordinates(1,0),coordinates(2,0),coordinates(3,0),coordinates(4,0),coordinates(2,1),coordinates(3,1) };
	}
	else {
		possibilites = std::vector < coordinates>{ coordinates(1,5),coordinates(2,5),coordinates(3,5),coordinates(4,5),coordinates(2,4),coordinates(3,4) };
	}
	srand(time(NULL));
	bool succes = false;
	int random_index = 0;
	while (!succes) {
		random_index = rand() % possibilites.size();
		int x = possibilites[random_index].x;
		int y = possibilites[random_index].y;
		if (board[x][y] == NULL && is_next_to_duke(x, y)) {
			succes = true;
		}
	}
	add_new_figure(possibilites[random_index], Footman);
}

/* checks that x and y represents square on which duke can be placed at the start
*/
bool game::check_duke_placement(int x, int y, bool first) {
	if (!coordinates_on_board(x, y)) {
		return false;
	}

	if (first && y == 0 && (x == 2 || x == 3)) {
		return true;
	}

	if (!first && y == 5 && (x == 2 || x == 3)) {
		return true;
	}

	return false;
}

/* asks user for placement of the footman at the start. It asks till the user places footman next to its own duke
*/
void game::user_add_footman() {
	int x, y;
	bool succes = false;
	while (!succes) {
		std::cout << "please type coordinates for placing footman next to your duke" << std::endl;
		std::cin >> x >> y;
		succes = add_new_figure(coordinates(x, y), Footman);
	}
}

/** gets coordinates for placing duke from user and places it
*
*/
void game::user_add_duke() {
	int x, y;
	bool succes = false;
	while (!succes) {
		if (first_player_plays) {
			std::cout << "please type coordinates 2 0 or 3 0 for placing duke" << std::endl;
		}
		else {
			std::cout << "please type coordinates 2 5 or 3 5 for placing duke" << std::endl;
		}
		std::cin >> x >> y;
		succes = check_duke_placement(x, y, first_player_plays);
	}
	add_new_figure(coordinates(x, y), Duke);
}

//caller is responsible for checking rights for using and existence of troop  on this square
void game::collect_commands(int x, int y, std::vector<possible_move>& possible_moves) {
	auto command_squares = &board[x][y]->command_squares_other;
	if (board[x][y]->starting_position) {
		command_squares = &board[x][y]->command_squares_starting;
	}

	std::vector<coordinates> start_command = std::vector<coordinates>();
	std::vector<coordinates> end_command = std::vector<coordinates>();
	if (command_squares->size() != 0) {
		for (auto& cord : *command_squares)
		{
			coordinates difference = coordinates(cord.x - board[x][y]->position_on_move_board.x, cord.y - board[x][y]->position_on_move_board.y);
			if (first_player_plays) { //orientation
				difference.x = -difference.x;
				difference.y = -difference.y;
			}
			int bx = x + difference.x;
			int by = y + difference.y;

			if (!coordinates_on_board(bx, by)) {
				continue;
			}

			//check for emptines/enemyness
			if (board[bx][by] == NULL || !belongs_to_current_player(bx, by)) {
				end_command.push_back(coordinates(bx, by));
			}
			else {
				start_command.push_back(coordinates(bx, by));
			}

		}
		for (auto& start : start_command) {
			for (auto& end : end_command) {
				std::vector<coordinates> coords{ coordinates(x,y),start,end };
				possible_moves.push_back(possible_move(coords, command_it));
				//std::cout << "[" << x << "," << y << "]" << "command" << "[" << start.x << "," << start.y << "]" << "[" << end.x << "," << end.y << "]" << std::endl;
			}
		}
	}
}

void game::collect_additions(int x, int y, std::vector<possible_move>& possible_moves) {
	if (coordinates_on_board(x, y) && board[x][y] == NULL) {
		std::vector<coordinates> coords{ coordinates(x,y) };
		possible_moves.push_back(possible_move(coords, add_it));
		//std::cout << x<< "," << y << std::endl;
	}
}

/*bool game::can_the_duke_be_taken() {

	auto collection_friendly = &second_player.packs.active;
	auto collection_enemy = &first_player.packs.active;
	if (first_player_plays) {
		collection_friendly = &first_player.packs.active;
		collection_enemy = &second_player.packs.active;
	}
	coordinates dukes_coordinates = coordinates(-1, -1);
	for (auto& item : *collection_enemy) {
		if (item.name == Duke) {
			dukes_coordinates = coordinates(item.x, item.y);
			break;
		}
	}
	for (auto& item : *collection_friendly) {
		if (get_move(coordinates(item.x, item.y), dukes_coordinates) != nothing) {
			std::vector<coordinates> coords{ coordinates(item.x,item.y),dukes_coordinates };
			//return possible_move(coords, move_it);
			return true;
		}

		auto collection_of_commands = &board[item.x][item.y]->command_squares_other;
		if (board[item.x][item.y]->starting_position) {
			collection_of_commands = &board[item.x][item.y]->command_squares_starting;
		}

		if (!collection_of_commands->empty()) {
			for (auto& troop : *collection_friendly) {
				if (check_command(coordinates(item.x, item.y), coordinates(troop.x, troop.y), dukes_coordinates)) { //@todo: only command squares
					std::vector<coordinates> coords{ coordinates(item.x,item.y),coordinates(troop.x,troop.y),dukes_coordinates };
					//return possible_move(coords,command_it);
					return true;
				}
			}
		}
	}
	return false;
	//return possible_move(std::vector<coordinates>(),add_it);


}*/

/* collects all possible commands, additions and moves for current player
* @param[output] possible_moves - reference for container which will store possible moves
*/
void game::collect_all_possible_moves(std::vector<possible_move>& possible_moves) {
	//possible_moves.clear();

	auto collection = &second_player.packs.active;
	if (first_player_plays) {
		collection = &first_player.packs.active;
	}

	//commands and moves
	for (auto& item : *collection) {
		if (item.name == Duke) {
			collect_additions(item.x, item.y + 1, possible_moves);
			collect_additions(item.x, item.y - 1, possible_moves);
			collect_additions(item.x + 1, item.y, possible_moves);
			collect_additions(item.x - 1, item.y, possible_moves);
		}

		collect_commands(item.x, item.y, possible_moves);

		for (size_t y_to = 0; y_to < 6; y_to++)
		{
			for (size_t x_to = 0; x_to < 6; x_to++)
			{
				auto move = get_move(coordinates(item.x, item.y), coordinates(x_to, y_to));
				if (move == nothing) {
					continue;
				}
				else {
					std::vector<coordinates> coords{ coordinates(item.x,item.y),coordinates(x_to,y_to) };
					possible_moves.push_back(possible_move(coords, move_it));
					//std::cout << "[" << item.x << "," << item.y << "]" << move << "[" << x_to << "," << y_to << "]" << std::endl;
				}
			}
		}
	}
}


/*void game::mark_winning_state() {
	std::ofstream my_file("winning_states.txt", std::ios_base::app);
	std::string hash = create_hash();

	my_file << hash;
	my_file.close();
}

void game::mark_losing_state() {
	std::ofstream my_file("losing_states.txt", std::ios_base::app);
	my_file << previous_hash;

	my_file.close();
}

void game::load_winning_states() {
	std::ifstream my_file("winning_states.txt");

	std::string row;
	while (std::getline(my_file, row)) {
		winning_states.emplace(row);
	}

	for (auto& item : winning_states) {
		std::cout << "[" << item << "]" << std::endl;
	}
	my_file.close();
}

std::vector<std::string> game::get_hash_after_turn(possible_move move) {
	packs_t first_packs = first_player.packs;
	packs_t second_packs = second_player.packs;

	std::vector<std::string> return_value = std::vector<std::string>();

	auto packs = second_packs;
	if (first_player_plays) {
		packs = first_packs;
	}

	switch (move.op) {
	case move_it:
		for (auto& fig : packs.backup) {
			packs.deploy_troop(fig, move.coords[0].x, move.coords[0].y);
			//create hash
			packs = second_packs;
			if (first_player_plays) {
				packs = first_packs;
			}

		}
		//tmp.move_troop(move.coords[0])
		break;
	}
	return return_value;
}*/

void game::append_active_to_hash(bool first, std::string& hash) {
	auto collection = second_player.packs.active;
	if (first) {
		collection = first_player.packs.active;
	}

	for (auto& item : collection) {
		char tmp = 'a' + item.name;

		if (board[item.x][item.y]->starting_position) {
			tmp = toupper(tmp);
		}
		hash.push_back(tmp);
		hash.push_back('0' + item.x);
		hash.push_back('0' + item.y);
	}
}

void game::append_passive_to_hash(bool first, std::string& hash) {
	auto collection = second_player.packs.backup;
	if (first) {
		collection = first_player.packs.backup;
	}

	for (auto& item : collection) {
		char tmp = 'a' + item;
		hash.push_back(tmp);
	}
}

std::string game::create_hash() {
	std::string hash = "s";
	if (first_player_plays) {
		hash = "f";
	}
	append_active_to_hash(true, hash);
	hash.push_back('|');
	append_active_to_hash(false, hash);
	hash.push_back('|');
	append_passive_to_hash(true, hash);
	hash.push_back('|');
	append_passive_to_hash(false, hash);
	//hash.push_back('\n');
	return hash;
}