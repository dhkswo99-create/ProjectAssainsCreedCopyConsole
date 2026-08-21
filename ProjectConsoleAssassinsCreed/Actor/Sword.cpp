#include "Sword.h"

#include <Actor/Player.h>
#include <Actor/Enemy/Enemy.h>
#include <Actor/Arrow.h>

using namespace Craft;
using SwordFrame = Sword::SwordFrame;

static const SwordFrame sword =
		{L"◈", 0.2f, Color::Red };


Sword::Sword(const Vector2& position)
	: Actor( sword.frame , position, sword.color)
{
	isSighted = true;
	sortingOrder = 1;
	timer.SetTargetTime(sword.playTime);
	//충돌 허용
	SetColiisionEnabled(true);
}

void Sword::OnCollision(const std::shared_ptr<Actor>& other)
{
	super::OnCollision(other);

	if (other->IsTypeOf<Player>()
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
	if (!timer.IsTimeOut())
	{
		return;
	}
	Destroy();

	timer.Reset();
}

