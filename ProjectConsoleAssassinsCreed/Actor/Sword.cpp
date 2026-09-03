  #include "Sword.h"

#include <Actor/Player.h>
#include <Actor/Enemy/Enemy.h>
#include <Actor/Arrow.h>

using namespace Craft;
using SwordFrame = Sword::SwordFrame;

static const SwordFrame sword =
		{L"◈", 0.025f, Color::Red }; // 총소요 0.175초 7틱.

Sword::Sword(const Vector2& position, const std::vector<Vector2>& path,
				const std::weak_ptr<Actor>& handler, int damage)
	: super(sword.frame , Vector2::Zero, sword.color), handler(handler), damage(damage)
{
	hp = 25;
	if (path.size() == 0)
	{
		return;
	}
	swordPos = path;
	isSighted = true;
	sortingOrder = 21;
	timer.SetTargetTime(sword.playTime);
	//충돌 허용
	effectSequenceCount = static_cast<int>(path.size());
	currentIndex = 0;
	SetColiisionEnabled(true);
} 

void Sword::OnCollision(const std::shared_ptr<Actor>& other)
{
	super::OnCollision(other);
	// 생성자와는 충돌이벤트 금지.
	if (auto owner = handler.lock())
	{
		if (other == owner)
		{
			return;
		}
	}
	DoAttack(other, damage);
}

void Sword::DoAttack(const std::shared_ptr<Actor>& other, int damage)
{
	if (
		other->IsTypeOf<Player>()
		|| other->IsTypeOf<Enemy>()
		|| other->IsTypeOf<Arrow>()
		)
	{
		if (auto swordHandler = handler.lock())
		{
			other->BeAttacked(swordHandler->GetFace() , damage);
		}
	}
}

void Sword::BeAttacked(const Vector2& face, int damage)
{
	this->hp -= damage;
	// 체력 0 이하
	if (this->hp <= 0)
	{
		if (auto swordHandler = handler.lock())
		{
			swordHandler->DestroyWeapon();
		}
	}
}

void Sword::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	timer.Tick(deltaTime);
	if (effectSequenceCount == currentIndex)
	{
		Destroy();
		return;
	}

	if (!timer.IsTimeOut())
	{
		return;
	}

	SetPosition(swordPos[currentIndex]);
	timer.Reset();
	++currentIndex;
}

