#pragma once
#include <TData/TValue.h>
#include <TData/TStruct.h>
enum E_player_Index
{
	player_base = 0x0,
	player_base_Level = 0x0,
	player_base_Nation = 0x1,
	player_base_Offical = 0x2,
	player_bagequip = 0x100,
	player_bagequip_ConfigId = 0x100,
	player_bagequip_Level = 0x101,
};

enum E_monster_Index
{
	monster_base = 0x10000,
	monster_base_Level = 0x10000,
};

struct player_bagequip_data : public Data::TStruct<Data::IRepeated>
{
	player_bagequip_data()
	{
		ConfigId.SetDataHandle(this);
		Level.SetDataHandle(this);
	}
	player_bagequip_data(const player_bagequip_data& other)
		: ConfigId(other.ConfigId)
		, Level(other.Level)
	{
		ConfigId.SetDataHandle(this);
		Level.SetDataHandle(this);
	}

	using ConfigIdType = Data::TValue<uint32_t, player_bagequip_data, player_bagequip_ConfigId>;
	using LevelType = Data::TValue<uint32_t, player_bagequip_data, player_bagequip_Level>;
	ConfigIdType ConfigId; //配置表ID
	LevelType Level; //强化等级
};

