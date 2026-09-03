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

void Client::BeAttacked(const Vector2& face, int damage)
{
	//if (!invincibilityTimer.IsTimeOut())
	//{
	//	return;
	//}
	//// 체력 감소
	//this->hp -= damage;
	//// 넉백 
	//std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	//if (level->CanMove(GetPosition() + face))
	//{
	//	SetPosition(GetPosition() + face);
	//	xPosition = GetPosition().x + face.x;
	//	yPosition = GetPosition().y + face.y;
	//}

	//// 체력 0 이하
	//if (this->hp <= 0)
	//{
	//	// 플레이어가 휘두른 칼 삭제 -> 필요한가? 싶긴 함
	//	swordSet.clear();
	//	// 플레이어 소멸
	//	Destroy();
	//}
	//invincibilityTimer.Reset();
}

void Client::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}
