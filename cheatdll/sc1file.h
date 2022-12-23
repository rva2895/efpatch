#pragma once

typedef struct str16
{
	unsigned short len;
	char* str;
} str16;

typedef struct str32
{
	unsigned long len;
	char* str;
} str32;

typedef struct ai_file_t
{
	unsigned long unknown1;
	unsigned long unknown2;
	str32 file;
} ai_file_t;

typedef struct terrain_t
{
	unsigned char id;
	unsigned char elevation;
	unsigned char unused;
} terrain_t;

typedef struct unit_t
{
	float x;
	float y;
	float z;
	unsigned long object_id;
	unsigned short unit_id;
	unsigned char state;
	float rotation;
	unsigned short frame;
	unsigned long garrisoned_in;
} unit_t;

typedef struct player_units_t
{
	unsigned long unit_count;
	unit_t* units;

} player_units_t;

struct POINT2
{
	long x, y;
};

struct sc1effect
{
	long type;	//see constants below
	long check;	//always = 0x17. For internal use by AOK (and AOKTS).
	long ai_goal;	//AI Script goal
	long amount;	//used for resources, hp, attack
	long resource;	//resource type (stone = 2)
	long diplomacy;	//state for change diplomacy (see constants below)
	long num_selected;	//number of units selected
	long location_unit;	//Unit ID for location setting
	long unit_id;	//unit constant for create object, etc.
	long player_source;
	long player_target;
	long technology;	//see "Complete Constant Lists" in AOKH utilities
	long string_id;	//as in string table
	long sound_id;
	long display_time;	//for Display Instructions
	long index;	//trigger index for Activate/Deactivate Trigger
	POINT2 location;
	POINT2 area_ll;	//lower-left corner of area
	POINT2 area_ur;	//upper-right corner
	long unit_group;
	long unit_type;	//Civilian, Military, Building, Other
	long instruction_panel;
	long text_len;
	char* text;	//Instructions/Chat
	long sound_len;
	char* sound_file;
	long* unit_ids;
};

struct sc1condition
{
	long type;	//see constants below
	long check;	//always = 0x10 (for internal use)
	long amount;	//of objects, difficult level
	long resource;	//resource type (see constants below)
	long unit_object;
	long unit_location;
	long unit_id;
	long player;
	long technology;	//see "Complete Constant Lists" in AOKH utilities
	long timer;
	long unknown;
	POINT2 area_ll;
	POINT2 area_ur;
	long unit_group;	//see constants below
	long unit_type;	//Civilian, Military, Building, Other
	long ai_signal;
};

typedef struct trigger_data_t
{
	unsigned long enabled;
	unsigned long looping;
	unsigned char string_table_id;
	unsigned char objective;
	unsigned long description_order;
	unsigned long starting_time;
	str32 description;
	str32 name;
	long n_effects;
	sc1effect* effects;
	long* effect_order;
	long n_conditions;
	sc1condition* conditions;
	long* condition_order;
} trigger_data_t;

