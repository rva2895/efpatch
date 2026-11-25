// RGE_Action_Types.h - owner-supplied registries for the three action
// id namespaces (mirrors the RGE_MASTER_TYPE enum precedent in
// RGE_Master_Static_Object.h). Context for naming/comments only; the
// literals in replacement code stay cited to the asm per the template.
// Owner-supplied 2026-11-22; the order registry is DERIVED (see the
// tail comment).

// RGE_Action::action_type / RGE_Task::action_type (__int16 @+4 / @+6).
// The setUnitAIAction/actionTypeIfOrdered/work2/is_idle consumers and
// the create_task_action switch arms key on these.
enum RGE_ACTION_TYPE
{
    RGE_ACTION_TYPE_MOVE_TO             = 1,    // Move to
    RGE_ACTION_TYPE_FOLLOW              = 2,    // Follow
    RGE_ACTION_TYPE_ENTER               = 3,    // Enter
    RGE_ACTION_TYPE_EXPLORE             = 4,    // Explore
    RGE_ACTION_TYPE_GATHER              = 5,    // Gather
    RGE_ACTION_TYPE_ATTACK              = 7,    // Attack
    RGE_ACTION_TYPE_ATTACK2             = 9,    // Attack2
    RGE_ACTION_TYPE_BIRD                = 10,   // Bird
    RGE_ACTION_TYPE_TRANSPORT           = 12,   // Transport
    RGE_ACTION_TYPE_GUARD               = 13,   // Guard
    RGE_ACTION_TYPE_MAKE                = 21,   // Make
    RGE_ACTION_TYPE_BUILD               = 101,  // Build
    RGE_ACTION_TYPE_MAKE_UNIT           = 102,  // Make unit
    RGE_ACTION_TYPE_MAKE_TECH           = 103,  // Make tech
    RGE_ACTION_TYPE_CONVERT             = 104,  // Convert
    RGE_ACTION_TYPE_HEAL                = 105,  // Heal
    RGE_ACTION_TYPE_REPAIR              = 106,  // Repair
    RGE_ACTION_TYPE_ARTIFACT            = 107,  // Artifact
    RGE_ACTION_TYPE_DISCOVERY_ARTIFACT  = 108,  // Discovery artifact
    RGE_ACTION_TYPE_HUNT                = 110,  // Hunt
    RGE_ACTION_TYPE_TRADE               = 111,  // Trade
    RGE_ACTION_TYPE_WONDER              = 120,  // Wonder
    RGE_ACTION_TYPE_FARM                = 121,  // Farm
    RGE_ACTION_TYPE_HOUSING             = 123,  // Housing
    RGE_ACTION_TYPE_UNPACK              = 125,  // Unpack
    RGE_ACTION_TYPE_OFFBOARD_TRADE      = 131,  // Offboard trade
    RGE_ACTION_TYPE_PICKUP_UNIT         = 132,  // Pickup unit
    RGE_ACTION_TYPE_CAPTURE             = 135,  // Capture
    RGE_ACTION_TYPE_DELIVER_UNIT        = 136,  // Deliver unit
    RGE_ACTION_TYPE_SHEPHERD            = 149,  // Shepherd
};

// RGE_Action::state (unsigned __int8 @+0xC). The per-action state
// machine (is_idle's idle set, the list-update removal pair). Owner-
// supplied 2026-11-22.
enum RGE_ACTION_STATE
{
    RGE_ACTION_STATE_NONE           = 0,    // None
    RGE_ACTION_STATE_DONE           = 1,    // Done
    RGE_ACTION_STATE_WAIT           = 2,    // Wait
    RGE_ACTION_STATE_SEARCH         = 3,    // Search
    RGE_ACTION_STATE_GOTO           = 4,    // Goto
    RGE_ACTION_STATE_GOTO_2         = 5,    // Goto2
    RGE_ACTION_STATE_WORK           = 6,    // Work
    RGE_ACTION_STATE_WORK_2         = 7,    // Work2
    RGE_ACTION_STATE_RETURN         = 8,    // Return
    RGE_ACTION_STATE_TURN           = 9,    // Turn
    RGE_ACTION_STATE_DELAY          = 10,   // Delay
    RGE_ACTION_STATE_MOVE           = 11,   // Move
    RGE_ACTION_STATE_ATTACK         = 12,   // Attack
    RGE_ACTION_STATE_FAILED         = 13,   // Failed
    RGE_ACTION_STATE_INVALIDATED    = 14,   // Invalidated
    RGE_ACTION_STATE_MOVE_NO_SEARCH = 15,   // MoveNoSearch
};

