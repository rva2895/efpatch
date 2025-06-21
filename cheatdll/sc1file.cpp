#include "stdafx.h"
#include "sc1file.h"
#include "zlib.h"

#define SCEN_STEP 0x2000

void read_str32(str32* s, FILE* f)
{
	fread(&s->len, sizeof(s->len), 1, f);
	if (s->len > 0)
	{
		s->str = (char*)malloc(s->len);
		fread(s->str, s->len, 1, f);
	}
	else
		s->str = 0;
}

void read(void* dst, char** ptr, int size)
{
	memcpy(dst, *ptr, size);
	(*ptr) += size;
}

void read_str16(str16* s, char** ptr)
{
	s->len = *(unsigned short*)(*ptr);
	(*ptr) += sizeof(short);
	s->str = (char*)malloc(s->len + 1);
	memcpy(s->str, *ptr, s->len);
	s->str[s->len] = 0;
	(*ptr) += s->len;
}

void read_str32(str32* s, char** ptr)
{
	s->len = *(unsigned long*)(*ptr);
	(*ptr) += sizeof(long);
	s->str = (char*)malloc(s->len + 1);
	memcpy(s->str, *ptr, s->len);
	s->str[s->len] = 0;
	(*ptr) += s->len;
}

void read_trigger_value(long* dst, char** ptr, long* values_remaining)
{
	if (*values_remaining > 0)
	{
		read(dst, ptr, sizeof(long));
		(*values_remaining)--;
	}
	else
		*dst = -1;
}

