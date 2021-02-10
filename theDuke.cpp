// theDuke.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "figures.h"
#include "game.h"

int main()
{
    game gameLogic;
    gameLogic.add_new_figure(1,1,Duke);
    /*duke bDuke = duke(4,4,true);
    duke wDuke = duke(3,4,false);
    duke qDuke = duke(2,4,false);
   
    gameLogic.board[3][4] = &wDuke;
    gameLogic.board[2][4] = &qDuke;*/
    gameLogic.print_board();
    gameLogic.check_move(4,4,0,4,gameLogic.board,true); //remove first to parameters x check it
   /* if (gameLogic.board[0][0]->starting_position) {
        std::cout << "Hello World!\n";
    }*/
}

