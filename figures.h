#pragma once
#include <iostream>
#include <array>
#include <memory>

enum types_of_moves { nothing, walk, slide, jump_and_slide, shoot, jump };

class cooardinates {
public:
	cooardinates(int _x, int _y, bool _valid) { x = _x; y = _y; valid = _valid; }
	int x;
	int y;
	bool valid;
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
	virtual std::string return_symbol()= 0;
	//bool is_slide_between(int x, int y);
	bool starting_position = true;
	types_of_moves starting_moves[5][5];
	types_of_moves other_moves[5][5];

	int position_x;
	int position_y;
	bool owned_by_first_player;
};



class duke : public figure {
public:
	duke(int x_,int y_, bool first_player) {
		for (size_t i = 0; i < 5; i++)
		{
			for (size_t j = 0; j < 5; j++)
			{
				starting_moves[i][j] = nothing;
				other_moves[i][j] = nothing;
			}
		}
		starting_moves[1][2] = slide;
		starting_moves[3][2] = slide;
		other_moves[2][1] = slide;
		other_moves[2][3] = slide;
		starting_moves[3][3] = slide;
	}
	std::string return_symbol() {
		return "D";
	}
};