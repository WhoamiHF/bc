#pragma once
#include <vector>
#include <string>

enum troop_name { Assassin, Bowman, Dragoon, Duke, Footman, General, Champion, Knight, Longbowman, Marshall, Nothing, Pikeman, Priest, Ranger, Seer, Wizard  };

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

class packs_t {
public:
	packs_t() {
		active = std::vector<troop>();
		backup = std::vector<troop_name>{ Duke, Footman,Footman,Footman,Pikeman,Pikeman,Pikeman,Marshall,General,Longbowman,Priest,Ranger,Knight,Assassin,Dragoon,Champion,Wizard,Seer,Bowman };
	}
	packs_t(std::vector<troop> active_,std::vector<troop_name> backup_) {
		active = active_;
		backup = backup_;
	}
	bool deploy_troop(troop_name figure, int x, int y);
	std::vector<troop> active;
	std::vector<troop_name> backup;
private:
	
};

class player {
public:
	//friend game;
	player() = default;
	player(bool _first_player, bool played_by_pc_) {
		first_player = _first_player;
		packs_t troop_packs = packs_t();
		played_by_pc = played_by_pc_;
	}

	troop_name pick_random_backup_figure();
	//bool deploy_troop(troop_name figure, int x, int y);
	void change_coordinates(int fx, int fy, int tx, int ty);

	void kill_troop(int x, int y);

	void print_symbol(troop_name name);
	void print_active();
	void print_backup();
	bool first_player;
	bool played_by_pc;
	void sort_active_pack();
	//bool compare_troops(const troop& first,const troop& second);
	packs_t packs;
};