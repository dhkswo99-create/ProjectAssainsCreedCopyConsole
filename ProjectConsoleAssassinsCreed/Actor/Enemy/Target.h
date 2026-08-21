#pragma once

#include <Actor/ACTOR.H>
#include <Actor/Enemy/Enemy.h>

using namespace Craft;
class Target : public Enemy
{
	TYPE_DECLARATIONS(Target, Enemy)

public:
	Target(const Vector2& position);
	~Target();

	void Tick(float deltaTime) override;
	bool GetIsDead() { return isDead; }

private:
	bool isDead = false;
};

