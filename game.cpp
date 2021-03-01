#include <iostream>
#include "game.h"
#include <array>
#include <memory>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>

/*gets differences and returns normalized vector and it's "length" (number of movements in this direction before reaching destination)
* @param difference_x - represents x part of non-normalized vector (from start to finish)
* @param difference_y - represents y part of non-normalized vector (from start to finish)
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

/* checks path - going backwards it checks if slide or jump_and slide is closer to "to square" than obstacle in path (friendly or enemy troop)
* the function is called only if there is no move on move board (or if "to square" is outside of the move board of "from square")
*/
types_of_moves game::check_path(int from_x, int from_y, int to_x, int to_y) {
	int difference_x = to_x - from_x;
	int difference_y = to_y - from_y;

	auto direction = get_directions(difference_x, difference_y);

	if (direction.distance == 0) {
		return nothing;
	}

	for (int i = direction.distance - 1; i > 0; i--) {
		int tile_x = from_x + i * direction.x;
		int tile_y = from_y + i * direction.y;

		if (board[tile_x][tile_y] != NULL) {
			return nothing;
			break;
		}

		//get correct tile
		int move_board_x = board[from_x][from_y]->position_on_move_board.x + i * direction.x;
		int move_board_y = board[from_x][from_y]->position_on_move_board.y + i * direction.y;

		//first player plays from up to down, left to right so we need to mirror difference around x and y axis
		if (first_player_plays) {
			move_board_x = board[from_x][from_y]->position_on_move_board.x - i * direction.x;
			move_board_y = board[from_x][from_y]->position_on_move_board.y - i * direction.y;
		}
		//@todo:check on move board
		if (coordinates_on_move_board(move_board_x, move_board_y)) {
			types_of_moves tile = board[from_x][from_y]->other_moves[move_board_x][move_board_y];
			if (board[from_x][from_y]->starting_position) {
				tile = board[from_x][from_y]->starting_moves[move_board_x][move_board_y];
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
bool game::check_command(int x, int y, int from_x, int from_y, int to_x, int to_y) {
	//@todo: figure: bool contains command, vector<x,y> command coordinates
	int difference_x_from = from_x - x;
	int difference_y_from = from_y - y;
	int difference_x_to = to_x - x;
	int difference_y_to = to_y - y;

	//first player plays from up to down, left to right so we need to mirror difference around x and y axis, this can be done only because difference is not used for absolute positions on board
	if (first_player_plays) {
		difference_x_from = -difference_x_from;
		difference_x_to = -difference_x_to;
		difference_y_from = -difference_y_from;
		difference_y_to = -difference_y_to;
	}

	int from_x_on_board = difference_x_from + board[x][y]->position_on_move_board.x;
	int from_y_on_board = difference_y_from + board[x][y]->position_on_move_board.y;
	int to_x_on_board = difference_x_to + board[x][y]->position_on_move_board.x;
	int to_y_on_board = difference_y_to + board[x][y]->position_on_move_board.y;


	//checks if coordinates of all three are on board
	if (!coordinates_on_board(x, y) || !coordinates_on_board(from_x, from_y) || !coordinates_on_board(to_x, to_y)) {
		return false;
	}

	//checks if "from square" and "to_square" are on move board of the troop on position [x][y]
	if (!coordinates_on_move_board(from_x_on_board, from_y_on_board) || !coordinates_on_move_board(to_x_on_board, to_y_on_board)) {
		return false;
	}

	//checks if there is friendly troop on "from square"
	if (board[from_x][from_y] == NULL || !belongs_to_current_player(from_x, from_y)) {
		return false;
	}

	//checks if there is not friendly troop on "to square"
	if (board[to_x][to_y] == NULL || !belongs_to_current_player(to_x, to_y)) {

		auto move_board = board[x][y]->other_moves;
		if (board[x][y]->starting_position) {
			move_board = board[x][y]->starting_moves;
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

	//checks if the destination is on troops move board
	if (coordinates_on_move_board(cord_x, cord_y)) {

		//chooses current side of the board
		auto move_board = board[from.x][from.y]->other_moves;
		if (board[from.x][from.y]->starting_position) {
			move_board = board[from.x][from.y]->starting_moves;
		}

		switch (move_board[cord_x][cord_y])
		{
		case nothing:
			return check_path(from.x, from.y, to.x, to.y);
			break;
		case walk:
		{
			if (check_walk(from.x, from.y, to.x, to.y)) {
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
			if (check_walk(from.x, from.y, to.x, to.y)) {
				return walk;
			}
			return nothing;
			break;
		}
		case command:
			//since command is treated separately, this results to nothing
			return check_path(from.x, from.y, to.x, to.y);
			break;
		default:
			break;
		}
	}
	else {
		return check_path(from.x, from.y, to.x, to.y);
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
* @todo: should be private
*/
bool game::add_new_figure(int to_x, int to_y, troop_name name_of_troop) {
	if (board[to_x][to_y] != NULL) {
		return false;
	}

	if (name_of_troop != Duke && !is_next_to_duke(to_x, to_y)) {
		std::cout << "add new troop next to your Duke" << std::endl;
		return false;
	}

	bool was_available = false;
	if (first_player_plays) {
		was_available = first_player.packs.deploy_troop(name_of_troop, to_x, to_y);
	}
	else {
		was_available = second_player.packs.deploy_troop(name_of_troop, to_x, to_y);
	}

	if (was_available) {
		switch (name_of_troop) {
		case Duke:
			board[to_x][to_y] = std::unique_ptr<figure>(new duke(first_player_plays));
			break;
		case Pikeman:
			board[to_x][to_y] = std::unique_ptr<figure>(new pikeman(first_player_plays));
			break;
		case Marshall:
			board[to_x][to_y] = std::unique_ptr<figure>(new marshall(first_player_plays));
			break;
		case Ranger:
			board[to_x][to_y] = std::unique_ptr<figure>(new ranger(first_player_plays));
			break;
		case Assassin:
			board[to_x][to_y] = std::unique_ptr<figure>(new assassin(first_player_plays));
			break;
		case General:
			board[to_x][to_y] = std::unique_ptr<figure>(new general(first_player_plays));
			break;
		case Knight:
			board[to_x][to_y] = std::unique_ptr<figure>(new knight(first_player_plays));
			break;
		case Longbowman:
			board[to_x][to_y] = std::unique_ptr<figure>(new longbowman(first_player_plays));
			break;
		case Champion:
			board[to_x][to_y] = std::unique_ptr<figure>(new champion(first_player_plays));
			break;
		case Wizard:
			board[to_x][to_y] = std::unique_ptr<figure>(new wizard(first_player_plays));
			break;
		case Seer:
			board[to_x][to_y] = std::unique_ptr<figure>(new seer(first_player_plays));
			break;
		case Footman:
			board[to_x][to_y] = std::unique_ptr<figure>(new footman(first_player_plays));
			break;
		case Priest:
			board[to_x][to_y] = std::unique_ptr<figure>(new priest(first_player_plays));
			break;
		case Bowman:
			board[to_x][to_y] = std::unique_ptr<figure>(new bowman(first_player_plays));
			break;
		case Dragoon:
			board[to_x][to_y] = std::unique_ptr<figure>(new dragoon(first_player_plays));
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
*
*/
bool game::move_troop(coordinates from, coordinates to) {
	auto move = get_move(from, to);
	switch (move) {
	case nothing:
		std::cout << "Move from [" << from.x << "," << from.y << "] to [" << to.x << "," << to.y << "] was not succesful" << std::endl;
		return false;
		break;
	case shoot:
		std::cout << "Succesful shoot from [" << from.x << "," << from.y << "] to [" << to.x << "," << to.y << "]" << std::endl;
		remove_figure(to.x, to.y);
		board[from.x][from.y]->starting_position = !board[from.x][from.y]->starting_position; //troop stays on same position, but changes moves
		break;
	default:
		std::cout << "Succesful move from [" << from.x << "," << from.y << "] to [" << to.x << "," << to.y << "]" << std::endl;
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
	if (x < 0 || x>5 || y < 0 || y>5) {
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
		return add_new_figure(tx, ty, name);
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
		return command_troop(x, y, fx, fy, tx, ty);
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
*
*/
bool game::command_troop(int x, int y, int fx, int fy, int tx, int ty) {
	if (board[x][y] == NULL) {
		return false;
	}
	if (check_command(x, y, fx, fy, tx, ty)) {
		if (board[tx][ty] != NULL) {
			remove_figure(tx, ty);
		}

		board[tx][ty] = std::move(board[fx][fy]);

		if (first_player_plays) {
			first_player.change_coordinates(fx, fy, tx, ty);
		}
		else {
			second_player.change_coordinates(fx, fy, tx, ty);
		}

		board[x][y]->starting_position = !board[x][y]->starting_position; //troop has commanded

		return true;
	}
	return false;
}

double game::evaluate_state() {
	double value = 0;
	for (auto& troop : first_player.packs.active) {
		value += 2;
	}
	for (auto& troop : second_player.packs.active) {
		value -= 2;
	}

	//@todo: coeficient active/backup
	for (auto& troop : first_player.packs.backup) {
		value += 1;
	}

	for (auto& troop : second_player.packs.backup) {
		value -= 1;
	}
	return value;
}

double game::evaluate_move(possible_move move) {
	double return_value = 0;
	switch (move.op) {
	case move_it:
	{
		game tmp = *this;
		coordinates from = move.coords[0];
		coordinates to = move.coords[1];
		tmp.move_troop(from, to);
		return_value = tmp.evaluate_state();
		break;
	}
	case command_it:
	{
		game tmp = *this;
		coordinates base = move.coords[0];
		coordinates from = move.coords[1];
		coordinates to = move.coords[2];
		return_value = tmp.evaluate_state();
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
			tmp.first_player.packs.deploy_troop(item, to.x, to.y); //@todo: add to board
			sum += tmp.evaluate_state();
		}

		return_value = sum / collection->size();
		break;
	}
	}
	return return_value;

}

/* if current player is played by computer this function is called and chooses random move which is then done
*
*/
void game::computer_play() {
	collect_all_possible_moves();
	//@todo: all posible moves - local?

	int index = rand() % possible_moves.size();
	switch (possible_moves[index].op) {
	case move_it:
	{
		coordinates from = possible_moves[index].coords[0];
		coordinates to = possible_moves[index].coords[1];
		move_troop(from, to);
		break;
	}
	case add_it:
	{
		coordinates to = possible_moves[index].coords[0];
		troop_name name;
		if (first_player_plays) {
			name = first_player.pick_random_backup_figure();
		}
		else {
			name = second_player.pick_random_backup_figure();
		}
		add_new_figure(to.x, to.y, name);
		break;
	}
	case command_it:
	{
		coordinates base = possible_moves[index].coords[0];
		coordinates from = possible_moves[index].coords[1];
		coordinates to = possible_moves[index].coords[2];
		command_troop(base.x, base.y, from.x, from.y, to.x, to.y);
		break;
	}
	}
}

/* main function - while a player didn't win it will ask players to play and print the board
*
*/
void game::play() {
	gameover = false;
	while (!gameover) {
		previous_hash = create_hash();
		print_board();
		print_packs();
		std::cout << "player one" << std::endl;
		first_player_plays = true;
		if (first_player.played_by_pc) {
			computer_play();
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

		previous_hash = create_hash();
		std::cout << "player two" << std::endl;
		first_player_plays = false;

		if (second_player.played_by_pc) {
			computer_play();
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
		add_new_figure(possibilites_first_player[random_index].x, possibilites_first_player[random_index].y, Duke);
	}
	else {
		add_new_figure(possibilites_second_player[random_index].x, possibilites_second_player[random_index].y, Duke);
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
	add_new_figure(possibilites[random_index].x, possibilites[random_index].y, Footman);
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
		succes = add_new_figure(x, y, Footman);
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
	add_new_figure(x, y, Duke);
}

//caller is responsible for checking rights for using and existence of troop  on this square
void game::collect_commands(int x, int y) {
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

void game::collect_addition(int x, int y) {
	if (coordinates_on_board(x, y) && board[x][y] == NULL) {
		std::vector<coordinates> coords{ coordinates(x,y) };
		possible_moves.push_back(possible_move(coords, add_it));
		//std::cout << x<< "," << y << std::endl;
	}
}

/* collects all possible commands, additions and moves for current player
*
*/
void game::collect_all_possible_moves() {
	possible_moves.clear();

	auto collection = &second_player.packs.active;
	if (first_player_plays) {
		collection = &first_player.packs.active;
	}

	//commands and moves
	for (auto& item : *collection) {
		if (item.name == Duke) {
			collect_addition(item.x, item.y + 1);
			collect_addition(item.x, item.y - 1);
			collect_addition(item.x + 1, item.y);
			collect_addition(item.x - 1, item.y);
		}

		collect_commands(item.x, item.y);

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


void game::mark_winning_state() {
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
}

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
	hash.push_back('\n');
	std::cout << hash;
	return hash;
}