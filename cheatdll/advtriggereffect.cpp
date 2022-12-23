#include "stdafx.h"

//#include <regex>
#include <map>
#include "effects.h"
#include "advtriggereffect.h"

const CHANGE_UNIT_MASTER_PARAMS change_master_params[] =
{
    {10, offsetof(RGE_Master_Static_Object, vfptr),                     T_FORBIDDEN,    false,  {"vfptr"}},
    {10, offsetof(RGE_Master_Static_Object, master_type),               T_FORBIDDEN,    true,   {"Type", "master_type"}},
    {10, offsetof(RGE_Master_Static_Object, name),                      T_FORBIDDEN,    false,  {"InternalName", "name"}},
    {10, offsetof(RGE_Master_Static_Object, gbg_name2),                 T_FORBIDDEN,    false,  {"InternalName2", "gbg_name2"}},
    {10, offsetof(RGE_Master_Static_Object, unit_line),                 T_INT16,        false,  {"Unitline", "unit_line"}},
    {10, offsetof(RGE_Master_Static_Object, min_tech_level),            T_UINT8,        false,  {"MinTechLevel", "min_tech_level"}},
    {10, offsetof(RGE_Master_Static_Object, string_id),                 T_INT16,        false,  {"LanguageDLLName", "string_id"}},
    {10, offsetof(RGE_Master_Static_Object, string_id2),                T_INT16,        false,  {"LanguageDLLCreation", "string_id2"}},
    {10, offsetof(RGE_Master_Static_Object, id),                        T_INT16,        true,   {"ID1", "id"}},
    {10, offsetof(RGE_Master_Static_Object, copy_id),                   T_INT16,        true,   {"ID2", "copy_id"}},
    {10, offsetof(RGE_Master_Static_Object, base_id),                   T_INT16,        true,   {"ID3", "base_id"}},
    {10, offsetof(RGE_Master_Static_Object, object_group),              T_INT16,        true,   {"Class", "object_group"}},
    {10, offsetof(RGE_Master_Static_Object, sprite),                    T_SPRITE_PTR,   false,  {"StandingGraphics", "sprite"}},
    {10, offsetof(RGE_Master_Static_Object, sprite2),                   T_SPRITE_PTR,   false,  {"StandingGraphics2", "sprite2"}},
    {10, offsetof(RGE_Master_Static_Object, death_sprite),              T_SPRITE_PTR,   false,  {"DyingGraphics", "death_sprite"}},
    {10, offsetof(RGE_Master_Static_Object, undead_sprite),             T_SPRITE_PTR,   false,  {"UndeadGraphics", "undead_sprite"}},
    {10, offsetof(RGE_Master_Static_Object, undead_flag),               T_UINT8,        false,  {"DeathMode", "UndeadMode", "undead_flag"}},
    {10, offsetof(RGE_Master_Static_Object, hp),                        T_INT16,        true,   {"HitPoints", "hp"}},
    {10, offsetof(RGE_Master_Static_Object, los),                       T_FLOAT,        true,   {"LineOfSight", "los"}},
    {10, offsetof(RGE_Master_Static_Object, obj_max),                   T_UINT8,        true,   {"GarrisonCapacity", "obj_max"}},
    {10, offsetof(RGE_Master_Static_Object, radius_x),                  T_FLOAT,        true,   {"CollisionSizeX", "radius_x"}},
    {10, offsetof(RGE_Master_Static_Object, radius_y),                  T_FLOAT,        true,   {"CollisionSizeY", "radius_y"}},
    {10, offsetof(RGE_Master_Static_Object, radius_z),                  T_FLOAT,        false,  {"CollisionSizeZ", "radius_z"}},
    {10, offsetof(RGE_Master_Static_Object, selected_sound),            T_SOUND_PTR,    false,  {"SelectionSound", "selected_sound"}},
    {10, offsetof(RGE_Master_Static_Object, created_sound),             T_SOUND_PTR,    false,  {"TrainSound", "created_sound"}},
    {10, offsetof(RGE_Master_Static_Object, death_sound),               T_SOUND_PTR,    false,  {"DyingSound", "death_sound"}},
    {10, offsetof(RGE_Master_Static_Object, damage_sound),              T_SOUND_PTR,    false,  {"DamageSound", "damage_sound"}},
    {10, offsetof(RGE_Master_Static_Object, death_spawn_obj_id),        T_INT16,        false,  {"DeadUnitID", "death_spawn_obj_id"}},
    {10, offsetof(RGE_Master_Static_Object, sort_number),               T_UINT8,        false,  {"PlacementMode", "SortNumber", "sort_number"}},
    {10, offsetof(RGE_Master_Static_Object, can_be_built_on),           T_UINT8,        false,  {"AirMode", "CanBeBuiltOn", "can_be_built_on"}},
    {10, offsetof(RGE_Master_Static_Object, button_pict),               T_INT16,        false,  {"IconID", "button_pict"}},
    {10, offsetof(RGE_Master_Static_Object, hide_in_scenario_editor),   T_UINT8,        true,   {"HideInEditor", "hide_in_scenario_editor"}},
    {10, offsetof(RGE_Master_Static_Object, portrait_pict),             T_INT16,        false,  {"Portrait", "portrait_pict"}},
    {10, offsetof(RGE_Master_Static_Object, available),                 T_UINT8,        true,   {"Available", "available"}},
    {10, offsetof(RGE_Master_Static_Object, disabled),                  T_UINT8,        true,   {"Disabled", "disabled"}},
    {10, offsetof(RGE_Master_Static_Object, tile_req1),                 T_INT16,        false,  {"PlacementSideTerrain1", "tile_req1"}},
    {10, offsetof(RGE_Master_Static_Object, tile_req2),                 T_INT16,        false,  {"PlacementSideTerrain2", "tile_req2"}},
    {10, offsetof(RGE_Master_Static_Object, center_tile_req1),          T_INT16,        false,  {"PlacementTerrain1", "center_tile_req1"}},
    {10, offsetof(RGE_Master_Static_Object, center_tile_req2),          T_INT16,        false,  {"PlacementTerrain2", "center_tile_req2"}},
    {10, offsetof(RGE_Master_Static_Object, construction_radius_x),     T_FLOAT,        false,  {"ClearanceSizeX", "construction_radius_x"}},
    {10, offsetof(RGE_Master_Static_Object, construction_radius_y),     T_FLOAT,        false,  {"ClearanceSizeY", "construction_radius_y"}},
    {10, offsetof(RGE_Master_Static_Object, elevation_flag),            T_UINT8,        false,  {"HillMode", "elevation_flag"}},
    {10, offsetof(RGE_Master_Static_Object, fog_flag),                  T_UINT8,        false,  {"FogVisibility", "fog_flag"}},
    {10, offsetof(RGE_Master_Static_Object, terrain),                   T_INT16,        false,  {"TerrainRestriction", "terrain"}},
    {10, offsetof(RGE_Master_Static_Object, movement_type),             T_UINT8,        false,  {"FlyMode", "movement_type"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_type_held[0]),    T_INT16,        false,  {"ResourceStorageType1", "attribute_type_held1"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_type_held[1]),    T_INT16,        false,  {"ResourceStorageType2", "attribute_type_held2"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_type_held[2]),    T_INT16,        false,  {"ResourceStorageType3", "attribute_type_held3"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_amount_held[0]),  T_FLOAT,        true,   {"Resource1Storage", "attribute_amount_held1"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_amount_held[1]),  T_FLOAT,        false,  {"Resource2Storage", "attribute_amount_held2"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_amount_held[2]),  T_FLOAT,        false,  {"Resource3Storage", "attribute_amount_held3"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_max_amount),      T_INT16,        true,   {"ResourceCapacity", "attribute_max_amount"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_rot),             T_FLOAT,        false,  {"ResourceDecay", "attribute_rot"}},
    {10, offsetof(RGE_Master_Static_Object, multiple_attribute_mod),    T_FLOAT,        false,  {"ResourceDecay2", "multiple_attribute_mod"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_flag[0]),         T_UINT8,        false,  {"Resource1StoreMode", "attribute_flag1"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_flag[1]),         T_UINT8,        false,  {"Resource2StoreMode", "attribute_flag2"}},
    {10, offsetof(RGE_Master_Static_Object, attribute_flag[2]),         T_UINT8,        false,  {"Resource3StoreMode", "attribute_flag3"}},
    {10, offsetof(RGE_Master_Static_Object, area_effect_object_level),  T_UINT8,        false,  {"BlastDefenseLevel", "area_effect_object_level"}},
    {10, offsetof(RGE_Master_Static_Object, combat_level),              T_UINT8,        false,  {"CombatLevel", "combat_level"}},
    {10, offsetof(RGE_Master_Static_Object, select_level),              T_UINT8,        false,  {"InteractionMode", "select_level"}},
    {10, offsetof(RGE_Master_Static_Object, map_draw_level),            T_UINT8,        false,  {"MinimapMode", "map_draw_level"}},
    {10, offsetof(RGE_Master_Static_Object, unit_level),                T_UINT8,        false,  {"InterfaceKind", "unit_level"}},
    {10, offsetof(RGE_Master_Static_Object, map_color),                 T_UINT8,        false,  {"MinimapColor", "map_color"}},
    {10, offsetof(RGE_Master_Static_Object, attack_reaction),           T_UINT8,        false,  {"AttackMode", "attack_reaction"}},
    {10, offsetof(RGE_Master_Static_Object, convert_terrain_flag),      T_UINT8,        false,  {"ConvertTerrain", "convert_terrain_flag"}},
    {10, offsetof(RGE_Master_Static_Object, damage_sprite_num),         T_FORBIDDEN,    false,  {"damage_sprite_num"}},
    {10, offsetof(RGE_Master_Static_Object, damage_sprites),            T_FORBIDDEN,    false,  {"damage_sprites"}},
    {10, offsetof(RGE_Master_Static_Object, occlusion_flag),            T_UINT8,        false,  {"SelectionMask", "OcclusionMode", "occlusion_flag"}},
    {10, offsetof(RGE_Master_Static_Object, obstruction_type),          T_UINT8,        false,  {"SelectionShapeType", "ObstructionType", "obstruction_type"}},
    {10, offsetof(RGE_Master_Static_Object, obstruction_class),         T_UINT8,        false,  {"SelectionShape", "BlockageClass", "obstruction_class"}},
    {10, offsetof(RGE_Master_Static_Object, object_flags),              T_UINT8,        false,  {"UnitAttribute", "object_flags1"}},
    {10, offsetof(RGE_Master_Static_Object, object_flags)+1,            T_UINT8,        false,  {"Civilization", "object_flags2"}},
    {10, offsetof(RGE_Master_Static_Object, object_flags)+2,            T_INT16,        false,  {"AttributeLeftover", "object_flags3"}},
    {10, offsetof(RGE_Master_Static_Object, help_string_id),            T_INT32,        false,  {"LanguageDLLHelp", "help_string_id"}},
    {10, offsetof(RGE_Master_Static_Object, help_page_id),              T_INT32,        false,  {"LanguageDLLHotKeyText", "help_page_id"}},
    {10, offsetof(RGE_Master_Static_Object, hotkey_id),                 T_INT32,        true,   {"LanguageDLLHotKey", "hotkey_id"}},
    {10, offsetof(RGE_Master_Static_Object, recyclable),                T_UINT8,        false,  {"Unselectable", "Recyclable", "recyclable"}},
    {10, offsetof(RGE_Master_Static_Object, track_as_resource),         T_UINT8,        false,  {"EnableAutoGather", "CanBeGathered", "track_as_resource"}},
    {10, offsetof(RGE_Master_Static_Object, create_doppleganger_on_death),T_UINT8,      false,  {"AutoGatherMode", "create_doppleganger_on_death"}},
    {10, offsetof(RGE_Master_Static_Object, resource_group),            T_UINT8,        false,  {"AutoGatherID", "resource_group"}},
    {10, offsetof(RGE_Master_Static_Object, draw_flag),                 T_UINT8,        false,  {"SelectionEffect", "draw_flag"}},
    {10, offsetof(RGE_Master_Static_Object, draw_color),                T_UINT8,        false,  {"SelectionColor", "draw_color"}},
    {10, offsetof(RGE_Master_Static_Object, outline_radius_x),          T_FLOAT,        false,  {"SelectionShapeSizeX", "outline_radius_x"}},
    {10, offsetof(RGE_Master_Static_Object, outline_radius_y),          T_FLOAT,        false,  {"SelectionShapeSizeY", "outline_radius_y"}},
    {10, offsetof(RGE_Master_Static_Object, outline_radius_z),          T_FLOAT,        false,  {"SelectionShapeSizeZ", "outline_radius_z"}},

    {20, offsetof(RGE_Master_Animated_Object, speed),                   T_FLOAT,        true,   {"Speed", "speed"}},

    {30, offsetof(RGE_Master_Moving_Object, move_sprite),               T_SPRITE_PTR,   false,  {"WalkingGraphics", "move_sprite"}},
    {30, offsetof(RGE_Master_Moving_Object, run_sprite),                T_SPRITE_PTR,   false,  {"WalkingGraphics2", "run_sprite"}},
    {30, offsetof(RGE_Master_Moving_Object, turn_speed),                T_FLOAT,        true,   {"RotationSpeed", "turn_speed"}},
    {30, offsetof(RGE_Master_Moving_Object, size_class),                T_UINT8,        false,  {"SizeClass", "size_class"}},
    {30, offsetof(RGE_Master_Moving_Object, obj_trail_id),              T_INT16,        false,  {"TrackingUnit", "obj_trail_id"}},
    {30, offsetof(RGE_Master_Moving_Object, obj_trail_options),         T_UINT8,        false,  {"TrackingUnitUsed", "obj_trail_options"}},
    {30, offsetof(RGE_Master_Moving_Object, obj_trail_spacing),         T_FLOAT,        false,  {"TrackingUnitDensity", "obj_trail_spacing"}},
    {30, offsetof(RGE_Master_Moving_Object, move_algorithem),           T_UINT8,        false,  {"MoveAlgorithm", "move_algorithem"}},
    {30, offsetof(RGE_Master_Moving_Object, mTurnRadius),               T_FLOAT,        false,  {"TurnRadius", "mTurnRadius"}},
    {30, offsetof(RGE_Master_Moving_Object, mTurnRadiusSpeed),          T_FLOAT,        false,  {"TurnRadiusSpeed", "mTurnRadiusSpeed"}},
    {30, offsetof(RGE_Master_Moving_Object, mMaximumYawPerSecondMoving),T_FLOAT,        false,  {"MaxYPSMoving", "mMaximumYawPerSecondMoving"}},
    {30, offsetof(RGE_Master_Moving_Object, mStationaryYawRevolutionTime),T_FLOAT,      false,  {"YawRoundTime", "mStationaryYawRevolutionTime"}},
    {30, offsetof(RGE_Master_Moving_Object, mMaximumYawPerSecondStationary),T_FLOAT,    false,  {"MaxYPSStationary", "mMaximumYawPerSecondStationary"}},

    {40, offsetof(RGE_Master_Action_Object, tasks),                     T_FORBIDDEN,    false,  {"tasks"}},
    {40, offsetof(RGE_Master_Action_Object, default_task),              T_INT16,        false,  {"DefaultTask", "default_task"}},
    {40, offsetof(RGE_Master_Action_Object, search_radius),             T_FLOAT,        true,   {"SearchRadius", "search_radius"}},
    {40, offsetof(RGE_Master_Action_Object, work_rate),                 T_FLOAT,        true,   {"WorkRate1", "work_rate"}},
    {40, offsetof(RGE_Master_Action_Object, gbg_work_rate_2),           T_FLOAT,        false,  {"WorkRate2", "gbg_work_rate_2"}},
    {40, offsetof(RGE_Master_Action_Object, drop_site),                 T_INT16,        false,  {"DropSite1", "drop_site"}},
    {40, offsetof(RGE_Master_Action_Object, backup_drop_site),          T_INT16,        false,  {"DropSite2", "backup_drop_site"}},
    {40, offsetof(RGE_Master_Action_Object, task_by_group),             T_UINT8,        false,  {"TaskSwapID", "task_by_group"}},
    {40, offsetof(RGE_Master_Action_Object, command_sound),             T_SOUND_PTR,    false,  {"AttackSound", "command_sound"}},
    {40, offsetof(RGE_Master_Action_Object, move_sound),                T_SOUND_PTR,    false,  {"MoveSound", "move_sound"}},
    {40, offsetof(RGE_Master_Action_Object, run_pattern),               T_UINT8,        false,  {"AnimalMode", "RunPattern", "run_pattern"}},

    {50, offsetof(RGE_Master_Combat_Object, run_pattern),               T_SPRITE_PTR,   false,  {"AttackGraphics", "fight_sprite"}},
    {50, offsetof(RGE_Master_Combat_Object, base_armor),                T_INT16,        true,   {"DefaultArmor", "base_armor"}},
    {50, offsetof(RGE_Master_Combat_Object, armor_num),                 T_FORBIDDEN,    true,   {"armor_num"}},
    {50, offsetof(RGE_Master_Combat_Object, armor),                     T_ARMOR_CLASS,  true,   {"Armor", "armor"}},
    {50, offsetof(RGE_Master_Combat_Object, weapon_num),                T_FORBIDDEN,    true,   {"weapon_num"}},
    {50, offsetof(RGE_Master_Combat_Object, weapon),                    T_ARMOR_CLASS,  true,   {"Attack", "weapon"}},
    {50, offsetof(RGE_Master_Combat_Object, defense_terrain_bonus),     T_INT16,        true,   {"TerrainDefenseBonus", "defense_terrain_bonus"}},
    {50, offsetof(RGE_Master_Combat_Object, weapon_range),              T_FLOAT,        true,   {"Range", "weapon_range"}},
    {50, offsetof(RGE_Master_Combat_Object, area_effect_range),         T_FLOAT,        true,   {"BlastRadius", "area_effect_range"}},
    {50, offsetof(RGE_Master_Combat_Object, area_effect_level),         T_UINT8,        false,  {"BlastLevel", "area_effect_level"}},
    {50, offsetof(RGE_Master_Combat_Object, speed_of_attack),           T_FLOAT,        true,   {"ReloadTime1", "speed_of_attack"}},
    {50, offsetof(RGE_Master_Combat_Object, missile_id),                T_INT16,        true,   {"ProjectileUnitID", "missile_id"}},
    {50, offsetof(RGE_Master_Combat_Object, base_hit_chance),           T_INT16,        true,   {"Accuracy", "base_hit_chance"}},
    {50, offsetof(RGE_Master_Combat_Object, break_off_combat),          T_UINT8,        false,  {"TowerMode", "BreakOffCombat", "break_off_combat"}},
    {50, offsetof(RGE_Master_Combat_Object, fire_missile_at_frame),     T_INT16,        false,  {"FrameDelay", "fire_missile_at_frame"}},
    {50, offsetof(RGE_Master_Combat_Object, weapon_offset_x),           T_FLOAT,        false,  {"ProjectileGraphicDisplacementX", "weapon_offset_x"}},
    {50, offsetof(RGE_Master_Combat_Object, weapon_offset_y),           T_FLOAT,        false,  {"ProjectileGraphicDisplacementY", "weapon_offset_y"}},
    {50, offsetof(RGE_Master_Combat_Object, weapon_offset_z),           T_FLOAT,        false,  {"ProjectileGraphicDisplacementZ", "weapon_offset_z"}},
    {50, offsetof(RGE_Master_Combat_Object, minimum_weapon_range),      T_FLOAT,        true,   {"MinRange", "minimum_weapon_range"}},
    {50, offsetof(RGE_Master_Combat_Object, orig_armor),                T_INT16,        false,  {"DisplayedMeleeArmor", "orig_armor"}},
    {50, offsetof(RGE_Master_Combat_Object, orig_weapon),               T_INT16,        false,  {"DisplayedAttack", "orig_weapon"}},
    {50, offsetof(RGE_Master_Combat_Object, orig_weapon_range),         T_FLOAT,        false,  {"DisplayedRange", "orig_weapon_range"}},
    {50, offsetof(RGE_Master_Combat_Object, orig_speed_of_attack),      T_FLOAT,        false,  {"ReloadTime2", "orig_speed_of_attack"}},
    {50, offsetof(RGE_Master_Combat_Object, missed_missile_spread),     T_FLOAT,        false,  {"AccuracyErrorRadius", "missed_missile_spread"}},

    {60, offsetof(RGE_Master_Missile_Object, missile_type),             T_UINT8,        false,  {"ProjectileType", "missile_type"}},
    {60, offsetof(RGE_Master_Missile_Object, targetting_type),          T_UINT8,        true,   {"SmartMode", "targetting_type"}},
    {60, offsetof(RGE_Master_Missile_Object, missile_hit_info),         T_UINT8,        false,  {"HitMode", "missile_hit_info"}},
    {60, offsetof(RGE_Master_Missile_Object, missile_die_info),         T_UINT8,        false,  {"VanishMode", "missile_die_info"}},
    {60, offsetof(RGE_Master_Missile_Object, area_effect_specials),     T_UINT8,        false,  {"AreaEffects", "area_effect_specials"}},
    {60, offsetof(RGE_Master_Missile_Object, ballistics_ratio),         T_UINT8,        false,  {"ProjectileArc", "ballistics_ratio"}},

    {70, offsetof(TRIBE_Master_Combat_Object, build_inventory[0].type), T_INT16,        true,   {"Cost1Type", "build_inventory1type"}},
    {70, offsetof(TRIBE_Master_Combat_Object, build_inventory[0].amount),T_INT16,       true,   {"Cost1Amount", "build_inventory1amount"}},
    {70, offsetof(TRIBE_Master_Combat_Object, build_inventory[0].flag), T_UINT8,        true,   {"Cost1IsPaid", "build_inventory1flag"}},
    {70, offsetof(TRIBE_Master_Combat_Object, build_inventory[1].type), T_INT16,        true,   {"Cost2Type", "build_inventory2type"}},
    {70, offsetof(TRIBE_Master_Combat_Object, build_inventory[1].amount),T_INT16,       true,   {"Cost2Amount", "build_inventory2amount"}},
    {70, offsetof(TRIBE_Master_Combat_Object, build_inventory[1].flag), T_UINT8,        true,   {"Cost2IsPaid", "build_inventory2flag"}},
    {70, offsetof(TRIBE_Master_Combat_Object, build_inventory[2].type), T_INT16,        true,   {"Cost3Type", "build_inventory3type"}},
    {70, offsetof(TRIBE_Master_Combat_Object, build_inventory[2].amount),T_INT16,       true,   {"Cost3Amount", "build_inventory3amount"}},
    {70, offsetof(TRIBE_Master_Combat_Object, build_inventory[2].flag), T_UINT8,        true,   {"Cost3IsPaid", "build_inventory3flag"}},
    {70, offsetof(TRIBE_Master_Combat_Object, build_pts_required),      T_INT16,        true,   {"TrainTime", "build_pts_required"}},
    {70, offsetof(TRIBE_Master_Combat_Object, id_of_building_obj),      T_INT16,        false,  {"TrainLocation", "id_of_building_obj"}},
    {70, offsetof(TRIBE_Master_Combat_Object, button_location),         T_UINT8,        false,  {"TrainButton", "button_location"}},
    {70, offsetof(TRIBE_Master_Combat_Object, orig_pierce_armor),       T_INT16,        false,  {"DisplayedPierceArmor", "orig_pierce_armor"}},
    {70, offsetof(TRIBE_Master_Combat_Object, rear_attack_modifier),    T_FLOAT,        false,  {"BackstabBonus", "rear_attack_modifier"}},
    {70, offsetof(TRIBE_Master_Combat_Object, flank_attack_modifier),   T_FLOAT,        false,  {"FlankBonus", "flank_attack_modifier"}},
    {70, offsetof(TRIBE_Master_Combat_Object, tribe_unit_type),         T_UINT8,        false,  {"CreatableType", "tribe_unit_type"}},
    {70, offsetof(TRIBE_Master_Combat_Object, volley_fire_amount),      T_FLOAT,        true,   {"MinDuplMissiles", "volley_fire_amount"}},
    {70, offsetof(TRIBE_Master_Combat_Object, max_attacks_in_volley),   T_UINT8,        true,   {"MaxDuplMissiles", "max_attacks_in_volley"}},
    {70, offsetof(TRIBE_Master_Combat_Object, volley_x_spread),         T_FLOAT,        false,  {"MissileSpawningAreaWidth", "volley_x_spread"}},
    {70, offsetof(TRIBE_Master_Combat_Object, volley_y_spread),         T_FLOAT,        false,  {"MissileSpawningAreaLength", "volley_y_spread"}},
    {70, offsetof(TRIBE_Master_Combat_Object, volley_start_spread_adjustment),T_FLOAT,  false,  {"MissileSpawningRandomness", "volley_start_spread_adjustment"}},
    {70, offsetof(TRIBE_Master_Combat_Object, volley_missile_id),       T_INT32,        false,  {"MissileDuplUnit", "volley_missile_id"}},
    {70, offsetof(TRIBE_Master_Combat_Object, special_attack_sprite_id),T_INT32,        false,  {"SpecialGraphics", "special_attack_sprite_id"}},
    {70, offsetof(TRIBE_Master_Combat_Object, special_attack_flag),     T_UINT8,        false,  {"SpecialAbility", "special_attack_flag"}},
    {70, offsetof(TRIBE_Master_Combat_Object, hero_flag),               T_UINT8,        false,  {"HeroMode", "hero_flag"}},
    {70, offsetof(TRIBE_Master_Combat_Object, garrison_sprite),         T_SPRITE_PTR,   false,  {"GarrisonGraphics", "garrison_sprite"}},

    {80, offsetof(TRIBE_Master_Building_Object, construction_sound),    T_SOUND_PTR,    false,  {"ConstructionSound", "construction_sound"}},
    {80, offsetof(TRIBE_Master_Building_Object, construction_sprite),   T_SPRITE_PTR,   false,  {"ConstructionGraphics", "construction_sprite"}},
    {80, offsetof(TRIBE_Master_Building_Object, building_connect_flag), T_UINT8,        false,  {"AdjacentMode", "building_connect_flag"}},
    {80, offsetof(TRIBE_Master_Building_Object, building_facet),        T_INT16,        true,   {"GraphicsAngle", "building_facet"}},
    {80, offsetof(TRIBE_Master_Building_Object, build_and_go_away),     T_UINT8,        false,  {"DisappearsWhenBuilt", "build_and_go_away"}},
    {80, offsetof(TRIBE_Master_Building_Object, on_build_make_obj),     T_INT16,        false,  {"StackUnitID", "StackUnit", "on_build_make_obj"}},
    {80, offsetof(TRIBE_Master_Building_Object, on_build_make_tile),    T_INT16,        false,  {"FoundationTerrainID", "on_build_make_tile"}},
    {80, offsetof(TRIBE_Master_Building_Object, on_build_make_overlay), T_INT16,        false,  {"OldTerrainLikeID", "on_build_make_overlay"}},
    {80, offsetof(TRIBE_Master_Building_Object, on_build_make_tech),    T_INT16,        false,  {"ResearchID", "on_build_make_tech"}},
    {80, offsetof(TRIBE_Master_Building_Object, can_burn),              T_UINT8,        false,  {"CanBurn", "can_burn"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_id[0]),          T_INT16,        false,  {"AnnexUnit1", "linked_id1"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_id[1]),          T_INT16,        false,  {"AnnexUnit2", "linked_id2"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_id[2]),          T_INT16,        false,  {"AnnexUnit3", "linked_id3"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_id[3]),          T_INT16,        false,  {"AnnexUnit4", "linked_id4"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_x[0]),           T_FLOAT,        false,  {"AnnexMisplacementX1", "linked_x1"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_x[1]),           T_FLOAT,        false,  {"AnnexMisplacementX2", "linked_x2"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_x[2]),           T_FLOAT,        false,  {"AnnexMisplacementX3", "linked_x3"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_x[3]),           T_FLOAT,        false,  {"AnnexMisplacementX4", "linked_x4"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_y[0]),           T_FLOAT,        false,  {"AnnexMisplacementY1", "linked_y1"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_y[1]),           T_FLOAT,        false,  {"AnnexMisplacementY2", "linked_y2"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_y[2]),           T_FLOAT,        false,  {"AnnexMisplacementY3", "linked_y3"}},
    {80, offsetof(TRIBE_Master_Building_Object, linked_y[3]),           T_FLOAT,        false,  {"AnnexMisplacementY4", "linked_y4"}},
    {80, offsetof(TRIBE_Master_Building_Object, construction_id),       T_INT16,        false,  {"HeadUnit", "construction_id"}},
    {80, offsetof(TRIBE_Master_Building_Object, open_close_id),         T_INT16,        false,  {"TransformUnit", "open_close_id"}},
    {80, offsetof(TRIBE_Master_Building_Object, open_close_sound),      T_SOUND_PTR,    false,  {"TransformSound", "open_close_sound"}},
    {80, offsetof(TRIBE_Master_Building_Object, garrison_units),        T_UINT8,        false,  {"GarrisonType", "garrison_units"}},
    {80, offsetof(TRIBE_Master_Building_Object, garrison_heal_rate),    T_FLOAT,        false,  {"GarrisonHealRate", "garrison_heal_rate"}},
    {80, offsetof(TRIBE_Master_Building_Object, garrison_repair_rate),  T_FLOAT,        false,  {"GarrisonRepairRate", "garrison_repair_rate"}},
    {80, offsetof(TRIBE_Master_Building_Object, salvage_id),            T_INT16,        false,  {"PileUnit", "salvage_id"}},
    {80, offsetof(TRIBE_Master_Building_Object, pillage_attr[0]),       T_UINT8,        false,  {"LootingTable1", "pillage_attr1"}},
    {80, offsetof(TRIBE_Master_Building_Object, pillage_attr[1]),       T_UINT8,        false,  {"LootingTable2", "pillage_attr2"}},
    {80, offsetof(TRIBE_Master_Building_Object, pillage_attr[2]),       T_UINT8,        false,  {"LootingTable3", "pillage_attr3"}},
    {80, offsetof(TRIBE_Master_Building_Object, pillage_attr[3]),       T_UINT8,        false,  {"LootingTable4", "pillage_attr4"}},
    {80, offsetof(TRIBE_Master_Building_Object, pillage_attr[4]),       T_UINT8,        false,  {"LootingTable5", "pillage_attr5"}},
    {80, offsetof(TRIBE_Master_Building_Object, pillage_attr[5]),       T_UINT8,        false,  {"LootingTable6", "pillage_attr6"}}
};