SCEN::SCEN(const char* filename)
{
	loaded = false;
	last_error = SC_CONV_ERROR_CANNOT_OPEN_FILE;

	header.instructions.str = NULL;
	header2.filename.str = NULL;

	messages.instructions.str = NULL;
	messages.hints.str = NULL;
	messages.victory.str = NULL;
	messages.loss.str = NULL;
	messages.history.str = NULL;
	messages.scouts.str = NULL;
	messages.pregame_c.str = NULL;
	messages.victory_c.str = NULL;
	messages.loss_c.str = NULL;
	messages.background_c.str = NULL;

	messages.bitmap_data = NULL;

	for (int i = 0; i < 32; i++)
		player_data2.unknown_strings[i].str = NULL;
	for (int i = 0; i < 16; i++)
		player_data2.ai_names[i].str = NULL;
	for (int i = 0; i < 16; i++)
		player_data2.ai_file[i].file.str = NULL;

	map.x = 0;
	map.y = 0;
	map.terrain_data = NULL;
	units.unit_sections_n = 0;
	units.player_units = NULL;

	player_data3.number_of_players = 0;
	for (int i = 0; i < 8; i++)
	{
		player_data3.player_data3_sub[i].constant.str = NULL;
		player_data3.player_data3_sub[i].diplo1 = NULL;
		player_data3.player_data3_sub[i].diplo2 = NULL;
		player_data3.player_data3_sub[i].unk_char = NULL;
		player_data3.player_data3_sub[i].unknown_struct = NULL;
	}

	triggers.number_of_triggers = 0;
	triggers.triggers = NULL;
	triggers.trigger_order = NULL;

	tail.len = 0;
	tail.data = NULL;

	//

	FILE* f = fopen(filename, "rb");

	if (!f)
	{
#ifdef _DEBUG
		log("Error: Cannot open %s", filename);
#endif
		last_error = SC_CONV_ERROR_CANNOT_OPEN_FILE;
		return;
	}

#ifdef _DEBUG
	log("Loading %s...", filename);
#endif

	fread(&header.version, sizeof(header.version), 1, f);
	fread(&header.length, sizeof(header.length), 1, f);
	fread(&header.saveable, sizeof(header.saveable), 1, f);
	fread(&header.timestamp, sizeof(header.timestamp), 1, f);

	if (header.version != 0x31322E31)
	{
#ifdef _DEBUG
		log(" ERROR");
		char ver[5];
		*(int*)ver = header.version;
		log("Cannot load file (unknown ASCII header - expected: 1.21, file: %s)", ver);
#endif
		last_error = SC_CONV_ERROR_INVALID_SCENARIO;
		fclose(f);
		return;
	}

	read_str32(&header.instructions, f);
	fread(&header.individual_victories_used, sizeof(header.individual_victories_used), 1, f);
	fread(&header.players, sizeof(header.players), 1, f);

	z_stream_s st;
	memset(&st, 0, sizeof(st));
	//int r = inflateInit2_(&st, -15, "1.2.8", 56);
	//int r = inflateInit2_(&st, -15, "1.2.8", sizeof(st));

	int dst_n = SCEN_STEP;
	void* dst = malloc(SCEN_STEP);

	int start = ftell(f);
	fseek(f, 0, SEEK_END);
	int end = ftell(f);
	fseek(f, start, SEEK_SET);
	void* data = malloc(end - start);
	fread(data, end - start, 1, f);
	fclose(f);

	int r = inflateInit2(&st, -MAX_WBITS);

	st.avail_in = end - start;
	st.next_in = (Bytef*)data;
	st.avail_out = SCEN_STEP;
	st.next_out = (Bytef*)dst;

	while (r = inflate(&st, Z_NO_FLUSH), r != Z_STREAM_END)
	{
		switch (r)
		{
		case Z_OK:
			dst_n *= 2;
			dst = realloc(dst, dst_n);
			st.next_out = (Bytef*)((char*)dst + dst_n / 2);
			st.avail_out = dst_n / 2;
			break;
		case Z_BUF_ERROR:
			dst_n *= 2;
			dst = realloc(dst, dst_n);
			st.next_out = (Bytef*)dst;
			st.avail_out = dst_n;
			st.next_in = (Bytef*)data;
			st.avail_in = end - start;
			break;
		default:
			inflateEnd(&st);
			free(data);
			free(dst);
			return;
			break;
		}
	}

	int total = st.total_out;

	inflateEnd(&st);
	free(data);

	char* p = (char*)dst;

	read(&header2.nextID, &p, sizeof(header2.nextID));
	read(&header2.version2, &p, sizeof(header2.version2));
	read(&header2.player_names, &p, sizeof(header2.player_names));
	read(&header2.string_table_player_names, &p, sizeof(header2.string_table_player_names));
	for (int i = 0; i < 16; i++)
	{
		read(&header2.player_data1[i].active, &p, sizeof(header2.player_data1[i].active));
		read(&header2.player_data1[i].human, &p, sizeof(header2.player_data1[i].human));
		read(&header2.player_data1[i].civ, &p, sizeof(header2.player_data1[i].civ));
		read(&header2.player_data1[i].cty_mode, &p, sizeof(header2.player_data1[i].cty_mode));
	}
	read(&header2.unknown1, &p, sizeof(header2.unknown1));
	read(&header2.unknown_minus1, &p, sizeof(header2.unknown_minus1));
	read(&header2.unknownBF, &p, sizeof(header2.unknownBF));

	//log("Ver2 = %f", header2.version2);

	if ((header2.version2 != 1.30f) && (header2.version2 != 1.22f))
	{
#ifdef _DEBUG
		log("Error: incorrect format version (expected: 1.30 or 1.22, file version: %.2f", header2.version2);
#endif
		last_error = SC_CONV_ERROR_INVALID_SCENARIO;
		return;
	}

	read_str16(&header2.filename, &p);

	//messages
	read(&messages.instructionsID, &p, sizeof(messages.instructionsID));
	read(&messages.hintsID, &p, sizeof(messages.hintsID));
	read(&messages.victoryID, &p, sizeof(messages.victoryID));
	read(&messages.lossID, &p, sizeof(messages.lossID));
	read(&messages.historyID, &p, sizeof(messages.historyID));
	read(&messages.scoutsID, &p, sizeof(messages.scoutsID));
	read_str16(&messages.instructions, &p);
	read_str16(&messages.hints, &p);
	read_str16(&messages.victory, &p);
	read_str16(&messages.loss, &p);
	read_str16(&messages.history, &p);
	read_str16(&messages.scouts, &p);
	read_str16(&messages.pregame_c, &p);
	read_str16(&messages.victory_c, &p);
	read_str16(&messages.loss_c, &p);
	read_str16(&messages.background_c, &p);
	read(&messages.picture_version, &p, sizeof(messages.picture_version));
	read(&messages.bitmap_width, &p, sizeof(messages.bitmap_width));
	read(&messages.bitmap_height, &p, sizeof(messages.bitmap_height));
	read(&messages.picture_orientation, &p, sizeof(messages.picture_orientation));

	if (messages.bitmap_width > 0 && messages.bitmap_height > 0)
	{
		messages.bitmap_size = ((messages.bitmap_width + 3) & 0xFFFFFFFC) * messages.bitmap_height + 0x428;
		messages.bitmap_data = (char*)malloc(messages.bitmap_size);
		read(messages.bitmap_data, &p, messages.bitmap_size);
	}
	else
	{
		messages.bitmap_size = 0;
		messages.bitmap_data = NULL;
	}
	
	//player data 2
	for (int i = 0; i < 32; i++)
		read_str16(&player_data2.unknown_strings[i], &p);
	for (int i = 0; i < 16; i++)
		read_str16(&player_data2.ai_names[i], &p);
	for (int i = 0; i < 16; i++)
	{
		read(&player_data2.ai_file[i].unknown1, &p, sizeof(player_data2.ai_file[i].unknown1));
		read(&player_data2.ai_file[i].unknown2, &p, sizeof(player_data2.ai_file[i].unknown2));
		read_str32(&player_data2.ai_file[i].file, &p);
	}
	read(&player_data2.ai_type, &p, sizeof(player_data2.ai_type));
	read(&player_data2.separator, &p, sizeof(player_data2.separator));

	if (player_data2.separator != 0xFFFFFF9D)
	{
#ifdef _DEBUG
		log("Error: incorrect separator value (expected: 0xFFFFFF9D, file: 0x%X", player_data2.separator);
#endif
		free(dst);
		last_error = SC_CONV_ERROR_INVALID_SCENARIO;
		return;
	}

	for (int i = 0; i < 16; i++)
	{
		read(&player_data2.resources[i].food, &p, sizeof(player_data2.resources[i].food));
		read(&player_data2.resources[i].carbon, &p, sizeof(player_data2.resources[i].carbon));
		read(&player_data2.resources[i].ore, &p, sizeof(player_data2.resources[i].ore));
		read(&player_data2.resources[i].nova, &p, sizeof(player_data2.resources[i].nova));
		read(&player_data2.resources[i].orex, &p, sizeof(player_data2.resources[i].orex));
		read(&player_data2.resources[i].trade_goods, &p, sizeof(player_data2.resources[i].trade_goods));
	}

	//victory
	read(&victory.separator, &p, sizeof(victory.separator));
	read(&victory.conquest, &p, sizeof(victory.conquest));
	read(&victory.ruins, &p, sizeof(victory.ruins));
	read(&victory.relics, &p, sizeof(victory.relics));
	read(&victory.discovery, &p, sizeof(victory.discovery));
	read(&victory.explored, &p, sizeof(victory.explored));
	read(&victory.gold, &p, sizeof(victory.gold));
	read(&victory.custom, &p, sizeof(victory.custom));
	read(&victory.mode, &p, sizeof(victory.mode));
	read(&victory.score, &p, sizeof(victory.score));
	read(&victory.time, &p, sizeof(victory.time));

	//diplomacy
	read(&diplomacy.per_player_diplo, &p, sizeof(diplomacy.per_player_diplo));
	p += 11520;
	read(&diplomacy.separator, &p, sizeof(diplomacy.separator));
	read(&diplomacy.allied_victory, &p, sizeof(diplomacy.allied_victory));

	//disables
	//read(&disables, &p, sizeof(disables));
	read(&disables.techs_n, &p, sizeof(disables.techs_n));
	read(&disables.tech_ids, &p, sizeof(disables.tech_ids));
	if (header2.version2 == 1.30f)
		read(&disables.tech_ids_extra, &p, sizeof(disables.tech_ids_extra));
	read(&disables.units_n, &p, sizeof(disables.units_n));
	read(&disables.unit_ids, &p, sizeof(disables.unit_ids));
	if (header2.version2 == 1.30f)
		read(&disables.unit_ids_extra, &p, sizeof(disables.unit_ids_extra));
	read(&disables.bldgs_n, &p, sizeof(disables.bldgs_n));
	read(&disables.bldg_ids, &p, sizeof(disables.bldg_ids));
	if (header2.version2 == 1.30f)
		read(&disables.bldg_ids_extra, &p, sizeof(disables.bldg_ids_extra));
	read(&disables.combat_mode, &p, sizeof(disables.combat_mode));
	read(&disables.naval_mode, &p, sizeof(disables.naval_mode));
	read(&disables.all_techs, &p, sizeof(disables.all_techs));
	read(&disables.starting_age, &p, sizeof(disables.starting_age));

	//map
	read(&map.separator, &p, sizeof(map.separator));
	read(&map.camera_x, &p, sizeof(map.camera_x));
	read(&map.camera_y, &p, sizeof(map.camera_y));
	read(&map.ai_type, &p, sizeof(map.ai_type));
	read(&map.x, &p, sizeof(map.x));
	read(&map.y, &p, sizeof(map.y));

	map.terrain_data = (terrain_t*)malloc(map.x * map.y * sizeof(terrain_t));
	for (int i = 0; i < map.x * map.y; i++)
	{
		read(&map.terrain_data[i].id, &p, sizeof(map.terrain_data[i].id));
		read(&map.terrain_data[i].elevation, &p, sizeof(map.terrain_data[i].elevation));
		read(&map.terrain_data[i].unused, &p, sizeof(map.terrain_data[i].unused));
	}

	//units
	read(&units.unit_sections_n, &p, sizeof(units.unit_sections_n));
	read(&units.player_data, &p, sizeof(units.player_data));
	units.player_units = (player_units_t*)malloc(sizeof(player_units_t) * units.unit_sections_n);
	for (int i = 0; i < units.unit_sections_n; i++)
	{
		read(&units.player_units[i].unit_count, &p, sizeof(units.player_units[i].unit_count));
		units.player_units[i].units = (unit_t*)malloc(sizeof(unit_t) * units.player_units[i].unit_count);
		for (int j = 0; j < units.player_units[i].unit_count; j++)
		{
			read(&units.player_units[i].units[j].x, &p, sizeof(units.player_units[i].units[j].x));
			read(&units.player_units[i].units[j].y, &p, sizeof(units.player_units[i].units[j].y));
			read(&units.player_units[i].units[j].z, &p, sizeof(units.player_units[i].units[j].z));
			read(&units.player_units[i].units[j].object_id, &p, sizeof(units.player_units[i].units[j].object_id));
			read(&units.player_units[i].units[j].unit_id, &p, sizeof(units.player_units[i].units[j].unit_id));
			read(&units.player_units[i].units[j].state, &p, sizeof(units.player_units[i].units[j].state));
			read(&units.player_units[i].units[j].rotation, &p, sizeof(units.player_units[i].units[j].rotation));
			read(&units.player_units[i].units[j].frame, &p, sizeof(units.player_units[i].units[j].frame));
			read(&units.player_units[i].units[j].garrisoned_in, &p, sizeof(units.player_units[i].units[j].garrisoned_in));
		}
	}

	//player data 3
	read(&player_data3.number_of_players, &p, sizeof(player_data3.number_of_players));
	for (int i = 0; i < 8; i++)
	{
		read_str16(&player_data3.player_data3_sub[i].constant, &p);
		read(&player_data3.player_data3_sub[i].camera_x, &p, sizeof(player_data3.player_data3_sub[i].camera_x));
		read(&player_data3.player_data3_sub[i].camera_y, &p, sizeof(player_data3.player_data3_sub[i].camera_y));
		read(&player_data3.player_data3_sub[i].unknown1, &p, sizeof(player_data3.player_data3_sub[i].unknown1));
		read(&player_data3.player_data3_sub[i].unknown2, &p, sizeof(player_data3.player_data3_sub[i].unknown2));
		read(&player_data3.player_data3_sub[i].allied_victory, &p, sizeof(player_data3.player_data3_sub[i].allied_victory));
		read(&player_data3.player_data3_sub[i].player_count, &p, sizeof(player_data3.player_data3_sub[i].player_count));

		player_data3.player_data3_sub[i].diplo1 = (unsigned char*)malloc(player_data3.player_data3_sub[i].player_count);
		read(player_data3.player_data3_sub[i].diplo1, &p, player_data3.player_data3_sub[i].player_count);
		player_data3.player_data3_sub[i].diplo2 = (unsigned long*)malloc(sizeof(long) * player_data3.player_data3_sub[i].player_count);
		read(player_data3.player_data3_sub[i].diplo2, &p, sizeof(long) * player_data3.player_data3_sub[i].player_count);

		read(&player_data3.player_data3_sub[i].color, &p, sizeof(player_data3.player_data3_sub[i].color));
		read(&player_data3.player_data3_sub[i].victory_version, &p, sizeof(player_data3.player_data3_sub[i].victory_version));
		read(&player_data3.player_data3_sub[i].unknown_n, &p, sizeof(player_data3.player_data3_sub[i].unknown_n));
		if (player_data3.player_data3_sub[i].victory_version == 2.0f)
		{
			player_data3.player_data3_sub[i].unk_char = (unsigned char*)malloc(8);
			read(player_data3.player_data3_sub[i].unk_char, &p, 8);
		}
		else
			player_data3.player_data3_sub[i].unk_char = 0;
		if (player_data3.player_data3_sub[i].unknown_n > 0)
		{
			player_data3.player_data3_sub[i].unknown_struct = (char*)malloc(player_data3.player_data3_sub[i].unknown_n * 44);
			read(player_data3.player_data3_sub[i].unknown_struct, &p, player_data3.player_data3_sub[i].unknown_n * 44);
		}
		else
			player_data3.player_data3_sub[i].unknown_struct = 0;
		read(&player_data3.player_data3_sub[i].unknown_0, &p, 7);
		read(&player_data3.player_data3_sub[i].unknown_flag, &p, sizeof(player_data3.player_data3_sub[i].unknown_flag));
	}

	read(&player_data3.trigger_version, &p, sizeof(player_data3.trigger_version));

	//triggers
	read(&triggers.instructions_start, &p, sizeof(triggers.instructions_start));
	read(&triggers.number_of_triggers, &p, sizeof(triggers.number_of_triggers));
	if (triggers.number_of_triggers > 0)
	{
		triggers.triggers = (trigger_data_t*)malloc(triggers.number_of_triggers * sizeof(trigger_data_t));
		//trigger data
		for (int i = 0; i < triggers.number_of_triggers; i++)
		{
			read(&triggers.triggers[i].enabled, &p, sizeof(triggers.triggers[i].enabled));
			read(&triggers.triggers[i].looping, &p, sizeof(triggers.triggers[i].looping));
			read(&triggers.triggers[i].string_table_id, &p, sizeof(triggers.triggers[i].string_table_id));
			read(&triggers.triggers[i].objective, &p, sizeof(triggers.triggers[i].objective));
			read(&triggers.triggers[i].description_order, &p, sizeof(triggers.triggers[i].description_order));
			read(&triggers.triggers[i].starting_time, &p, sizeof(triggers.triggers[i].starting_time));
			read_str32(&triggers.triggers[i].description, &p);
			read_str32(&triggers.triggers[i].name, &p);
			//effects
			read(&triggers.triggers[i].n_effects, &p, sizeof(triggers.triggers[i].n_effects));
			if (triggers.triggers[i].n_effects > 0)
			{
				triggers.triggers[i].effects = (sc1effect*)malloc(triggers.triggers[i].n_effects * sizeof(sc1effect));
				for (int j = 0; j < triggers.triggers[i].n_effects; j++)
				{
					read(&triggers.triggers[i].effects[j].type, &p, sizeof(triggers.triggers[i].effects[j].type));

					read(&triggers.triggers[i].effects[j].values_saved, &p, sizeof(triggers.triggers[i].effects[j].values_saved));
					read_trigger_value(&triggers.triggers[i].effects[j].ai_goal, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].amount, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].resource, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].diplomacy, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].num_selected, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].location_unit, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].unit_id, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].player_source, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].player_target, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].technology, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].string_id, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].sound_id, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].display_time, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].index, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].location.x, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].location.y, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].area_ll.x, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].area_ll.y, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].area_ur.x, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].area_ur.y, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].unit_group, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].unit_type, &p, &triggers.triggers[i].effects[j].values_saved);
					read_trigger_value(&triggers.triggers[i].effects[j].instruction_panel, &p, &triggers.triggers[i].effects[j].values_saved);
					triggers.triggers[i].effects[j].values_saved = 23;

					read(&triggers.triggers[i].effects[j].text_len, &p, sizeof(triggers.triggers[i].effects[j].text_len));

					//read(&triggers.triggers[i].effects[j], &p, sizeof(effect) - 4 * sizeof(long));
					if (triggers.triggers[i].effects[j].text_len > 0)
					{
						triggers.triggers[i].effects[j].text = (char*)malloc(triggers.triggers[i].effects[j].text_len);
						read(triggers.triggers[i].effects[j].text, &p, triggers.triggers[i].effects[j].text_len);
					}
					else
						triggers.triggers[i].effects[j].text = NULL;
					read(&triggers.triggers[i].effects[j].sound_len, &p, sizeof(triggers.triggers[i].effects[j].sound_len));
					if (triggers.triggers[i].effects[j].sound_len > 0)
					{
						triggers.triggers[i].effects[j].sound_file = (char*)malloc(triggers.triggers[i].effects[j].sound_len);
						read(triggers.triggers[i].effects[j].sound_file, &p, triggers.triggers[i].effects[j].sound_len);
					}
					else
						triggers.triggers[i].effects[j].sound_file = NULL;
					if (triggers.triggers[i].effects[j].num_selected > 0)
					{
						triggers.triggers[i].effects[j].unit_ids = (long*)malloc(sizeof(long) * triggers.triggers[i].effects[j].num_selected);
						read(triggers.triggers[i].effects[j].unit_ids, &p, sizeof(long) * triggers.triggers[i].effects[j].num_selected);
					}
					else
						triggers.triggers[i].effects[j].unit_ids = NULL;
				}
				triggers.triggers[i].effect_order = (long*)malloc(triggers.triggers[i].n_effects * sizeof(long));
				read(triggers.triggers[i].effect_order, &p, triggers.triggers[i].n_effects * sizeof(long));
			}
			else
			{
				triggers.triggers[i].effects = NULL;
				triggers.triggers[i].effect_order = NULL;
			}
			//conditions
			read(&triggers.triggers[i].n_conditions, &p, sizeof(triggers.triggers[i].n_conditions));
			if (triggers.triggers[i].n_conditions > 0)
			{
				triggers.triggers[i].conditions = (sc1condition*)malloc(triggers.triggers[i].n_conditions * sizeof(sc1condition));
				for (int j = 0; j < triggers.triggers[i].n_conditions; j++)
				{
					read(&triggers.triggers[i].conditions[j].type, &p, sizeof(triggers.triggers[i].conditions[j].type));
					read(&triggers.triggers[i].conditions[j].values_saved, &p, sizeof(triggers.triggers[i].conditions[j].values_saved));

					read_trigger_value(&triggers.triggers[i].conditions[j].amount, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].resource, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].unit_object, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].unit_location, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].unit_id, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].player, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].technology, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].timer, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].unknown, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].area_ll.x, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].area_ll.y, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].area_ur.x, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].area_ur.y, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].unit_group, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].unit_type, &p, &triggers.triggers[i].conditions[j].values_saved);
					read_trigger_value(&triggers.triggers[i].conditions[j].ai_signal, &p, &triggers.triggers[i].conditions[j].values_saved);
					triggers.triggers[i].conditions[j].values_saved = 16;

				}
				triggers.triggers[i].condition_order = (long*)malloc(triggers.triggers[i].n_conditions * sizeof(long));
				read(triggers.triggers[i].condition_order, &p, triggers.triggers[i].n_conditions * sizeof(long));
			}
			else
			{
				triggers.triggers[i].conditions = NULL;
				triggers.triggers[i].condition_order = NULL;
			}
		}
		//trigger display order array
		triggers.trigger_order = (unsigned long*)malloc(triggers.number_of_triggers * sizeof(long));
		read(triggers.trigger_order, &p, triggers.number_of_triggers * sizeof(long));
	}
	else
	{
		triggers.triggers = NULL;
		triggers.trigger_order = NULL;
	}

	tail.len = total - (p - dst);
	tail.data = (char*)malloc(tail.len);
	read(tail.data, &p, tail.len);

	free(dst);

