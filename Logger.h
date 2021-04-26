#pragma once
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>  
#include <stdio.h>
#include <time.h>
#include <string>
#include "game.h"

class logger {
public:
	logger() {
		std::time_t t = std::time(0);
		int xx = t;
		std::string s = std::to_string(xx);
		file = std::ofstream(s + ".txt");
		std::cout << xx;
	}

	void log(move_t move);
	std::ofstream file;
};