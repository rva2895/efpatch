#include "stdafx.h"
#include "editorres.h"

const char* resourceNames[] =
{
    "Food",                             //0
    "Carbon",
    "Ore",
    "Nova",
    "Population Headroom",
    "Conversion Range",
    "Current Tech Level",
    "Holocrons Captured",
    "Unused (Trade Bonus)",
    "Trade Goods",
    "Recharge Rate of Shields",         //10
    "Current Population",
    "Corpse Decay Time",
    "Remarkable Discovery",
    "Monuments Captured",
    "Meat Storage",
    "Berry Storage",
    "Fish Storage",
    "Power Core Range",
    "Unpowered Building Work Rate Multiplier",
    "Units Killed",                     //20
    "Technology Count",
    "% Map Explored",
    "Submarine Detection",
    "Shield Generator Range",
    "Shield Recharge Interval",
    "Drop-off Time of Shields",
    "Enable Jedi Conversion",
    "Enable Building Conversion",
    "Fambaa Shield Generator Range",
    "Unused (Building Limit)",          //30
    "Enable A-A Attack For AT-AT",
    "Bonus Population Cap",
    "Power Core Shielding",
    "Force",
    "Force Recharging Rate",
    "Farm Food Amount",
    "Civilian Population",
    "Shields On For Bombers/Fighters",
    "All Techs Achieved",
    "Military Population",              //40
    "Conversions",
    "Standing Monuments",
    "Razings",
    "Kill Ratio",
    "Survival to Finish",
    "Tribute Inefficiency",
    "Building Self Regeneration Timer",
    "Town Center Unavailable",
    "Gold Counter",
    "Reveal Ally",                      //50
    "Shielding",
    "Monasteries",
    "Tribute Sent",
    "All Ruins Captured",
    "All Relics Captured",
    "Enable Stealth For Masters",
    "Kidnap Storage",
    "Masters Can See Hidden Units",
    "Tech Level 1 Tech ID",
    "Tech Level 2 Tech ID",             //60
    "Tech Level 3 Tech ID",
    "Building Housing Rate",
    "Tech Level 4 Tech ID",
    "Gather Accumulator",
    "Salvage Decay Rate",
    "Unused (Allow Formations)",
    "Can Convert",
    "Hit Points Killed",
    "Killed P1",
    "Killed P2",                        //70
    "Killed P3",
    "Killed P4",
    "Killed P5",
    "Killed P6",
    "Killed P7",
    "Killed P8",
    "Conversion Resistance",
    "Trade Vig Rate",
    "Ore Mining Productivity",
    "Queued Units",                     //80
    "Training Count",
    "Start with Packed Town Center",
    "Boarding Recharge Rate",
    "Starting Villagers",
    "Tech Cost Modifier",
    "Tech Time Modifier",
    "Concentration",
    "Fish Trap Food Amount",
    "Medic Healing Rate",
    "Healing Range",                    //90
    "Starting Food",
    "Starting Carbon",
    "Starting Ore",
    "Starting Nova",
    "Enable PTWC / Kidnap / Loot",
    "Berserker Heal Timer",
    "Dominant Sheep Control",
    "Object Cost Sum",
    "Tech Cost Sum",
    "Holocron Nova Sum",                //100
    "Trade Income Sum",
    "P1 Tribute",
    "P2 Tribute",
    "P3 Tribute",
    "P4 Tribute",
    "P5 Tribute",
    "P6 Tribute",
    "P7 Tribute",
    "P8 Tribute",
    "P1 Kill Value",                    //110
    "P2 Kill Value",
    "P3 Kill Value",
    "P4 Kill Value",
    "P5 Kill Value",
    "P6 Kill Value",
    "P7 Kill Value",
    "P8 Kill Value",
    "P1 Razings",
    "P2 Razings",
    "P3 Razings",                       //120
    "P4 Razings",
    "P5 Razings",
    "P6 Razings",
    "P7 Razings",
    "P8 Razings",
    "P1 Razing Value",
    "P2 Razing Value",
    "P3 Razing Value",
    "P4 Razing Value",
    "P5 Razing Value",                  //130
    "P6 Razing Value",
    "P7 Razing Value",
    "P8 Razing Value",
    "Standing Fortresses",
    "Hit Points Razed",
    "Kills by P1",
    "Kills by P2",
    "Kills by P3",
    "Kills by P4",
    "Kills by P5",                      //140
    "Kills by P6",
    "Kills by P7",
    "Kills by P8",
    "Razings by P1",
    "Razings by P2",
    "Razings by P3",
    "Razings by P4",
    "Razings by P5",
    "Razings by P6",
    "Razings by P7",                    //150
    "Razings by P8",
    "Value Killed by Others",
    "Value Razed by Others",
    "Killed by Others",
    "Razed by Others",
    "Tribute from P1",
    "Tribute from P2",
    "Tribute from P3",
    "Tribute from P4",
    "Tribute from P5",                  //160
    "Tribute from P6",
    "Tribute from P7",
    "Tribute from P8",
    "Value Current Units",
    "Value Current Buildings",
    "Food Total",
    "Carbon Total",
    "Ore Total",
    "Nova Total",
    "Total Value of Kills",             //170
    "Total Tribute Received",
    "Total Value of Razings",
    "Total Fortresses Built",
    "Total Monuments Built",
    "Tribute Score",
    "Convert Min Adjustment",
    "Convert Max Adjustment",
    "Convert Resist Min Adjustment",
    "Convert Resist Max Adjustment",
    "Convert Building Min",             //180
    "Convert Building Max",
    "Convert Building Chance",
    "Reveal Enemy",
    "Value Wonders Castles",
    "Food Score",
    "Carbon Score",
    "Ore Score",
    "Nova Score",
    "Carbon Gathering Productivity",
    "Food-gathering Productivity",      //190
    "Holocron Nova Production Rate",
    "Converted Units Die",
    "Meditation",
    "Crenellations",
    "Construction Rate Modifier",
    "Hun Wonder Bonus",
    "Spies Discount",
    "Unknown 198",
    "Unknown 199",
    "Misc Counter 1",                   //200
    "Misc Counter 2",
    "Misc Counter 3",
    "Misc Counter 4",
    "Misc Counter 5",
    "Unknown 205",
    "Unknown 206",
    "Unknown 207",
    "Unknown 208",
    "Unknown 209",
    "Unknown 210",                      //210
    "Buildings are self powered",
    "Aircraft regenerates Hit Pts",
    "Max Force After Conversion",
    "Nova Mining Productivity",
    "Nova generation rate (small)",
    "Nova generation rate (large)",
    "Ore generation rate",
    "Food generation rate"
};

void __stdcall init_scenario_editor_dropdown(TDropDownPanel* dropdown)
{
    for (int i = 0; i < _countof(resourceNames); i++)
        TDropDownPanel__append_line(dropdown, resourceNames[i], i);
}

__declspec(naked) void onLoadScenResources() //0052A6DC, 0052A7B2
{
    __asm
    {
        mov     ecx, [edi]
        push    ecx
        call    init_scenario_editor_dropdown
        mov     ecx, 0052A83Fh
        jmp     ecx
    }
}

#pragma optimize( "s", on )
void setResListHooks()
{
    setHook((void*)0x0052A6DC, onLoadScenResources);
    setHook((void*)0x0052A7B2, onLoadScenResources);
}
#pragma optimize( "", on )
