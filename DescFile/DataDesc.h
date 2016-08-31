#include <TValue.h>
#include <TStruct.h>
#include <IndexName.h>
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

template<typename DataHandle>
struct player_bagequip_data : public TStruct<DataHandle>
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

	TValue<uint32_t, player_bagequip_data, player_bagequip_ConfigId> ConfigId; //���ñ�ID
	TValue<uint32_t, player_bagequip_data, player_bagequip_Level> Level; //ǿ���ȼ�
};