std::map<const std::string, const CHANGE_UNIT_MASTER_PARAMS&> unit_master_lookup_table;

void build_unit_master_variable_lookup_table()
{
    for (int i = 0; i < _countof(change_master_params); i++)
    {
        for (size_t j = 0; j < change_master_params[i].val_names.size(); j++)
            unit_master_lookup_table.emplace(std::pair<const std::string, const CHANGE_UNIT_MASTER_PARAMS&>(change_master_params[i].val_names[j], change_master_params[i]));
    }
}

/*void editUnitData(void* unitData, int i, int action)
{

}

bool __stdcall advTriggerEffectActual2(void* unitData, char* str)
{
    std::regex r("^(.+) (.+) ([0-9]+ )?([-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)$",
        std::regex_constants::icase);

    std::smatch h;
    std::string s(str);
    int action;
    if (std::regex_match(s, h, r))
    {
        std::ssub_match h_sub = h[1];
        std::string command = h_sub.str();
        if (command == "SET")
            action = 0;
        else if (command == "ADD")
            action = 1;
        else if (command == "MUL")
            action = 2;
        else
            return false;

        h_sub = h[2];
        std::string variable = h_sub.str();
        char type;
        int i = getArrayIndex(variable.c_str(), &type);
        if (i == -1)
        {
            if (variable == "Attack")
            {
                //todo
            }
            else if (variable == "Armor")
            {
                //todo
            }
            else
                return false;
            h_sub = h[3];
            std::string cls_s = h_sub.str();
            int cls;
            sscanf_s(cls_s.c_str(), "%d", &cls);

        }
        else
            editUnitData(unitData, i, action);
        return true;
    }
    else
        return false;
}*/

