#include "Clue.h"
#include <Level/GameLevel.h>


Clue::Clue(const Vector2& position)
	: super(L"i", position, Color::White)
{
	sortingOrder = 10;
}

Clue::~Clue()
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	if (level)
	{
		if (bTarget)
		{
			level->DropTargetClue(clue);
		}
		else if (bClient)
		{
			level->DropClientClue(clue);
		}
	}
}