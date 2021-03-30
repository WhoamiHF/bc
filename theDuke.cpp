// theDuke.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "figures.h"
#include "game.h"

void print_all_possible_moves(std::vector<possible_move> possible_moves) {
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
	game gameLogic;
	gameLogic.add_new_figure(coordinates(1, 1), Duke);
	gameLogic.add_new_figure(coordinates(1, 0), Marshall);
	gameLogic.move_troop(coordinates(1, 0), coordinates(2, 0));
	gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(coordinates(3, 1), Duke);
	gameLogic.add_new_figure(coordinates(2, 1), Pikeman);
	gameLogic.first_player_plays = true;
	gameLogic.print_board();
	std::vector<possible_move> possible_moves = std::vector<possible_move>();
	gameLogic.collect_all_possible_moves(possible_moves);
	print_all_possible_moves(possible_moves);
}

/* test same thing as test_1 and shoot with and without target*/
void test_2() {
	game gameLogic;
	gameLogic.add_new_figure(coordinates(3, 0), Duke);
	gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(coordinates(2, 3), Duke);
	gameLogic.add_new_figure(coordinates(3, 3), Longbowman);
	gameLogic.add_new_figure(coordinates(2, 4), Marshall);
	gameLogic.board[2][4]->starting_position = false;
	gameLogic.board[3][3]->starting_position = false;
	gameLogic.print_board();

	std::vector<possible_move> possible_moves = std::vector<possible_move>();
	gameLogic.collect_all_possible_moves(possible_moves);
	print_all_possible_moves(possible_moves);
}

/* Tests jump, jump and slide*/
void test_3() {
	game gameLogic;
	gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(coordinates(1, 2), Duke);
	gameLogic.add_new_figure(coordinates(1, 3), Footman);
	gameLogic.move_troop(coordinates(1, 3), coordinates(1, 4));
	gameLogic.first_player_plays = true;
	gameLogic.add_new_figure(coordinates(2, 2), Duke);
	gameLogic.add_new_figure(coordinates(3, 2), General);
	gameLogic.add_new_figure(coordinates(2, 3), Assassin);
	gameLogic.board[2][2]->starting_position = false;
	gameLogic.board[2][3]->starting_position = false;
	gameLogic.board[3][2]->starting_position = false;
	gameLogic.print_board();

	std::vector<possible_move> possible_moves = std::vector<possible_move>();
	gameLogic.collect_all_possible_moves(possible_moves);
	print_all_possible_moves(possible_moves);


	//std::cout << "eval:" << gameLogic.evaluate_move(possible_move(tmp, add_it),3);
}

int main()
{
	srand(time(NULL));
	//test_1();
	//test_2();
	//test_3();
	game gameLogic;
	gameLogic.play();
	//gameLogic.create_hash();
	//gameLogic.mark_winning_state();
	//gameLogic.mark_losing_state();
	//gameLogic.load_winning_states();
	//std::vector<coordinates> tmp = std::vector<coordinates>();
	

  //gameLogic.board[1][1]->print_boards(); //beware of null - function of gamelogic?
}