void modify_object_sprite(RGE_Static_Object* obj, RGE_Sprite* old_sprite, RGE_Sprite* new_sprite)
{
    if (obj->sprite == old_sprite)
        obj->vfptr->new_sprite(obj, new_sprite);
}

void modify_sprite(RGE_Master_Static_Object* master, RGE_Static_Object* obj, RGE_Player* player, RGE_Sprite** old_sprite, short new_sprite_id)
{
    if (new_sprite_id < (*base_game)->world->sprite_num)
    {
        RGE_Sprite* new_sprite = (*base_game)->world->sprites[new_sprite_id];
        if (obj)
            modify_object_sprite(obj, *old_sprite, new_sprite);
        else
        {
            for (int i = 0; i < player->objects->Number_of_objects; i++)
                if (player->objects->List[i]->master_obj == master)
                    modify_object_sprite(player->objects->List[i], *old_sprite, new_sprite);
        }
        *old_sprite = new_sprite;
    }
}

void modify_sound(RGE_Sound** old_sound, short new_sound_id)
{
    if (new_sound_id < (*base_game)->world->sound_num)
    {
        RGE_Sound* new_sound = (*base_game)->world->sounds[new_sound_id];
        *old_sound = new_sound;
    }
}

void modify_value(void* address, size_t offset, UNIT_MASTER_DATA_OPERATION op, float v)
{
    switch (op)
    {
    case OP_SET:
        *(float*)((size_t)address + offset) = v;
        break;
    case OP_ADD:
        *(float*)((size_t)address + offset) += v;
        break;
    case OP_SUB:
        *(float*)((size_t)address + offset) -= v;
        break;
    case OP_MUL:
        *(float*)((size_t)address + offset) *= v;
        break;
    case OP_DIV:
        *(float*)((size_t)address + offset) /= v;
        break;
    case OP_POW:
        *(float*)((size_t)address + offset) = (float)pow(*(float*)((size_t)address + offset), v);
        break;
    default:
        log("Error: logical operations are not allowed for floats");
        break;
    }
}

