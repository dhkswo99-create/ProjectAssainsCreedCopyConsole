#pragma once

#include <Actor/ACTOR.H>
#include <Actor/Sword.H>
#include <Actor/Enemy/Enemy.h>

using namespace Craft;
class Target : public Enemy
{
	TYPE_DECLARATIONS(Target, Enemy)

public:
	Target(const Vector2& position);
	~Target();







	virtual void BeAttacked(const Vector2& face, int damage) override;
	void Tick(float deltaTime) override;
	bool GetIsDead() { return isDead; }

private:
	// 피격 무적 시간 타이머
	Timer invincibilityTimer;

	// 패턴 생성을 위한 검 객체
	std::vector<std::vector<Vector2>> swordRoute;
	std::vector<std::shared_ptr<Sword>> swordSet;

	// 게임 상태 결정을 위한 변수
	bool isDead = false;
};

