#ifndef __Item_CSV_READER_H__
#define __Item_CSV_READER_H__

#include <iostream>
#include <map>

#include "CsvReader.h"

//csv Read the file and place it in the item table. .
void putItemIntoTable(); //( table , Whether to test )

int get_Item_Type_Value(std::string inputString);
int get_Item_SubType_Value(int type_value, std::string inputString);
int get_Item_AntiFlag_Value(std::string inputString);
int get_Item_Flag_Value(std::string inputString);
int get_Item_WearFlag_Value(std::string inputString);
int get_Item_Immune_Value(std::string inputString);
int get_Item_LimitType_Value(std::string inputString);
int get_Item_ApplyType_Value(std::string inputString);


// You can also read Monster Proto .
int get_Mob_Rank_Value(std::string inputString);
int get_Mob_Type_Value(std::string inputString);
int get_Mob_BattleType_Value(std::string inputString);

int get_Mob_Size_Value(std::string inputString);
int get_Mob_AIFlag_Value(std::string inputString);
int get_Mob_RaceFlag_Value(std::string inputString);
int get_Mob_ImmuneFlag_Value(std::string inputString);

//
bool Set_Proto_Mob_Table(TMobTable *mobTable, cCsvTable &csvTable);
bool Set_Proto_Item_Table(TItemTable *itemTable, cCsvTable &csvTable);

#endif