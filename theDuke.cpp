// theDuke.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "figures.h"
#include "game.h"

int main()
{
    srand(time(NULL));

    game gameLogic;
    gameLogic.add_new_figure(5,0,Wizard);
    gameLogic.add_new_figure(3, 4, Marshall);
    gameLogic.add_new_figure(2, 1, Duke);
    gameLogic.add_new_figure(3, 0, Pikeman);
    gameLogic.first_player_plays = false;
    gameLogic.add_new_figure(0, 0, Duke);
    gameLogic.add_new_figure(3, 3, Pikeman);
    gameLogic.add_new_figure(4, 2, Pikeman);
    gameLogic.add_new_figure(4, 3, Marshall);
    gameLogic.print_board();
    gameLogic.print_packs();
    gameLogic.board[4][3]->starting_position = false;
    gameLogic.check_command(4, 3, 4, 2, 5, 2);
    gameLogic.move_troop(3, 3, 4, 2);
    gameLogic.move_troop(0, 0, 7, 0);
    gameLogic.print_board();
    gameLogic.print_packs();
   // gameLogic.move_troop
   // gameLogic.first_player_plays = true;
    gameLogic.move_troop(2,1,1,1);
    gameLogic.print_board();
    gameLogic.print_packs();
    //gameLogic.board[3][3]->print_boards();
   // gameLogic.board[1][1]->starting_position = false;
    //gameLogic.board[1][1]->print_boards(); //beware of null

}

