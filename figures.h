#pragma once
#include <iostream>
#include <array>
#include <memory>
#include <set> //possibly redundant
#include <vector>

enum types_of_moves { nothing, walk, slide, jump_and_slide, shoot, jump,command,walk_or_command };

class coordinates {
public:
	coordinates() = default;
	coordinates(int _x, int _y) { x = _x; y = _y;}
	int x;
	int y;
};

class directions {
public:
	directions(int x_, int y_, int distance_) {
		x = x_;
		y = y_;
		distance = distance_;
	}
	int x;
	int y;
	int distance;
};

class figure abstract {
public:
	figure() {
		for (size_t i = 0; i < 5; i++)
		{
			for (size_t j = 0; j < 5; j++)
			{
				starting_moves[i][j] = nothing;
				other_moves[i][j] = nothing;
			}
			command_squares_starting = std::vector<coordinates>();
			command_squares_other = std::vector<coordinates>();
			position_on_move_board = coordinates(2, 2);
			name = "";
		}
	}
	virtual std::string return_symbol()= 0;

	bool has_command;
	std::vector<coordinates> command_squares_starting;
	std::vector<coordinates> command_squares_other;

	bool starting_position = true;
	types_of_moves starting_moves[5][5];
	types_of_moves other_moves[5][5];
	
	coordinates position_on_move_board;

	int position_x;
	int position_y;
	bool owned_by_first_player;
	std::string name;

	void print_boards() {
		std::cout << "current" << std::endl;
		auto current = &other_moves;
		auto next = &starting_moves;
		if (starting_position) {
			current = &starting_moves;
			next = &other_moves;
		}
		for (size_t i = 0; i < 5; i++)
		{
			for (size_t j = 0; j < 5; j++)
			{
				switch ((*current)[j][i]) {
				case jump:
					std::cout << "jp ";
					break;
				case jump_and_slide:
					std::cout << "js ";
					break;
				case walk:
					std::cout << "wl ";
					break;
				case slide:
					std::cout << "sl ";
					break;
				case shoot:
					std::cout << "sh ";
					break;
				default:
					std::cout << "__ ";
				}
			}
			std::cout << std::endl;
		}
		std::cout << "next" << std::endl;
		for (size_t i = 0; i < 5; i++)
		{
			for (size_t j = 0; j < 5; j++)
			{
				switch ((*next)[j][i]) {
				case jump:
					std::cout << "jp ";
					break;
				case jump_and_slide:
					std::cout << "js ";
					break;
				case walk:
					std::cout << "wl ";
					break;
				case slide:
					std::cout << "sl ";
					break;
				case shoot:
					std::cout << "sh ";
					break;
				default:
					std::cout << "__ ";
				}
			}
			std::cout << std::endl;
		}
	}
};

//-----------------------------------------------------Figures declarations------------------------------------------------------------------//

class duke : public figure {
public:
	duke(int x_,int y_, bool first_player) {
		starting_moves[1][2] = slide;
		starting_moves[3][2] = slide;

		other_moves[2][1] = slide;
		other_moves[2][3] = slide;

		owned_by_first_player = first_player;
		name = "Duke";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "D1";
		}
		return "D2";
	}
};

class pikeman : public figure {
public:
	pikeman(int x_, int y_, bool first_player) {
		starting_moves[0][0] = walk;
		starting_moves[1][1] = walk;
		starting_moves[3][1] = walk;
		starting_moves[4][0] = walk;

		other_moves[1][0] = shoot;
		other_moves[3][0] = shoot;
		other_moves[2][1] = walk;
		other_moves[2][3] = walk;
		other_moves[2][4] = walk;

		owned_by_first_player = first_player;
		name = "Pikeman";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "P1";
		}
		return "P2";
	}
};

class seer : public figure {
public:
	seer(int x_, int y_, bool first_player) {
		starting_moves[2][0] = jump;
		starting_moves[0][2] = jump;
		starting_moves[4][2] = jump;
		starting_moves[2][4] = jump;

		starting_moves[1][1] = walk;
		starting_moves[1][3] = walk;
		starting_moves[3][1] = walk;
		starting_moves[3][3] = walk;

		other_moves[0][0] = jump;
		other_moves[4][0] = jump;
		other_moves[0][4] = jump;
		other_moves[4][4] = jump;

		other_moves[2][1] = walk;
		other_moves[1][2] = walk;
		other_moves[3][2] = walk;
		other_moves[2][3] = walk;


		owned_by_first_player = first_player;
		name = "Seer";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "S1";
		}
		return "S2";
	}
};

