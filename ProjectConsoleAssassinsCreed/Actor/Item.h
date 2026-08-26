#pragma once

#include <Actor/ACTOR.H>

using namespace Craft;
class Item : public Actor
{
	TYPE_DECLARATIONS(Item, Actor)

public:
	Item(const Vector2& position);
	~Item() = default;

	void SetClue(std::wstring newClue)
	{
		clue = newClue;
	}

private:


private:
	std::wstring clue;
};

