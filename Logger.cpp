#include "Logger.h"

void logger::log(move_t move) {
	switch (move.op) {
	case add_it:
		file << "Add [" << move.coords[0].x << "," << move.coords[0].y << "] ";
		break;
	case move_it:
		file << "Move [" << move.coords[0].x << "," << move.coords[0].y << "] to [" << move.coords[1].x << "," << move.coords[1].y << "] ";
		break;
	case command_it:
		file << "Command [" << move.coords[0].x << "," << move.coords[0].y  << "] " << move.coords[1].x << "," << move.coords[1].y << "] to [" << move.coords[2].x << "," << move.coords[2].y << "] ";
		break;
	default:
		break;
	}

}