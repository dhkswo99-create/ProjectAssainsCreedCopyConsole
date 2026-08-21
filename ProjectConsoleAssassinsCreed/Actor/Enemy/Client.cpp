#include "Client.h"

Client::Client(const Vector2& position)
	:super(L"C", position, Color::Purple)
{
	sortingOrder = 3;
	SetColiisionEnabled(true);
}

Client::~Client()
{
	isDead = true;
}

void Client::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}
