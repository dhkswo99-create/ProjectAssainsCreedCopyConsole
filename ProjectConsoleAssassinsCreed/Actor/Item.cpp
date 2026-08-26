#include "Item.h"

Item::Item(const Vector2& position)
	: super(L"i", position, Color::White)
{
	sortingOrder = 1;
}
