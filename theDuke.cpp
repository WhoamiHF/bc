// theDuke.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "figures.h"
#include "game.h"

void test_1() {
	game gameLogic;
	gameLogic.add_new_figure(2, 1, Duke);
	gameLogic.add_new_figure(2, 2, Marshall);
	gameLogic.print_board();
	gameLogic.collect_all_possible_moves();
	gameLogic.move_troop(coordinates(2, 2),coordinates( 1, 3));
}

void test_2() {
	game gameLogic;
	gameLogic.add_new_figure(3, 0, Duke);
	gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(2, 2, Duke);
	gameLogic.add_new_figure(3, 2, Longbowman);
	gameLogic.add_new_figure(2, 3, Marshall);
	gameLogic.board[2][3]->starting_position = false;
	gameLogic.board[3][2]->starting_position = false;
	gameLogic.print_board();
	gameLogic.print_packs();
	gameLogic.collect_all_possible_moves();
	//gameLogic.command_troop(2, 3, 2, 2, 1, 2); //check null
	gameLogic.print_board();
	std::cout << gameLogic.possible_moves.size();
}

void test_3() {
	game gameLogic;
	gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(3, 0, Duke);
	gameLogic.first_player_plays = true;
	gameLogic.add_new_figure(2, 2, Duke);
	gameLogic.add_new_figure(3, 2, Longbowman);
	gameLogic.add_new_figure(2, 3, Marshall);
	gameLogic.board[2][3]->starting_position = false;
	gameLogic.board[3][2]->starting_position = false;
	gameLogic.print_board();
	gameLogic.print_packs();
	gameLogic.collect_all_possible_moves();
	std::vector<coordinates> tmp = std::vector<coordinates>{ coordinates(2,1) };
	std::cout << "eval:" << gameLogic.evaluate_move(possible_move(tmp, add_it));
	//gameLogic.command_troop(2, 3, 2, 2, 1, 2); //check null
	gameLogic.print_board();
	std::cout << gameLogic.possible_moves.size();
}

int main()
{
	srand(time(NULL));
	//test_1();
	//test_2();
	test_3();
	/*game gameLogic;
	gameLogic.place_starting_troops();
	gameLogic.play();
	gameLogic.create_hash();
	gameLogic.mark_winning_state();
	gameLogic.mark_losing_state();
	gameLogic.load_winning_states();
	std::vector<coordinates> tmp = std::vector<coordinates>();*/
	

  //gameLogic.board[1][1]->print_boards(); //beware of null - function of gamelogic?
}

