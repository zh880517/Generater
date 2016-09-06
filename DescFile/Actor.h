#pragma once
#include <TData/TValue.h>
#include <TData/TRepeated.h>
#include <TData/TStruct.h>
#include <TData/IDataAdapter.h>
#include <DataDesc.h>

struct PlayerBaseData : public Data::TStruct<Data::IDataAdapter>
{
	PlayerBaseData()
	{
		Level.SetDataHandle(this);
		Nation.SetDataHandle(this);
		bagequip.SetDataHandle(this);
	}
	PlayerBaseData(const PlayerBaseData& other)
		: Level(other.Level)
		, Nation(other.Nation)
		, bagequip(other.bagequip)
	{
		Level.SetDataHandle(this);
		Nation.SetDataHandle(this);
		bagequip.SetDataHandle(this);
	}

	using LevelType = Data::TValue<uint32_t, PlayerBaseData, player_base_Level>;
	using NationType = Data::TBaseValue<uint32_t, PlayerBaseData, player_base_Nation>;
	using bagequipType = Data::TRepeated<player_bagequip_data, PlayerBaseData, player_bagequip> ;

	LevelType Level; //玩家等级
	NationType Nation; //玩家国籍
	bagequipType bagequip; //背包物品
};


