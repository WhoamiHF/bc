// theDuke.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "figures.h"
#include "game.h"
#include "Logger.h"

void print_all_possible_moves(std::vector<move_t> possible_moves) {
	for (auto&& item : possible_moves) {
		switch (item.op) {
		case add_it:
			std::cout << "add " << item.coords[0].x << " " << item.coords[0].y << std::endl;
			break;
		case move_it:
			std::cout << "move " << item.coords[0].x << " " << item.coords[0].y << ", " << item.coords[1].x << " " << item.coords[1].y << std::endl;
			break;
		case command_it:
			std::cout << "command " << item.coords[0].x << " " << item.coords[0].y << ", " << item.coords[1].x << " " << item.coords[1].y << ", " << item.coords[2].x << " " << item.coords[2].y << std::endl;
		}
	}
}

/* Tests command, slide and walk, all with and without obstacles
*/
void test_1() {
	precomputed moves = precomputed();
	game gameLogic = game(&moves.sheet_odd, &moves.sheet_even);
	gameLogic.add_new_figure(coordinates(1, 1), Duke,false);
	gameLogic.add_new_figure(coordinates(1, 0), Marshall,false);
	gameLogic.move_troop(coordinates(1, 0), coordinates(2, 0));
	gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(coordinates(3, 1), Duke,false);
	gameLogic.add_new_figure(coordinates(2, 1), Pikeman,false);
	gameLogic.first_player_plays = true;
	gameLogic.print_board();
	std::vector<move_t> possible_moves = std::vector<move_t>();
	gameLogic.collect_all_possible_moves(possible_moves);
	print_all_possible_moves(possible_moves);
}

/* test same thing as test_1 and shoot with and without target*/
void test_2() {
	precomputed moves = precomputed();
	game gameLogic = game(&moves.sheet_odd, &moves.sheet_even);
	gameLogic.add_new_figure(coordinates(3, 0), Duke,false);
	gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(coordinates(2, 3), Duke,false);
	gameLogic.add_new_figure(coordinates(3, 3), Longbowman,false);
	gameLogic.add_new_figure(coordinates(2, 4), Marshall,false);
	gameLogic.board[2][4]->starting_position = false;
	gameLogic.board[3][3]->starting_position = false;
	gameLogic.print_board();

	std::vector<move_t> possible_moves = std::vector<move_t>();
	gameLogic.collect_all_possible_moves(possible_moves);
	print_all_possible_moves(possible_moves);
}

/* Tests jump, jump and slide*/
void test_3() {
	precomputed moves = precomputed();
	game gameLogic = game(&moves.sheet_odd,&moves.sheet_even);
	gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(coordinates(1, 2), Duke,false);
	gameLogic.add_new_figure(coordinates(1, 3), Footman,false);
	gameLogic.move_troop(coordinates(1, 3), coordinates(1, 4));
	gameLogic.first_player_plays = true;
	gameLogic.add_new_figure(coordinates(2, 2), Duke,false);
	gameLogic.add_new_figure(coordinates(3, 2), General,false);
	gameLogic.add_new_figure(coordinates(2, 3), Assassin,false);
	gameLogic.board[2][2]->starting_position = false;
	gameLogic.board[2][3]->starting_position = false;
	gameLogic.board[3][2]->starting_position = false;
	gameLogic.print_board();

	std::vector<move_t> possible_moves = std::vector<move_t>();
	gameLogic.collect_all_possible_moves(possible_moves);
	print_all_possible_moves(possible_moves);


	//std::cout << "eval:" << gameLogic.evaluate_move(possible_move(tmp, add_it),3);
}

void test_4() {
	precomputed moves = precomputed();
	game gameLogic = game(&moves.sheet_odd, &moves.sheet_even);
	gameLogic.first_player_plays = true;
	gameLogic.add_new_figure(coordinates(2, 0), Duke, false);
	gameLogic.add_new_figure(coordinates(3, 0), Footman, true);
	gameLogic.add_new_figure(coordinates(1, 0), Footman, true);

	gameLogic.add_new_figure(coordinates(2, 1), Champion, true);
	gameLogic.board[2][1]->starting_position = !gameLogic.board[2][1]->starting_position;

	gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(coordinates(2, 5), Duke, false);
	gameLogic.add_new_figure(coordinates(1, 5), Footman, true);
	gameLogic.add_new_figure(coordinates(3, 5), Footman, false);
	gameLogic.add_new_figure(coordinates(2, 4), Ranger, true);
	gameLogic.first_player_plays = true;
	gameLogic.move_troop(coordinates(2, 1), coordinates(0, 1));
	gameLogic.print_board();
	gameLogic.print_packs();

	std::vector<move_t> possible_moves = std::vector<move_t>();
	gameLogic.collect_all_possible_moves(possible_moves);
	print_all_possible_moves(possible_moves);
}

/* testing collect all possible moves, correct implementation of mirroring*/
void test_5() {
	precomputed moves = precomputed();
	game gameLogic = game(&moves.sheet_odd, &moves.sheet_even);
	gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(coordinates(0, 0), Duke, false);
	gameLogic.add_new_figure(coordinates(1, 0), Pikeman, true);
	gameLogic.add_new_figure(coordinates(0, 1), Assassin, true);
	gameLogic.board[0][0]->starting_position = !gameLogic.board[0][0]->starting_position;
	gameLogic.print_board();
	gameLogic.print_packs(); 
	std::vector<move_t> possible_moves = std::vector<move_t>();
	gameLogic.collect_all_possible_moves(possible_moves);
	print_all_possible_moves(possible_moves);
}
int main()
{
	srand(time(NULL));
	//test_1();
	//test_2();
	//test_3();
	//test_4();
	//test_5();
	precomputed moves = precomputed();
	game gameLogic = game(&moves.sheet_odd,&moves.sheet_even);
	gameLogic.play();
	//gameLogic.create_hash();
	//gameLogic.mark_winning_state();
	//gameLogic.mark_losing_state();
	//gameLogic.load_winning_states();
	//std::vector<coordinates> tmp = std::vector<coordinates>();
	

  //gameLogic.board[1][1]->print_boards(); //beware of null - function of gamelogic?
}