#ifdef _DEBUG
	log("OK");
#endif

	loaded = true;
	last_error = SC_CONV_ERROR_SUCCESS;
}

SCEN::~SCEN()
{
	free(header.instructions.str);
	free(header2.filename.str);

	free(messages.instructions.str);
	free(messages.hints.str);
	free(messages.victory.str);
	free(messages.loss.str);
	free(messages.history.str);
	free(messages.scouts.str);
	free(messages.pregame_c.str);
	free(messages.victory_c.str);
	free(messages.loss_c.str);
	free(messages.background_c.str);
	free(messages.bitmap_data);

	for (int i = 0; i < 32; i++)
		free(player_data2.unknown_strings[i].str);
	for (int i = 0; i < 16; i++)
		free(player_data2.ai_names[i].str);
	for (int i = 0; i < 16; i++)
		free(player_data2.ai_file[i].file.str);

	free(map.terrain_data);

	for (int i = 0; i < units.unit_sections_n; i++)
		free(units.player_units[i].units);
	free(units.player_units);

	for (int i = 0; i < 8; i++)
	{
		free(player_data3.player_data3_sub[i].constant.str);
		free(player_data3.player_data3_sub[i].diplo1);
		free(player_data3.player_data3_sub[i].diplo2);
		free(player_data3.player_data3_sub[i].unk_char);
		free(player_data3.player_data3_sub[i].unknown_struct);
	}

	for (int i = 0; i < triggers.number_of_triggers; i++)
	{
		free(triggers.triggers[i].description.str);
		free(triggers.triggers[i].name.str);
		for (int j = 0; j < triggers.triggers[i].n_effects; j++)
		{
			free(triggers.triggers[i].effects[j].text);
			free(triggers.triggers[i].effects[j].sound_file);
			free(triggers.triggers[i].effects[j].unit_ids);
		}
		free(triggers.triggers[i].effects);
		free(triggers.triggers[i].effect_order);
		free(triggers.triggers[i].conditions);
		free(triggers.triggers[i].condition_order);
	}
	free(triggers.triggers);
	free(triggers.trigger_order);

	free(tail.data);
}

typedef struct write_ptr_t
{
	char* p;		//current
	char* base;		//start
	int c;			//capacity
} write_ptr_t;

void write(void* src, write_ptr_t* p, int size)
{
	int offset = p->p - p->base;
	while (offset + size > p->c)
	{
		p->c *= 2;
		p->base = (char*)realloc(p->base, p->c);
		p->p = p->base + offset;
	}
	memcpy(p->p, src, size);
	p->p += size;
}

void write_str16(str16* s, write_ptr_t* p)
{
	write(&s->len, p, sizeof(short));
	write(s->str, p, s->len);
}

void write_str32(str32* s, write_ptr_t* p)
{
	write(&s->len, p, sizeof(long));
	write(s->str, p, s->len);
}

void write_str32(str32* s, FILE* f)
{
	fwrite(&s->len, sizeof(long), 1, f);
	fwrite(s->str, s->len, 1, f);
}
