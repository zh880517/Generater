#include <string>
#include <map>
#include <Name.h>
#include <UtilsMap.h>
class PlayerBaseItemBagData
{ 
public:
	PlayerBaseItemBagData(uint64_t iKeyFirst, uint64_t ikeySecond):KeyFirst(iKeyFirst),KeySecond(iKeySecond),BaseID(0),ItemNum(1){}

	uint64_t GetKeyFirst()const { return KeyFirst; }
	uint64_t GetKeySecond()const { return KeySecond; }

	uint32_t GetBaseID()const { return BaseID; }
	uint32_t GetItemNum()const { return ItemNum; }

	template<typename T>
	bool SetBaseID(T &t, uint32_t t_BaseID)
	{
		if (t_BaseID == BaseID) return false;
		t.SetValue(Name("player.bag.item", 636454631), Name("BaseID", 2834271757), BaseID, KeyFirst, KeySecond);
		return true;
	}

	template<typename T>
	bool SetItemNum(T &t, uint32_t t_ItemNum)
	{
		if (t_ItemNum == ItemNum) return false;
		t.SetValue(Name("player.bag.item", 636454631), Name("ItemNum", 219292900), ItemNum, KeyFirst, KeySecond);
		return true;
	}

	template<typename T> 
	void Visit(T& t)
	{
		t.Property(Name("player.bag.item", 636454631), Name("BaseID", 2834271757), BaseID, true);
		t.Property(Name("player.bag.item", 636454631), Name("ItemNum", 219292900), ItemNum, true);
	}
private:
	uint32_t BaseID;
	uint32_t ItemNum;
	uint64_t KeyFirst;
	uint64_t KeySecond;
}

class PlayerBaseData
{ 
public:
	PlayerBaseData(uint64_t iKey):Key(iKey),BuySpace(0){}

	uint64_t GetKey()const { return Key; }

	uint32_t GetCreatTime()const { return CreatTime; }
	uint32 GetLevel()const { return Level; }

	template<typename T>
	bool SetLevel(T &t, uint32 t_Level)
	{
		if (t_Level == Level) return false;
		t.SetValue(Name("player.base", 852532731), Name("Level", 228043805), Level, Key);
		return true;
	}
	uint32_t GetBuySpace()const { return BuySpace; }
	PlayerBaseItemBagData* GetItemBagData(uint64_t iIndex) { return FindMapPtr(mItemBag, iIndex); }

	template<typename T>
	bool AddItemBagData(T& t, const PlayerBaseItemBagData& stData, uint64_t iIndex)
	{
		if (FindMapPtr(mItemBag, iIndex) != nullptr) return false;
		mItemBag[iIndex] = stData;
		t.Add(Name("player.bag.item", 636454631), it->second);
		return true;
	}

	template<typename T>
	bool DeleteItemBagData(T& t, uint64_t iIndex)
	{
		auto it = mItemBag.find(iIndex);
		if (it != mItemBag.end())
		{
			t.Delete(Name("player.bag.item", 636454631), it->second);
			mItemBag.erase(it);
			return true;
		}
		return false;
	}

	template<typename T>
	void ForEacheItemBagData(T&t)
	{
		for (auto it : mItemBag)
			if (!t(it->first, it->second))
				return;
	}

	template<typename T> 
	void Visit(T& t)
	{
		t.Property(Name("player.base", 852532731), Name("CreatTime", 3982507683), CreatTime, false);
		t.Property(Name("player.base", 852532731), Name("Level", 228043805), Level, true);
		t.Property(Name("player.bag", 4190651082), Name("BuySpace", 3009882721), BuySpace, false);
		t.Repeat(Name("player.bag.item", 636454631), mItemBag, true);
	}
private:
	uint32_t CreatTime;
	uint32 Level;
	uint32_t BuySpace;
	std::map<uint64_t, PlayerBaseItemBagData> mItemBag;
	uint64_t Key;
}

struct Actor_PlayerData
{
	PlayerBaseData Base;

	template<typename T> 
	void Update(T& t)
	{
		t.Struct(Name("player", 351953554), Base);
	}

}

class ScenePlayerItemBagData
{ 
public:
	ScenePlayerItemBagData(uint64_t iKeyFirst, uint64_t ikeySecond):KeyFirst(iKeyFirst),KeySecond(iKeySecond),BaseID(0),ItemNum(1){}

	uint64_t GetKeyFirst()const { return KeyFirst; }
	uint64_t GetKeySecond()const { return KeySecond; }

	uint32_t GetBaseID()const { return BaseID; }
	uint32_t GetItemNum()const { return ItemNum; }

	template<typename T> 
	void Visit(T& t)
	{
		t.Property(Name("player.bag.item", 636454631), Name("BaseID", 2834271757), BaseID, false);
		t.Property(Name("player.bag.item", 636454631), Name("ItemNum", 219292900), ItemNum, false);
	}
private:
	uint32_t BaseID;
	uint32_t ItemNum;
	uint64_t KeyFirst;
	uint64_t KeySecond;
}

class ScenePlayerData
{ 
public:
	ScenePlayerData(uint64_t iKey):Key(iKey){}

	uint64_t GetKey()const { return Key; }

	uint32 GetLevel()const { return Level; }
	ScenePlayerItemBagData* GetItemBagData(uint64_t iIndex) { return FindMapPtr(mItemBag, iIndex); }

	template<typename T> 
	void Visit(T& t)
	{
		t.Property(Name("player.base", 852532731), Name("Level", 228043805), Level, false);
		t.Repeat(Name("player.bag.item", 636454631), mItemBag, false);
	}
private:
	uint32 Level;
	std::map<uint64_t, ScenePlayerItemBagData> mItemBag;
	uint64_t Key;
}

struct Actor_SceneData
{
	std::map<uint64_t, ScenePlayerData> mPlayer;

	template<typename T> 
	void Update(T& t)
	{
		t.Map(Name("player", 351953554), mPlayer);
	}

}

