#pragma once

#include <Actor/Actor.h>
#include <Util/Timer.h>

class Player : public Craft::Actor
{
	TYPE_DECLARATIONS(Player, Actor)
public:
	Player(const Craft::Vector2& position);
	~Player()
	{
		SetFace(Craft::Vector2::Zero);
	}
	void MiniMapSubmit();

	void Move(float directionX, float directionY, float deltaTime);
	
	//공격함수
	void Attack(const int range, const Craft::Vector2& face, float deltaTime);

	void SetRange(int newRange) { range = newRange; }


private:
	virtual void Tick(float deltaTime) override;

	// 공격 요청 변수
	bool doAttack = false;
	bool doneAttack = true;

	// 딜레이 변수 // 디폴트 0.2 
	float castDelay = 0.2f;
	float attackDelay = 0.2f;

	//버프 시간
	float buffDuration = 0.3f;

	// 딜레이 시간 
	Timer delay;

	// 스킬 지속시간
	Timer buff;
	
	// 사거리
	int range = 3;

	float xPosition = 0.0f;
	float yPosition = 0.0f;

	float dx = 0;
	float dy = 0;


	float moveSpeed = 10.0f;
};

