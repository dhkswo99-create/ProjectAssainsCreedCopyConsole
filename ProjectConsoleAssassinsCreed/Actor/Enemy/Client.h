#pragma once

#include <Actor/ACTOR.H>
#include <Actor/Enemy/Enemy.h>

using namespace Craft;
class Client : public Enemy
{
	TYPE_DECLARATIONS(Client, Enemy)

public:
	Client(const Vector2& position);
	~Client();

	virtual void BeAttacked(const Vector2& face, int damage) override;
	void Tick(float deltaTime) override;
	bool GetIsDead() { return isDead; }

private:
	bool isDead = false;
};