class ranger : public figure {
public:
	ranger(int x_, int y_, bool first_player) {
		starting_moves[2][1] = slide;
		starting_moves[2][3] = slide;
		
		starting_moves[0][1] = jump;
		starting_moves[1][0] = jump;
		starting_moves[3][0] = jump;
		starting_moves[4][1] = jump;

		other_moves[1][1] = slide;
		other_moves[3][1] = slide;

		other_moves[1][4] = jump;
		other_moves[3][4] = jump;

		owned_by_first_player = first_player;
		name = "Ranger";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "R1";
		}
		return "R2";
	}
};

class knight : public figure {
public:
	knight(int x_, int y_, bool first_player) {
		starting_moves[1][0] = jump;
		starting_moves[3][0] = jump;

		starting_moves[1][2] = walk;
		starting_moves[3][2] = walk;
		starting_moves[2][3] = walk;
		starting_moves[2][4] = walk;

		other_moves[2][1] = slide;

		other_moves[1][3] = walk;
		other_moves[0][4] = walk;
		other_moves[3][3] = walk;
		other_moves[4][4] = walk;

		owned_by_first_player = first_player;
		name = "Knight";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "K1";
		}
		return "K2";
	}
};

class bowman : public figure {
public:
	bowman(int x_, int y_, bool first_player) {
		starting_moves[2][1] = walk;
		starting_moves[1][2] = walk;
		starting_moves[3][2] = walk;

		starting_moves[0][2] = jump;
		starting_moves[4][2] = jump;
		starting_moves[2][4] = jump;

		other_moves[1][1] = shoot;
		other_moves[3][1] = shoot;
		other_moves[2][0] = shoot;

		other_moves[1][3] = walk;
		other_moves[3][3] = walk;

		owned_by_first_player = first_player;
		name = "Bowman";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "B1";
		}
		return "B2";
	}
};

class dragoon : public figure {
public:
	dragoon(int x_, int y_, bool first_player) {
		starting_moves[0][0] = shoot;
		starting_moves[2][0] = shoot;
		starting_moves[4][0] = shoot;

		starting_moves[1][2] = walk;
		starting_moves[3][2] = walk;

		other_moves[1][3] = slide;
		other_moves[3][3] = slide;

		other_moves[1][0] = jump;
		other_moves[3][0] = jump;

		other_moves[2][0] = walk;
		other_moves[2][1] = walk;

		owned_by_first_player = first_player;
		name = "Dragoon";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "X1";
		}
		return "X2";
	}
};

class footman : public figure {
public:
	footman(int x_, int y_, bool first_player) {
		other_moves[2][0] = walk;
		other_moves[1][1] = walk;
		other_moves[3][1] = walk;
		other_moves[1][3] = walk;
		other_moves[3][3] = walk;

		starting_moves[2][1] = walk;
		starting_moves[1][2] = walk;
		starting_moves[2][3] = walk;
		starting_moves[3][2] = walk;

		owned_by_first_player = first_player;
		name = "Footman";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "F1";
		}
		return "F2";
	}
};

class assassin : public figure {
public:
	assassin(int x_, int y_, bool first_player) {
		starting_moves[2][0] = jump_and_slide;
		starting_moves[0][4] = jump_and_slide;
		starting_moves[4][4] = jump_and_slide;

		other_moves[0][0] = jump_and_slide;
		other_moves[4][0] = jump_and_slide;
		other_moves[2][4] = jump_and_slide;

		owned_by_first_player = first_player;
		name = "Assassin";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "A1";
		}
		return "A2";
	}
};

class priest : public figure {
public:
	priest(int x_, int y_, bool first_player) {
		starting_moves[1][1] = slide;
		starting_moves[3][1] = slide;
		starting_moves[1][3] = slide;
		starting_moves[3][3] = slide;

		other_moves[1][1] = walk;
		other_moves[3][1] = walk;
		other_moves[1][3] = walk;
		other_moves[3][3] = walk;

		other_moves[0][0] = jump;
		other_moves[4][0] = jump;
		other_moves[0][4] = jump;
		other_moves[4][4] = jump;

		owned_by_first_player = first_player;
		name = "Priest";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "E1";
		}
		return "E2";
	}
};

