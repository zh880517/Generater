#include "stdafx.h"
#include <DataDesc.h>
class DataDesc_Index_Regist
{
public:
	DataDesc_Index_Regist(){
		KeyFieldMgr::AddKey("player.base", player_base);
		KeyFieldMgr::AddField("Level", player_base_Level);
		KeyFieldMgr::AddField("Nation", player_base_Nation);
		KeyFieldMgr::AddField("Offical", player_base_Offical);
		KeyFieldMgr::AddKey("player.bagequip", player_bagequip);
		KeyFieldMgr::AddField("ConfigId", player_bagequip_ConfigId);
		KeyFieldMgr::AddField("Level", player_bagequip_Level);
		KeyFieldMgr::AddKey("monster.base", monster_base);
		KeyFieldMgr::AddField("Level", monster_base_Level);
	}
};
static DataDesc_Index_Regist s_stDataDesc_Index_Regist;

