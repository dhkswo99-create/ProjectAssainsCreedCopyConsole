#include "Item.h"
#include <Level/GameLevel.h>

using namespace Craft;

Item::Item(
	const std::wstring& image,
	const Vector2& position,
	Color color)
	: super(L"i", position, Color::White)
{
	dropKey = L"F";
	SetColiisionEnabled(true);
}

void Item::OnCollision(const std::shared_ptr<Actor>& other)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	level->ItemOnCollision(dropKey);
}

 