// UnitAIModule::currentActionValue (the module's current-action id;
// setUnitAIAction's second resolved value, work2's type()==9 confirm,
// the speed-fallback move). 600-699.
enum RGE_UNITAI_ACTION
{
    RGE_UNITAI_ACTION_ATTACK            = 600,  // attack
    RGE_UNITAI_ACTION_DEFEND            = 601,  // defend
    RGE_UNITAI_ACTION_BUILD             = 602,  // build
    RGE_UNITAI_ACTION_HEAL              = 603,  // heal
    RGE_UNITAI_ACTION_CONVERT           = 604,  // convert
    RGE_UNITAI_ACTION_EXPLORE           = 605,  // explore
    RGE_UNITAI_ACTION_STOP              = 606,  // stop
    RGE_UNITAI_ACTION_RUN_AWAY          = 607,  // run away
    RGE_UNITAI_ACTION_RETREAT           = 608,  // retreat
    RGE_UNITAI_ACTION_GATHER            = 609,  // gather
    RGE_UNITAI_ACTION_MOVE              = 610,  // move
    RGE_UNITAI_ACTION_PATROL            = 611,  // patrol
    RGE_UNITAI_ACTION_FOLLOW            = 612,  // follow
    RGE_UNITAI_ACTION_HUNT              = 613,  // hunt
    RGE_UNITAI_ACTION_TRANSPORT         = 614,  // transport
    RGE_UNITAI_ACTION_TRADE             = 615,  // trade
    RGE_UNITAI_ACTION_EVASIVE_MOVE      = 616,  // evasive move
    RGE_UNITAI_ACTION_ENTER             = 617,  // enter
    RGE_UNITAI_ACTION_REPAIR            = 618,  // repair
    RGE_UNITAI_ACTION_TRAIN             = 619,  // train
    RGE_UNITAI_ACTION_RESEARCH          = 620,  // research
    RGE_UNITAI_ACTION_UNLOAD            = 621,  // unload
    RGE_UNITAI_ACTION_SEEK_AND_DESTROY  = 622,  // seek and destroy
    RGE_UNITAI_ACTION_EXPLORE_AND_DESTROY = 623,// explore and destroy
    RGE_UNITAI_ACTION_GROUP_GATHER      = 624,  // group gather
    RGE_UNITAI_ACTION_WAIT              = 628,  // wait
    RGE_UNITAI_ACTION_ATTACK_ROUNDUP    = 629,  // attack roundup
    RGE_UNITAI_ACTION_SCOUT             = 630,  // scout
    RGE_UNITAI_ACTION_CONTROL           = 631,  // control
    RGE_UNITAI_ACTION_SHEPHERD          = 632,  // shepherd
    RGE_UNITAI_ACTION_RELEASED          = 699,  // released
};

// UnitAIModule::currentOrderValue (the module's current-order id;
// setUnitAIAction's first resolved value). DERIVED, not owner-supplied:
// every setUnitAIAction table pair satisfies order = action + 100
// (all 12 pairs asm-verified 0x409C57-0x409CE3), so the order names
// mirror the action names. Only the 12 table-reachable values are
// enumerated; the +100 rule extends to the rest of the 6xx set.
enum RGE_UNITAI_ORDER
{
    RGE_UNITAI_ORDER_ATTACK             = 700,  // = ATTACK 600 + 100
    RGE_UNITAI_ORDER_DEFEND             = 701,  // = DEFEND 601 + 100 (derived by the same +100 rule; the consumer = RGE_Action_Guard__first_in_stack_new's order dispatch, asm push 2BDh)
    RGE_UNITAI_ORDER_BUILD              = 702,  // = BUILD 602 + 100
    RGE_UNITAI_ORDER_HEAL               = 703,  // = HEAL 603 + 100
    RGE_UNITAI_ORDER_CONVERT            = 704,  // = CONVERT 604 + 100
    RGE_UNITAI_ORDER_EXPLORE            = 705,  // = EXPLORE 605 + 100
    RGE_UNITAI_ORDER_GATHER             = 709,  // = GATHER 609 + 100
    RGE_UNITAI_ORDER_MOVE               = 710,  // = MOVE 610 + 100
    RGE_UNITAI_ORDER_PATROL             = 711,  // = PATROL 611 + 100 (derived by the same +100 rule; the consumer = RGE_Action_Move_To__update_new's re-path gate, asm cmp 2C7h)
    RGE_UNITAI_ORDER_HUNT               = 713,  // = HUNT 613 + 100
    RGE_UNITAI_ORDER_TRANSPORT          = 714,  // = TRANSPORT 614 + 100
    RGE_UNITAI_ORDER_TRADE              = 715,  // = TRADE 615 + 100
    RGE_UNITAI_ORDER_ENTER              = 717,  // = ENTER 617 + 100
    RGE_UNITAI_ORDER_REPAIR             = 718,  // = REPAIR 618 + 100
};

// RGE_Player::attributes (float* @+0xAC = 172; [index] = +4*index). The
// FULL owner-supplied registry (0-218) lives in DECOMP_GAME_FACTS.md;
// the consumed values are enumerated here (the RGE_UNITAI_ORDER
// precedent - the set grows as the decodes consume):
enum RGE_PLAYER_ATTRIBUTE
{
    RGE_PLAYER_ATTRIBUTE_CONVERSION_RANGE         = 5,   // Conversion Range (the actionTypeIfOrdered 104-Convert range test)
    RGE_PLAYER_ATTRIBUTE_SHIELD_RECHARGE_RATE     = 10,  // Recharge Rate of Shields (the shield pair multiplier)
    RGE_PLAYER_ATTRIBUTE_SHIELD_RECHARGE_INTERVAL = 25,  // Shield Recharge Interval (the shield pair tick interval)
    RGE_PLAYER_ATTRIBUTE_SHIELD_DROP_OFF_TIME     = 26,  // Drop-off Time of Shields (the drop-off tick amount)
    RGE_PLAYER_ATTRIBUTE_ENABLE_BUILDING_CONVERSION = 28,// Enable Building Conversion (the actionTypeIfOrdered gate)
    RGE_PLAYER_ATTRIBUTE_FARM_FOOD_AMOUNT         = 36,  // Farm Food Amount (the Make Work timer source for the Berry Storage tasks)
    RGE_PLAYER_ATTRIBUTE_CIVILIAN_POPULATION      = 37,  // Civilian Population (the TRIBE_Combat destructor decrement)
    RGE_PLAYER_ATTRIBUTE_MILITARY_POPULATION      = 40,  // Military Population (the TRIBE_Combat destructor decrement)
    RGE_PLAYER_ATTRIBUTE_FISH_TRAP_FOOD_AMOUNT    = 88,  // Fish Trap Food Amount (the Make Work timer source otherwise)
};
