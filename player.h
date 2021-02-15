#pragma once
#include <vector>

class game;

enum troop_name { Nothing, Duke, Ranger, Assassin, Wizard, General, Bowman, Champion, Knight, Marshall, Priest, Seer, Dragoon, Pikeman, Footman, Longbowman };

class troop {
public:
	troop(int x_, int y_, troop_name name_) {
		x = x_;
		y = y_;
		name = name_;
	}
	int x;
	int y;
	troop_name name;
};

class player {
public:
	player() = default;
	player(bool _first_player, game* game_logic_) {
		first_player = _first_player;
		active_troops = std::vector<troop>();
		pack_troops = std::vector<troop_name>{ Duke, Footman,Footman,Footman,Pikeman,Pikeman,Pikeman,Marshall,General,Longbowman,Priest,Ranger,Knight,Assassin,Dragoon,Champion,Wizard,Seer,Bowman };
		game_logic = game_logic_;
	}

	troop_name pick_random_pack_figure();
	bool deploy_troop(troop_name figure, int x, int y);
	void change_coordinates(int fx, int fy, int tx, int ty);

	void kill_troop(int x, int y);

	void print_symbol(troop_name name);
	void print_active();
	void print_pack();
	
	bool first_player;
	game* game_logic; //may be redundant
	std::vector<troop> active_troops;
	std::vector<troop_name> pack_troops;
};