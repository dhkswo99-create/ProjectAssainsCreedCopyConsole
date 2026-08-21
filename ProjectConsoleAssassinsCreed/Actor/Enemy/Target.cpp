#include "Target.h"

using namespace Craft;
Target::Target(const Vector2& position)
	: super(L"T", position, Color::White)
{
	sortingOrder = 3;
	SetColiisionEnabled(true);
}

Target::~Target()
{
	isDead = true;
}

void Target::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}