class wizard : public figure {
public:
	wizard(int x_, int y_, bool first_player) {
		starting_moves[1][1] = walk;
		starting_moves[2][1] = walk;
		starting_moves[3][1] = walk;
		starting_moves[1][2] = walk;
		starting_moves[1][3] = walk;
		starting_moves[2][3] = walk;
		starting_moves[3][3] = walk;
		starting_moves[3][2] = walk;

		other_moves[0][0] = jump;
		other_moves[2][0] = jump;
		other_moves[4][0] = jump;
		other_moves[0][2] = jump;
		other_moves[0][4] = jump;
		other_moves[2][4] = jump;
		other_moves[4][4] = jump;
		other_moves[4][2] = jump;

		owned_by_first_player = first_player;
		name = "Wizard";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "W1";
		}
		return "W2";
	}
};


class marshall : public figure {
public:
	marshall(int x_, int y_, bool first_player) {
		starting_moves[0][0] = jump;
		starting_moves[4][0] = jump;
		starting_moves[2][4] = jump;

		starting_moves[1][2] = slide;
		starting_moves[3][2] = slide;

		other_moves[1][1] = walk_or_command;
		other_moves[2][1] = walk_or_command;
		other_moves[3][1] = walk_or_command;

		other_moves[1][2] = walk;
		other_moves[1][3] = walk;
		other_moves[0][2] = walk;
		other_moves[3][3] = walk;
		other_moves[3][2] = walk;
		other_moves[4][2] = walk;

		owned_by_first_player = first_player;
		command_squares_other.push_back(coordinates(1, 1));
		command_squares_other.push_back(coordinates(2, 1));
		command_squares_other.push_back(coordinates(3, 1));
		name = "Marshall";
	}
	std::string return_symbol() {
		if (owned_by_first_player) {
			return "M1";
		}
		return "M2";
	}
};


class champion : public figure {
public:
	champion(int x_, int y_, bool first_player) {
		starting_moves[2][1] = walk;
		starting_moves[1][2] = walk;
		starting_moves[2][3] = walk;
		starting_moves[3][2] = walk;

		starting_moves[2][0] = jump;
		starting_moves[0][2] = jump;
		starting_moves[4][2] = jump;
		starting_moves[2][4] = jump;

		other_moves[2][0] = jump;
		other_moves[2][4] = jump;
		other_moves[4][2] = jump;
		other_moves[0][2] = jump;

		other_moves[1][2] = shoot;
		other_moves[2][1] = shoot;
		other_moves[4][2] =	shoot;
		other_moves[2][4] = shoot;

		owned_by_first_player = first_player;
		name = "Champion";
	}

	std::string return_symbol() {
		if (owned_by_first_player) {
			return "C1";
		}
		return "C2";
	}
};

class general : public figure {
public:
	general(int x_, int y_, bool first_player) {
		starting_moves[2][1] = walk;
		starting_moves[0][2] = walk;
		starting_moves[3][2] = walk;
		starting_moves[2][3] = walk;

		starting_moves[1][0] = jump;
		starting_moves[3][0] = jump;

		other_moves[1][0] = jump;
		other_moves[3][0] = jump;

		other_moves[2][1] = walk;
		other_moves[0][2] = walk;
		other_moves[4][2] = walk;

		other_moves[1][2] = walk_or_command;
		other_moves[3][2] = walk_or_command;

		other_moves[1][3] = command;
		other_moves[2][3] = command;
		other_moves[3][3] = command;

		owned_by_first_player = first_player;
		command_squares_other.push_back(coordinates(1, 2)); 
		command_squares_other.push_back(coordinates(1, 3));
		command_squares_other.push_back(coordinates(2, 3));
		command_squares_other.push_back(coordinates(3, 3));
		command_squares_other.push_back(coordinates(3, 2));
		name = "General";
	}

	std::string return_symbol() {
		if (owned_by_first_player) {
			return "G1";
		}
		return "G2";
	}
};

class longbowman : public figure {
public:
	longbowman(int x_, int y_, bool first_player) {
		starting_moves[2][2] = walk;
		starting_moves[1][3] = walk;
		starting_moves[2][4] = walk;
		starting_moves[3][3] = walk;

		other_moves[2][0] = shoot;
		other_moves[2][1] = shoot;

		other_moves[1][4] = walk;
		other_moves[3][4] = walk;

		owned_by_first_player = first_player;
		position_on_move_board = coordinates(2, 3);
		name = "Longbowman";
	}

	std::string return_symbol() {
		if (owned_by_first_player) {
			return "L1";
		}
		return "L2";
	}
};
