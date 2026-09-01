#include "Sword.h"

#include <Actor/Player.h>
#include <Actor/Enemy/Enemy.h>
#include <Actor/Arrow.h>

using namespace Craft;
using SwordFrame = Sword::SwordFrame;

static const SwordFrame sword =
		{L"◈", 0.025f, Color::Red }; // 총소요 0.17초 7틱.

Sword::Sword(const Vector2& position, const std::vector<Vector2>& path,
				const std::weak_ptr<Actor>& handler)
	: super(sword.frame , Vector2::Zero, sword.color), handler(handler)
{
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
	if (
		other->IsTypeOf<Player>()
		|| other->IsTypeOf<Enemy>()
		|| other->IsTypeOf<Arrow>()
		)
	{ 
		other->Destroy();
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

