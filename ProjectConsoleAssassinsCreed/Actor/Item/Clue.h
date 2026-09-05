#pragma once

#include <Actor/Item/Item.h>

using namespace Craft;
class Clue : public Item
{
	TYPE_DECLARATIONS(Clue, Item)

public:
	Clue(const Vector2& position);
	~Clue();

	void SetClue(std::wstring newClue)
	{
		clue = newClue;
	}

	void SetTarget(bool kindOfClue)
	{
		bTarget = kindOfClue;
	}
	void SetClient(bool kindOfClue)
	{
		bClient = kindOfClue;
	}

	std::wstring GetClue() { return clue; }



private:


private:
	std::wstring clue;
	
	bool bTarget = false;
	bool bClient = false;
};

