#include "stdafx.h"

#include "revertToX1.h"

BYTE_ASSIGN revertX1[] =
{
	{0x1C593, 0x66},	//ground-to-air
	{0x1C594, 0x81},
	{0x1C595, 0xFA},
	{0x1C596, 0x22},
	{0x1C597, 0x5},
	{0x5D4BD, 0x6},		//?map print?
	{0x5D4CA, 0x3},		//?--
	{0x5D4DC, 0xFD},	//?--
	{0x60E9F, 0xC5},	//check!
	{0x60EA0, 0x1B},
	{0xF87AD, 0x24},	//???
	{0xF89CE, 0xF},
	{0xF89CF, 0x84},
	{0xF89E1, 0x75},
	{0xF8A13, 0x75},
	{0xF8A14, 0x33},
	{0x12A30A, 0x8B}, //terrain load jmp
	{0x12A30B, 0xF},
	{0x12A30C, 0x6A},
	{0x12A30D, 0x0},
	{0x12A30E, 0x68},
	{0x13B3C1, 0x6A},
	{0x13B3C2, 0xB},
	{0x13B3C3, 0x68},
	{0x13B3C4, 0x92},
	{0x13B3C5, 0x29}, //end terrain load
	{0x13BD3A, 0x8B},		//effects
	{0x13BD3B, 0x8F},
	{0x13BD3C, 0x24},
	{0x13BD3D, 0xE},
	{0x13C429, 0xF},	//???
	{0x13C42A, 0x8C},
	{0x13C42B, 0x4C},
	{0x13C42C, 0x4},
	{0x13CA4E, 0xF},	//???
	{0x13CA4F, 0x8C},
	{0x13CA50, 0xD7},
	{0x13CA51, 0x5},
	{0x13CE86, 0x75},	//smth with effect
	{0x1E3EE0, 0x56},	//jedi-holo 1
	{0x1E3EE1, 0x8B},
	{0x1E3EE2, 0xC1},
	{0x1E3EE3, 0x33},
	{0x1E3EE4, 0xF6},
	{0x1E3F10, 0x56},	//jedi-holo 2
	{0x1E3F11, 0x8B},
	{0x1E3F12, 0xC1},
	{0x1E3F13, 0x33},
	{0x1E3F14, 0xF6},
	{0x1F2690, 0x57},	//???
	{0x1F2691, 0xC7},
	{0x1F2692, 0x41},
	{0x1F2693, 0x4},
	{0x1F2694, 0x0},
	{0x1F2695, 0x0},
	{0x1F2696, 0x0},
	{0x1F2697, 0x0},
	{0x1F2698, 0x8D},
	{0x1F2699, 0x79},
	{0x1F269A, 0xC},
	{0x1F269B, 0xB9},
	{0x1F269C, 0x10},
	{0x1F269D, 0x0},
	{0x1F26A0, 0x83},
	{0x1F26A1, 0xC8},
	{0x1F26A2, 0xFF},
	{0x1F26A3, 0xF3},
	{0x1F26A4, 0xAB},
	{0x1F26A5, 0x5F},
	{0x1F26A6, 0xC3},
	{0x1F26A7, 0x90},
	{0x1F2AF8, 0x5},		//effect overflow
	{0x1F2B02, 0x5},
	//{0x1F2B4C, 0x25},	//count
	//{0x1F2B56, 0xD0},	//jumptable
	//{0x1F2B57, 0x3D},
	//{0x1F2B58, 0x5F},
	{0x1F2C5F, 0x5},
	{0x1F3DB5, 0x5},
	{0x1F3DC8, 0x5},
	//{0x1F557B, 0x8B},	//effect init
	//{0x1F557C, 0x46},
	//{0x1F557D, 0x8},
	//{0x1F557E, 0xB1},
	//{0x1F557F, 0x1},
	{0x28F277, 0x43},	//title
	{0x28F278, 0x6C},
	{0x28F279, 0x6F},
	{0x28F27A, 0x6E},
	{0x28F27B, 0x65},
	{0x28F27C, 0x20},
	{0x28F27D, 0x43},
	{0x28F27E, 0x61},
	{0x28F27F, 0x6D},
	{0x28F280, 0x70},
	{0x28F281, 0x61},
	{0x28F282, 0x69},
	{0x28F283, 0x67},
	{0x28F285, 0x73},
	{0x28F286, 0x5C},
	{0x28F287, 0x31},
	{0x28F288, 0x2E},
	{0x28F289, 0x30},
	{0x28F28A, 0x0},
	{0x28F298, 0x31},//
	{0x28F2CE, 0x43},//
	{0x28F2CF, 0x6C},
	{0x28F2D0, 0x6F},
	{0x28F2D1, 0x6E},
	{0x28F2D2, 0x65},
	{0x28F2D3, 0x20},
	{0x28F2D4, 0x43},
	{0x28F2D5, 0x61},
	{0x28F2D6, 0x6D},
	{0x28F2D7, 0x70},
	{0x28F2D8, 0x61},
	{0x28F2D9, 0x69},
	{0x28F2DA, 0x67},
	{0x28F2DC, 0x73},
	{0x28F2DD, 0x0},
	{0x290CFF, 0x31},
	{0x2910F2, 0x31},
	{0x2910FA, 0x31},
	{0x291101, 0x31},
	{0x291109, 0x31},
	{0x291113, 0x31},
	{0x29111F, 0x31},
	{0x299D86, 0x31},
	{0x299D96, 0x31},//??
	{0x299F96, 0x31},
	{0x299FA5, 0x31},
	{0x299FB6, 0x31},
	{0x299FC4, 0x31},
	{0x29A409, 0x31},
	{0x29A74B, 0x31},
	{0x29E55F, 0x31}
};

void revertToX1()
{
	log("Restoring x1 state...");

	for (int i = 0; i < (sizeof(revertX1) / sizeof(revertX1[0])); i++)
		writeByteF(revertX1[i].addr, revertX1[i].val);
}
