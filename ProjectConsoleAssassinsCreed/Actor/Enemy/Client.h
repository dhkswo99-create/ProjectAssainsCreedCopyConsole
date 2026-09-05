#pragma once

#include <Actor/ACTOR.H>
#include <Actor/Sword.H>
#include <Actor/Enemy/Enemy.h>

using namespace Craft;
class Client : public Enemy,
	public std::enable_shared_from_this<Actor>
{
	TYPE_DECLARATIONS(Client, Enemy)

public:
	Client(const Vector2& position);
	~Client();

	void MiniMapSubmit();
	void DisplayHp();




	virtual void BeAttacked(const Vector2& face, int damage) override;
	void Tick(float deltaTime) override;
	bool GetIsDead() { return isDead; }

private:
	// 회전 행렬 계산
	Craft::Vector2 CalMatrix(const Craft::Vector2& face,
		int matrix0, int matrix1, int matrix2, int matrix3);

	// 주어진 패턴대로 공격
	void Attack(int range, std::vector<int>& damage);

	// 첫 패턴 빠르게 다가가 전진 공격
	void CalcFarAttackPattern();

	// 근접 시 연속 공격 1
	void CalcNearFirstPattern();

	// 근접 시 연속 공격 2
	void CalcNearSecondPattern();

	// 밀어내기 공격
	void CalcSpinningSlash();

	// swordRoute에 들어갈 위치 검증 후 삽입
	bool InsertSwordRoute(std::shared_ptr<GameLevel>& level,
		std::vector<Vector2>& swordRoute, Vector2& vector);

private:
	// 회전 방향
	Vector2 positive90face;
	Vector2 positive45face;
	Vector2 negative45face;
	Vector2 negative90face;


	//보스 데미지 패턴 별로 추가 연산 진행 후 대입
	int damage = 20;

	// 보스 그로기 게이지
	int groggy = 100;

	// 피격 무적 시간 타이머
	Timer invincibilityTimer;
	// 패턴 딜레이
	Timer patternDelay;

	// 보스 전 돌입 시간
	Timer bossTimer;
	// 보스전 돌입 변수
	bool beBoss = false;

	// 공격 상태 변수
	bool doAttack = false;

	// 그로기 돌입
	bool isGroggy = false;

	// 패턴 돌입 변수
	bool farPattern = false; // 멀 때 가까이 빠르게 접근해 공격
	bool nearFirstPattern = false; // 가까울 때 연속 공격
	bool nearSecondPattern = false; // 가까울 때 연속 공격
	bool supprsstionPattern = false; // 밀어내는 공격


	// 패턴 생성을 위한 검 변수
	int positive90Degree[4] = { 0, -1, 1, 0 };
	int positive45Degree[4] = { 1, -1, 1, 1 };
	int negative45Degree[4] = { 1, 1, -1, 1 };
	int negative90Degree[4] = { 0, 1, -1, 0 };
	std::vector<std::vector<Vector2>> swordRoute;
	std::vector<std::shared_ptr<Sword>> swordSet;

	// 게임 상태 결정을 위한 변수
	bool isDead = false;
};

