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
	gameLogic.move_troop(2, 2, 1, 3);
}

void test_2() {
	game gameLogic;
	//gameLogic.add_new_figure(1, 2, Duke);
	//gameLogic.first_player_plays = false;
	gameLogic.add_new_figure(2, 2, Duke);
	gameLogic.add_new_figure(3, 2, Pikeman);
	gameLogic.add_new_figure(2, 1, Marshall);
	gameLogic.board[2][1]->starting_position = false;
	gameLogic.print_board();
	gameLogic.collect_all_possible_moves();
	gameLogic.command_troop(2, 1, 2, 2, 1, 2); //check null
	gameLogic.print_board();
	std::cout << gameLogic.possible_moves.size();
}

int main()
{
	srand(time(NULL));
	//test_1();
	//test_2();
	game gameLogic;
	gameLogic.place_starting_troops();
	gameLogic.play();

  //gameLogic.board[1][1]->print_boards(); //beware of null - function of gamelogic?
}