template<class T>
void modify_value(void* address, size_t offset, UNIT_MASTER_DATA_OPERATION op, T v)
{
    switch (op)
    {
    case OP_SET:
        *(T*)((size_t)address + offset) = v;
        break;
    case OP_ADD:
        *(T*)((size_t)address + offset) += v;
        break;
    case OP_SUB:
        *(T*)((size_t)address + offset) -= v;
        break;
    case OP_MUL:
        *(T*)((size_t)address + offset) *= v;
        break;
    case OP_DIV:
        *(T*)((size_t)address + offset) /= v;
        break;
    case OP_AND:
        *(T*)((size_t)address + offset) &= v;
        break;
    case OP_OR:
        *(T*)((size_t)address + offset) |= v;
        break;
    case OP_POW:
        *(T*)((size_t)address + offset) = (T)pow(*(T*)((size_t)address + offset), v);
        break;
    }
}

void modify_armor_class(RGE_Master_Static_Object* master, size_t offset, UNIT_MASTER_DATA_OPERATION op, __int16 type, __int16 value)
{
    RGE_Armor_Weapon_Info** ptr = (RGE_Armor_Weapon_Info**)((size_t)master + offset);
    size_t count_offset = offset == 0x134 ? 0x132 : 0x138;
    __int16* count = (__int16*)((size_t)master + count_offset);

    RGE_Armor_Weapon_Info* type_ptr = NULL;

    //find class
    for (int i = 0; i < *count; i++)
        if ((*ptr)[i].type == type)
            type_ptr = &(*ptr)[i];

    if (!type_ptr)
    {
        RGE_Armor_Weapon_Info* new_ptr = (RGE_Armor_Weapon_Info*)calloc_internal(sizeof(RGE_Armor_Weapon_Info), *count + 1);
        memcpy(new_ptr, *ptr, sizeof(RGE_Armor_Weapon_Info) * *count);
        new_ptr[*count].type = type;
        new_ptr[*count].value = 0;
        free_internal(*ptr);
        *ptr = new_ptr;
        type_ptr = &new_ptr[*count];
        (*count)++;
    }

    modify_value(&type_ptr->value, 0, op, value);
}

