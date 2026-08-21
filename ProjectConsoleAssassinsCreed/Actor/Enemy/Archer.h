#pragma once

#include <Util/Timer.h>
#include <Actor/Actor.h>
#include <Actor/Enemy/Enemy.h>

class Archer : public Enemy
{
	TYPE_DECLARATIONS(Archer, Enemy)
		                      
public:
	Archer(const Vector2& position);
	~Archer() = default;

	virtual void Tick(float deltaTime) override;

	void Attack(int range, const Vector2& face, float deltaTime);

	void WillAttack(); 

	void FacePlayer();
	bool InAttackRange() { return distance < range; }

	// 시야 범위 내에 Player가 발각된다면 Calling 상태로 진입
	// 이미 Call이 호출된 상태라면 Tracking 호출
	// 발각 위치를 갖고 간다.
	//void Calling(const Vector2& spotOfDetection);

	//// 모든 Enemy Awake, Move Call완료 시 caller = false;
	//void Call(const Vector2& spotOfDetection);
private:
	float runRange;

	float castDelay;
	float attackDelay;  // 랜덤으로 돌릴 것

	bool doAttack = false;
	bool doneAttack = true;

	Vector2 rightVector = Vector2::Right;

	double facingAngle;

	Timer delay;
	int range;
};
