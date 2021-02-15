#include "player.h"
#include <iostream>

/*	returns random name of figure which is in the pack
 * does NOT deploy -> for deployment is used function "deploy_troop"
*/
troop_name player::pick_random_pack_figure() {
	size_t size = pack_troops.size();

	if (size == 0) {
		return Nothing;
	}

	size_t index = rand() % size;
	troop_name new_unit = pack_troops[index];

	return new_unit;
}

/* checks if the troop is available
* it is called by game::add_new_figure which checks for emptiness of square
* then it deployes troop - removes from pack and adds to active troops.
* caller is responsible for putting the troop on the board - this function simply updates player's packs
*/
bool player::deploy_troop(troop_name figure, int x, int y) {
	if (x < 0 || x>5 || y < 0 || y>5) {
		return false;
	}

	auto iterator = std::find(pack_troops.begin(), pack_troops.end(), figure);
	if (iterator != pack_troops.end()) {
		active_troops.push_back(troop(x, y, *iterator));
		pack_troops.erase(iterator);
		return true;
	}
	else {
		return false;
		std::cout << "not found";
	}
}

/** called by game::remove_figure()
*	removes troop from player's active troops (ergo killing it)
*	caller is responsible for removing the troop from the board - this function simply updates player's active pack
*/
void player::kill_troop(int x, int y) {
	for (auto it = active_troops.begin(); it != active_troops.end(); it++) {
		if (it->x == x && it->y == y) {
			active_troops.erase(it);
			break;
		}
	}
}

/* Just a big switch on troops name for printing active/reserve pack
*/
void player::print_symbol(troop_name name) {
	switch (name) {
	case Duke:
		std::cout << "Duke ";
		break;
	case Ranger:
		std::cout << "Ranger ";
		break;
	case Assassin:
		std::cout << "Assassin ";
		break;
	case Pikeman:
		std::cout << "Pikeman ";
		break;
	case Marshall:
		std::cout << "Marshall ";
		break;
	case General:
		std::cout << "General ";
		break;
	case Knight:
		std::cout << "Knight ";
		break;
	case Longbowman:
		std::cout << "Longbowman ";
		break;
	case Champion:
		std::cout << "Champion ";
		break;
	case Wizard:
		std::cout << "Wizard ";
		break;
	case Seer:
		std::cout << "Seer ";
		break;
	case Footman:
		std::cout << "Footman ";
		break;
	case Priest:
		std::cout << "Priest ";
		break;
	case Bowman:
		std::cout << "Bowman ";
		break;
	case Dragoon:
		std::cout << "Dragoon ";
		break;
	default:
		std::cout << "Other ";
		break;
	}
}

/* As name suggests it prints names and coordinates of active troops*/
void player::print_active() {
	for (const auto& figure : active_troops) {
		print_symbol(figure.name);
		std::cout << "[" << figure.x << "," << figure.y << "] ";
	}
	std::cout << std::endl;
}

/* As name suggests it prints names (pack troops don't have coordinates) of pack troops*/
void player::print_pack() {
	for (const auto& figure : pack_troops) {
		print_symbol(figure);
	}
	std::cout << std::endl;
}

/* Called from game::move_troop, marks change of coordinates after move
	Because it's called after moving caller is responsible for checking legality of the move 	
*/
void player::change_coordinates(int fx, int fy, int tx, int ty) {
	for (auto& figure : active_troops) {
		if (figure.x == fx && figure.y == fy) {
			figure.x = tx;
			figure.y = ty;
			break;
		}	
	}
}