bool is_master_field_in_this_master(const CHANGE_UNIT_MASTER_PARAMS& param, unsigned __int8 master_type)
{
    switch (master_type)
    {
    case 10:
        return param.master_type == 10;
    case 15:
        return param.master_type == 10 || param.master_type == 15;
    case 20:
        return (param.master_type <= master_type) && param.master_type != 15;
    case 25:
        return param.master_type == 10 || param.master_type == 20 || param.master_type == 25;
    case 30:
    case 40:
    case 50:
    case 60:
        return (param.master_type <= master_type) && param.master_type != 15 && param.master_type != 25;
    case 70:
    case 80:
        return (param.master_type <= master_type) && param.master_type != 15 && param.master_type != 25 && param.master_type != 60;
    default:
        return false;
    }
}

void __stdcall advTriggerEffect_do_single_line_effect(RGE_Master_Static_Object* master, RGE_Static_Object* obj, RGE_Player* player, const char* s)
{
    char command[8];
    char variable[64];
    char amount[32];
    char amount2[32];

    command[0] = '\0';
    variable[0] = '\0';
    amount[0] = '\0';
    amount2[0] = '\0';

    unsigned char uint8Amount;
    short int16Amount;
    int int32Amount;
    float floatAmount;
    short int16Amount2;

    UNIT_MASTER_DATA_OPERATION op;

    //sscanf_s(s, "%s %s %s",
    //    command, (unsigned)_countof(command), variable, (unsigned)_countof(variable), amount, (unsigned)_countof(amount));

    sscanf_s(s, "%s %s %s %s",
        command, (unsigned)_countof(command), variable, (unsigned)_countof(variable), amount, (unsigned)_countof(amount), amount2, (unsigned)_countof(amount2));

    if (!strcmp(command, "SET"))
        op = OP_SET;
    else if (!strcmp(command, "ADD"))
        op = OP_ADD;
    else if (!strcmp(command, "SUB"))
        op = OP_SUB;
    else if (!strcmp(command, "MUL"))
        op = OP_MUL;
    else if (!strcmp(command, "DIV"))
        op = OP_DIV;
    else if (!strcmp(command, "AND"))
        op = OP_AND;
    else if (!strcmp(command, "OR"))
        op = OP_OR;
    else if (!strcmp(command, "POW"))
        op = OP_POW;
    else
    {
        log("Error: unknown command: %s", command);
        return;
    }
    
    std::map<const std::string, const CHANGE_UNIT_MASTER_PARAMS&>::iterator r = unit_master_lookup_table.find(variable);
    if (r != unit_master_lookup_table.end())
    {
        if (is_master_field_in_this_master((*r).second, master->master_type))
        {
            switch ((*r).second.val_type)
            {
            case T_UINT8:
                if (sscanf_s(amount, "%hhu", &uint8Amount) > 0)
                    modify_value(master, (*r).second.offset, op, uint8Amount);
                else
                    log("Error: value %s is invalid for variable %s", amount, variable);
                break;
            case T_INT16:
                if (sscanf_s(amount, "%hd", &int16Amount) > 0)
                    modify_value(master, (*r).second.offset, op, int16Amount);
                else
                    log("Error: value %s is invalid for variable %s", amount, variable);
                break;
            case T_INT32:
                if (sscanf_s(amount, "%d", &int32Amount) > 0)
                    modify_value(master, (*r).second.offset, op, int32Amount);
                else
                    log("Error: value %s is invalid for variable %s", amount, variable);
                break;
            case T_FLOAT:
                if (sscanf_s(amount, "%f", &floatAmount) > 0)
                    modify_value(master, (*r).second.offset, op, floatAmount);
                else
                    log("Error: value %s is invalid for variable %s", amount, variable);
                break;
            case T_SPRITE_PTR:
                if (op == OP_SET)
                {
                    if (sscanf_s(amount, "%hd", &int16Amount) > 0)
                        modify_sprite(master, obj, player, (RGE_Sprite**)((size_t)master + (*r).second.offset), int16Amount);
                    else
                        log("Error: value %s is not a valid sprite id", amount, variable);
                }
                else
                    log("Error: command %s is not allowed for variable %s", command, variable);
                break;
            case T_SOUND_PTR:
                if (op == OP_SET)
                {
                    if (sscanf_s(amount, "%hd", &int16Amount) > 0)
                        modify_sound((RGE_Sound**)((size_t)master + (*r).second.offset), int16Amount);
                    else
                        log("Error: value %s is not a valid sprite id", amount, variable);
                }
                else
                    log("Error: command %s is not allowed for variable %s", command, variable);
                break;
            case T_ARMOR_CLASS:
                if ((sscanf_s(amount, "%hd", &int16Amount) > 0) && (sscanf_s(amount2, "%hd", &int16Amount2) > 0))
                    modify_armor_class(master, (*r).second.offset, op, int16Amount, int16Amount2);
                else
                    log("Error: valuee %s, %s are invalid for variable %s", amount, amount2, variable);
                break;
            default:
                break;
            }

        }
        else
        {
            log("Warning: cannot modify var %s for type %d", variable, (int)master->master_type);
        }
    }
    else
    {
        log("Error: unknown variable %s", variable);
    }
}

