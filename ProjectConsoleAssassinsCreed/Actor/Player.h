#pragma once

#include <Actor/Actor.h>
#include <Util/Timer.h>
#include <vector>

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
	
	virtual void OnCollision(const std::shared_ptr<Actor>& other) override;


private:
	virtual void Tick(float deltaTime) override;

	// 검 궤적 계산 
	Craft::Vector2 CalMatrix(const Craft::Vector2& face, int matrix0, int matrix1, int matrix2, int matrix3);
	bool CalcSwordRoute(const Craft::Vector2& face, const Craft::Vector2& position);
	std::vector<Craft::Vector2> GetSwordNearRoute() { return swordNearRoute; }
	std::vector<Craft::Vector2> GetSwordMiddleRoute() { return swordMiddleRoute; }
	std::vector<Craft::Vector2> GetSwordFarRoute() { return swordFarRoute; }


private:
	//검 궤적 계산 변수
	int positive90Degree[4] = {0, -1, 1, 0};
	int positive45Degree[4] = {1, -1, 1, 1};
	int negative45Degree[4] = {1, 1, -1, 1};
	int negative90Degree[4] = {0, 1, -1, 0};
	std::vector<Craft::Vector2> swordNearRoute;
	std::vector<Craft::Vector2> swordMiddleRoute;
	std::vector<Craft::Vector2> swordFarRoute;

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

