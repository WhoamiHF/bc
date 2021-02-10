// theDuke.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "figures.h"
#include "game.h"

int main()
{
    game gameLogic;
    gameLogic.add_new_figure(4,4,Duke);
    gameLogic.add_new_figure(3, 4, Duke);
    gameLogic.add_new_figure(2, 4, Duke);
    gameLogic.print_board();

    gameLogic.move_troop(2,4,5,4); //remove first to parameters x check it
    gameLogic.print_board();
   /* if (gameLogic.board[0][0]->starting_position) {
        std::cout << "Hello World!\n";
    }*/
}