void __stdcall advTriggerEffect_do_multi_line_effect(RGE_Master_Static_Object* master, RGE_Static_Object* obj, RGE_Player* player, const char* s)
{
    char* s_heap = NULL;
    char s_stack[0x800];
    size_t s_len = strlen(s);
    char* s_tmp;
    if (s_len < 0x800)
    {
        s_tmp = s_stack;
    }
    else
    {
        s_heap = (char*)malloc(s_len + 1);
        s_tmp = s_heap;
    }
    strcpy_safe(s_tmp, s_len + 1, s);
    char* pch = strtok(s_tmp, "\r\n");
    char* com_strs[64];
    int str_count = 0;
    while (pch && str_count < 64)
    {
        com_strs[str_count++] = pch;
        pch = strtok(NULL, "\r\n");
    }
    for (int i = 0; i < str_count; i++)
        advTriggerEffect_do_single_line_effect(master, obj, player, com_strs[i]);

    free(s_heap);
}

void __declspec(naked) advTriggerEffect()
{
    __asm
    {
        mov     eax, [edi + 6Ch]
        push    eax         //string
        push    0           //player
        mov     ecx, [ebp]
        push    ecx         //obj
        push    esi         //master
        call    advTriggerEffect_do_multi_line_effect
        ret
    }
}
