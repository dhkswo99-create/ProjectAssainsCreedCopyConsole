#include "Clue.h"
#include <Level/GameLevel.h>
#include <Render/Renderer.h>


Clue::Clue(const Vector2& position)
	: super(L"i", position, Color::BrightYellow)
{
	sortingOrder = 10;
}

Clue::~Clue()
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	level->SoundPlay(L"Item.wav");
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

void Clue::Tick(float deltaTime)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	Renderer::Get().ScreenSubmit( //플레이어 미니맵 처리
		L"I",
		Vector2(static_cast<int>(
			static_cast<float>(position.x)
			/ (static_cast<float>(level->GetMap().size()) / 49)),
			1 + static_cast<int>(
				static_cast<float>(position.y)
				/ (static_cast<float>(level->GetMap().size()) / 20))
		), // offset 0, 1 -> 세부 조정
		Color::BrightYellow,
		20,
		true
	);
}