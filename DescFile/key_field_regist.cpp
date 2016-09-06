#include "stdafx.h"
#include <TData/keyFieldMgr.h>
#include <DataDesc.h>
class DataDesc_Index_Regist
{
public:
	DataDesc_Index_Regist(){
		Data::KeyFieldMgr::AddKey("player.base", player_base);
		Data::KeyFieldMgr::AddPackageDB("player_base", player_base);
		Data::KeyFieldMgr::AddField("Level", player_base_Level, Data::FT_uint32_t);
		Data::KeyFieldMgr::AddField("Nation", player_base_Nation, Data::FT_uint32_t);
		Data::KeyFieldMgr::AddField("Offical", player_base_Offical, Data::FT_uint32_t);
		Data::KeyFieldMgr::AddKey("player.bagequip", player_bagequip);
		Data::KeyFieldMgr::AddRepeatedDB("player_bagequip", player_bagequip);
		Data::KeyFieldMgr::AddField("ConfigId", player_bagequip_ConfigId, Data::FT_uint32_t);
		Data::KeyFieldMgr::AddField("Level", player_bagequip_Level, Data::FT_uint32_t);
		Data::KeyFieldMgr::AddKey("monster.base", monster_base);
		Data::KeyFieldMgr::AddPackageDB("monster_base", monster_base);
		Data::KeyFieldMgr::AddField("Level", monster_base_Level, Data::FT_uint32_t);
	}
};
static DataDesc_Index_Regist s_stDataDesc_Index_Regist;