class SCEN
{
public:
	bool loaded;
	int last_error;
	struct header_t
	{
		unsigned long version;
		unsigned long length;
		long saveable;
		unsigned long timestamp;
		str32 instructions;
		unsigned long individual_victories_used;
		unsigned long players;
	} header;
	struct header2_t
	{
		unsigned long nextID;
		float version2;
		char player_names[16 * 256];
		long string_table_player_names[16];
		struct player_data1_t
		{
			unsigned long active;
			unsigned long human;
			unsigned long civ;
			unsigned long cty_mode;
		} player_data1[16];
		unsigned long unknown1;
		unsigned long unknown_minus1;
		unsigned char unknownBF;
		str16 filename;
	} header2;
	struct messages_t
	{
		unsigned long instructionsID;
		unsigned long hintsID;
		unsigned long victoryID;
		unsigned long lossID;
		unsigned long historyID;
		unsigned long scoutsID;
		str16 instructions;
		str16 hints;
		str16 victory;
		str16 loss;
		str16 history;
		str16 scouts;
		str16 pregame_c;
		str16 victory_c;
		str16 loss_c;
		str16 background_c;
		unsigned long picture_version;
		unsigned long bitmap_width;
		long bitmap_height;
		short picture_orientation;
		//char bitmapinfoheader[40];
		long bitmap_size;
		char* bitmap_data;
	} messages;
	struct player_data2_t
	{
		str16 unknown_strings[32];
		str16 ai_names[16];
		ai_file_t ai_file[16];
		unsigned char ai_type[16];
		unsigned long separator;
		struct resources_t
		{
			unsigned long food;
			unsigned long carbon;
			unsigned long ore;
			unsigned long nova;
			unsigned long orex;
			unsigned long trade_goods;
		} resources[16];
	} player_data2;
	struct victory_t
	{
		unsigned long separator;
		unsigned long conquest;
		unsigned long ruins;
		unsigned long relics;
		unsigned long discovery;
		unsigned long explored;
		unsigned long gold;
		unsigned long custom;
		unsigned long mode;
		unsigned long score;
		unsigned long time;
	} victory;
	struct diplomacy_t
	{
		char per_player_diplo[16 * 64];
		// 11520 zero bytes here
		unsigned long separator;
		unsigned long allied_victory[16];
	} diplomacy;
	struct disables_t
	{
		unsigned long techs_n[16];
		long tech_ids[16 * 30];
		long tech_ids_extra[16 * 30];
		unsigned long units_n[16];
		long unit_ids[16 * 30];
		long unit_ids_extra[16 * 30];
		unsigned long bldgs_n[16];
		long bldg_ids[16 * 20];
		long bldg_ids_extra[16 * 40];
		unsigned long combat_mode;
		unsigned long naval_mode;
		unsigned long all_techs;
		unsigned long starting_age[16];
	} disables;
	struct map_t
	{
		unsigned long separator;
		long camera_x;
		long camera_y;
		long ai_type;
		unsigned long x;
		unsigned long y;
		terrain_t* terrain_data;
	} map;
	struct units_t
	{
		unsigned long unit_sections_n;
		char player_data[8 * 24];
		player_units_t* player_units;
	} units;
	struct player_data3_t
	{
		unsigned long number_of_players;
		struct player_data3_sub_t
		{
			str16 constant;
			float camera_x;
			float camera_y;
			short unknown1;
			short unknown2;
			unsigned char allied_victory;
			unsigned short player_count;
			unsigned char* diplo1;
			unsigned long* diplo2;
			unsigned long color;
			float victory_version;
			unsigned short unknown_n;
			unsigned char* unk_char; //only if unknownf == 2.0f
			char* unknown_struct; //N*44
			unsigned char unknown_0[7];
			long unknown_flag;
		} player_data3_sub[8];
		double trigger_version;
	} player_data3;
	struct triggers_t
	{
		char instructions_start;
		long number_of_triggers;
		trigger_data_t* triggers;
		unsigned long* trigger_order;
	} triggers;
	/*struct files_t
	{
		unsigned long included;
		unsigned long es_only_data_flag;
		char* es_only_data; //396 bytes
		char* files;
	} files;*/
	struct tail_t
	{
		long len;
		char* data;
	} tail;

	SCEN(const char* filename);
	~SCEN();


	void convert_unit_ids();
	bool save(const char* filename);
	bool isLoaded() { return loaded; }
	int get_last_error() { return last_error; }

	/*
#ifndef RELEASE_DLL
	int convert();
	int extract_files(const char* dir);
	int extract_files_c(const char* dir);
	int write_xml(const char* filename);
#endif
*/
};

#define SC_CONV_ERROR_SUCCESS 0
#define SC_CONV_ERROR_CANNOT_OPEN_FILE 1
#define SC_CONV_ERROR_CANNOT_CREATE_FILE 2
#define SC_CONV_ERROR_INVALID_SCENARIO 3
