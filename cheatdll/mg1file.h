#pragma once
#include "stdafx.h"

typedef struct _maptile
{
	char terrain;
	char elevation;
} TILE;

typedef struct _playerinfo
{
	char diplomacy_from[9];
	int diplomacy_to[9];
	int name_len;
	char* name;
	int civ;
	int color;
} PLAYERINFO;

typedef struct _team
{
	int n;
	char* names[8];
	char colors[8];
	char civs[8];
} TEAM;

class MG1
{
private:
	char* map_type;
public:
	bool loaded;
	bool hasAI;
	DWORD version;
	char* p;
	void readN(void*, int);
	int read4();
	short read2();
	char read1();
	void skip(int);

	TEAM getTeam1();
	TEAM getTeam2();
	DWORD getTeams();
	char* getMapType();
	char* getGameType();

	int getNAllies(int);
	bool allied(int, int);

	struct _data
	{
		int game_speed;
		int start_time;
		short my_player_number;
		int number_of_players;
		int map_x;
		int map_y;
		TILE* map; //array of tiles
		PLAYERINFO* players;
		int duration;
		int duration2;
		int pop_limit;
		int game_type;
	} d;
	MG1(const char*);
	~MG1();
};
