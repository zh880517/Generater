#include <TValue.h>
#include <TRepeated.h>
#include <TStruct.h>
#include <DataDesc.h>

template<typename DataHandle>
struct PlayerBaseData : public TStruct<DataHandle>
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

	TValue<uint32_t, PlayerBaseData, player_base_Level> Level; //玩家等级
	TBaseValue<uint32_t, PlayerBaseData, player_base_Nation> Nation; //玩家国籍
	TRepeated<player_bagequip_data, PlayerBaseData, player_bagequip> bagequip; //背包物品
};


