#include "Target.h"

#include <Render/Renderer.h>
#include <random>


using namespace Craft;
Target::Target(const Vector2& position)
	: super(L"T", position, Color::White)
{
	hp = 500;
	sortingOrder = 3;
	sightDegree = 40;
	sightRange = 15;
	range = 10;
	moveSpeed = 20.f;
	patternDelay.SetTargetTime(1.0f);
	invincibilityTimer.SetTargetTime(0.2f);
	for (int ix = 0; ix < range; ++ix)
	{
		swordRoute.emplace_back();
	}
	// 충돌 가능 객체
	SetColiisionEnabled(true);
}

Target::~Target()
{
	isDead = true;
}

void Target::DestroyWeapon()
{
	swordSet.clear();
	doAttack = false;
}

void Target::BeAttacked(const Vector2& face, int damage)
{
	// 무적시간
	if (!invincibilityTimer.IsTimeOut())
	{
		return;
	}
	if (!beBoss)
	{
		beBoss = true;
	}

	// 체력 감소
	this->hp -= damage;
	if (isGroggy)
	{
		this->hp -= damage;
	}	
	// 넉백 
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	if (level->CanMove(GetPosition() + face))
	{
		SetPosition(GetPosition() + face);
	}

	// 체력 0 이하
	if (this->hp <= 0)
	{
		// 타겟이 휘두른 칼 삭제 
		swordSet.clear();
		// 타겟 소멸
		Destroy();
	}
	invincibilityTimer.Reset();
}

void Target::DisplayHp()
{
	// 타겟 정보
	Renderer::Get().ScreenSubmit( 
		L"Target(",
		Vector2(1, 42), // offset 0, 1 -> 세부 조정
		Color::White,
		21,
		true
	); 
	Renderer::Get().ScreenSubmit( 
		L"/3)",
		Vector2(9, 42), // offset 0, 1 -> 세부 조정
		Color::White,
		21,
		true
	); 
	// Target (n/3) 
	if (beBoss)
	{
		Renderer::Get().ScreenSubmit(
			L"HP:",
			Vector2(13, 42), // offset 0, 1 -> 세부 조정
			Color::White,
			21,
			true
		);
		for (int ix = 0; ix < hp / 25; ++ix)
		{
			Renderer::Get().ScreenSubmit(
				L"■",
				Vector2(16 + ix, 42), // offset 0, 1 -> 세부 조정
				Color::Red,
				21,
				true
			);
		}
	}
}

void Target::MiniMapSubmit()
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	if (level->GetTargetClueCount() == 3)
	{
		Renderer::Get().ScreenSubmit( //플레이어 미니맵 처리
			L"T",
			Vector2(static_cast<int>(
				static_cast<float>(position.x)
				/ (static_cast<float>(level->GetMap().size()) / 49)),
				1 + static_cast<int>(
					static_cast<float>(position.y)
					/ (static_cast<float>(level->GetMap().size()) / 20))
			), // offset 0, 1 -> 세부 조정
			Color::White,
			20,
			true
		);
	}
}

void Target::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());

	invincibilityTimer.Tick(deltaTime);
	if (found && !beBoss)
	{
		bossTimer.Tick(deltaTime);
		beBoss = true;
		sightRange = 50;
		sightDegree = 180;
		// todo 보스 룸을 고립되게 만들 벽 객체 생성
	}
	if (found && !doAttack)
	{
		SetFace(FacingDirection(GetPosition()));
		pathDirection.clear();
		pathDirection = FindRoute(level->GetPlayerPosition());
	}
	MiniMapSubmit();
	DisplayHp();
	if (beBoss)
	{
		patternDelay.Tick(deltaTime);
		if (!patternDelay.IsTimeOut())
		{
			return;
		}
		SetMoveSpeed(10.f);
		std::vector<int> patternDamage;
		int patternRange = 10;
		

		// 모든 패턴 및 공격이 진행 중이지 않을 때
		if (!(doAttack
			|| farPattern
			|| nearFirstPattern
			|| nearSecondPattern
			|| supprsstionPattern)
			)
		{
			srand(deltaTime * 100000);
			if (distance > 6)
			{
				farPattern = true;
			}
			else
			{
				int randomNum = rand() % 10;
				// 40% 첫패턴
				if (randomNum > 5)
				{
					nearFirstPattern = true;
				}
			//	// 40% 두번째
			//	else if (randomNum > 1)
			//	{
			//		nearSecondPattern = true;
			//	}
			//	// 20% 밀어내기
			//	else
			//	{
			//		supprsstionPattern = true;
			//	}
			}
		}
		if (patternDamage.size() == 0)
		{
			for (int ix = 0; ix < range; ++ix)
			{
				patternDamage.emplace_back(10);
			}
		}
		//  멀면 추적해서 따라가서 전진 공격
		if (farPattern && !doAttack)
		{
			SetMoveSpeed(40.f);
			if (moveIndex > 0 && distance > 4)
			{
				Move(pathDirection[moveIndex], deltaTime);
			}

			if (distance <= 4)
			{
				CalcFarAttackPattern();
				patternDelay.Reset();
				farPattern = false;
				doAttack = true;
			}
		}
		// todo 가까울 때 연속 공격 로직 
		if (nearFirstPattern && !doAttack)
		{
			SetMoveSpeed(20.f);
			if (moveIndex > 0 && distance > 2)
			{
				Move(pathDirection[moveIndex], deltaTime);
			}
			if (distance <= 2)
			{
				if (!bDoSecondAttack)
				{
					for (int ix = 1; ix <= 4; ++ix)
					{
						if (!InsertSwordRoute(level, swordRoute[ix], face * (ix % 3))) return;
					}
					patternDelay.Reset();
					doAttack = true;
					bDoSecondAttack = true;

				}
				else
				{
					CalcNearFirstPattern();
					patternDelay.Reset();
					nearFirstPattern = false;
					bDoSecondAttack = false;
					doAttack = true;
				}
			}
		}

		// todo 가까울 때 연속 공격 로직 2
		if (nearSecondPattern && !doAttack)
		{
			CalcNearSecondPattern();
			patternDelay.Reset();
			nearSecondPattern = false;
			doAttack = true;
			
		}
		// todo 밀어내기 회전격
		if (supprsstionPattern && !doAttack)
		{
			CalcSpinningSlash();
			patternDelay.Reset();
			supprsstionPattern = false;
			doAttack = true;
		}

		if (doAttack)
		{
			patternDelay.Reset();
			Attack(patternDamage);
			doAttack = false;
		}




	}
}

void Target::Attack(std::vector<int>& damage)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	std::shared_ptr<Level> owner = GetOwner();
	Vector2 currentPos = GetPosition();
	if (owner)
	{
		for (int ix = 0; ix < 4; ++ix)
		{
			swordSet.emplace_back(
				owner->SpawnActor<Sword>(GetPosition(), swordRoute[ix], weak_from_this(), damage[ix])
			);
			swordRoute[ix].clear();
		}
	}
	damage.clear();
}

Vector2 Target::CalMatrix(const Vector2& face, int matrix0, int matrix1, int matrix2, int matrix3)
{ // 바라보는 방향 기준으로 회전 계산 함수
	return Vector2( // 대각선을 바라보고 45도 계산이라면
		(face.x * face.y != 0 && matrix0 == 1) ?
		Vector2((face.x * matrix0 + face.y * matrix2) / 2,
			(face.x * matrix1 + face.y * matrix3) / 2)
		: Vector2(face.x * matrix0 + face.y * matrix2,
			face.x * matrix1 + face.y * matrix3));
}
// 작업 완료
void Target::CalcFarAttackPattern()
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());

	Vector2 negative90face = CalMatrix(face, negative90Degree[0], negative90Degree[1],
		negative90Degree[2], negative90Degree[3]);
	Vector2 negative45face = CalMatrix(face, negative45Degree[0], negative45Degree[1],
		negative45Degree[2], negative45Degree[3]);
	Vector2 positive45face = CalMatrix(face, positive45Degree[0], positive45Degree[1],
		positive45Degree[2], positive45Degree[3]);
	Vector2 positive90face = CalMatrix(face, positive90Degree[0], positive90Degree[1],
		positive90Degree[2], positive90Degree[3]);
	if (face.x == 0 || face.y == 0)
	{
		// First
		if (!InsertSwordRoute(level, swordRoute[0], positive45face * -1 + negative90face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], positive45face * -2 + negative90face)) return;
		if (!InsertSwordRoute(level, swordRoute[2], positive45face * -2 + negative90face)) return;
		if (!InsertSwordRoute(level, swordRoute[3], positive45face * -2 + negative90face)) return;
		// Second
		if (!InsertSwordRoute(level, swordRoute[0], negative90face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], negative90face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[2], positive45face * -1 + negative90face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[3], positive45face * -1 + negative90face * 2)) return;
		// Third
		if (!InsertSwordRoute(level, swordRoute[0], negative45face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], negative45face + negative90face)) return;
		if (!InsertSwordRoute(level, swordRoute[2], negative45face + negative90face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[3], negative90face * 4)) return;
		// Fourth
		if (!InsertSwordRoute(level, swordRoute[0], face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], face + negative45face)) return;
		if (!InsertSwordRoute(level, swordRoute[2], face + negative45face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[3], face + negative45face * 2 + negative90face)) return;
		// Fifth
		if (!InsertSwordRoute(level, swordRoute[0], positive45face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[2], face * 2 + negative45face)) return;
		if (!InsertSwordRoute(level, swordRoute[3], face * 2 + negative45face * 2)) return;
		// Sixth
		if (!InsertSwordRoute(level, swordRoute[0], positive45face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], positive45face + face)) return;
		if (!InsertSwordRoute(level, swordRoute[2], face * 3)) return;
		if (!InsertSwordRoute(level, swordRoute[3], face * 3 + negative45face)) return;
		// Seventh
		if (!InsertSwordRoute(level, swordRoute[0], positive90face + positive45face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], positive45face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[2], positive45face * 2 + face)) return;
		if (!InsertSwordRoute(level, swordRoute[3], positive45face * 1 + face * 3)) return;
	}
	else
	{
		// First
		if (!InsertSwordRoute(level, swordRoute[0], negative90face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[1], positive45face * -1 + negative90face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[2], positive45face * -1 + negative90face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[3], positive45face * -1 + negative90face * 2)) return;
		// Second
		if (!InsertSwordRoute(level, swordRoute[0], negative45face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], negative45face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[2], negative45face * 2 + negative90face)) return;
		if (!InsertSwordRoute(level, swordRoute[3], negative45face * 2 + negative90face * 2)) return;
		// Third
		if (!InsertSwordRoute(level, swordRoute[0], negative45face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], negative45face + face)) return;
		if (!InsertSwordRoute(level, swordRoute[2], negative45face * 2 + face)) return;
		if (!InsertSwordRoute(level, swordRoute[3], negative45face * 4)) return;
		// Fourth
		if (!InsertSwordRoute(level, swordRoute[0], face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[2], face * 2 + negative45face)) return;
		if (!InsertSwordRoute(level, swordRoute[3], face * 2 + negative45face * 2)) return;
		// Fifth
		if (!InsertSwordRoute(level, swordRoute[0], face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[2], face * 3)) return;
		if (!InsertSwordRoute(level, swordRoute[3], face * 4)) return;
		// Sixth
		if (!InsertSwordRoute(level, swordRoute[0], positive45face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], positive45face + face)) return;
		if (!InsertSwordRoute(level, swordRoute[2], positive45face + face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[3], positive45face + face * 3)) return;
		// Seventh
		if (!InsertSwordRoute(level, swordRoute[0], positive45face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], positive45face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[2], positive45face * 3)) return;
		if (!InsertSwordRoute(level, swordRoute[3], positive45face * 3 + face)) return;
	}
}

void Target::CalcNearFirstPattern()
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());

	Vector2 negative90face = CalMatrix(face, negative90Degree[0], negative90Degree[1],
		negative90Degree[2], negative90Degree[3]);
	Vector2 negative45face = CalMatrix(face, negative45Degree[0], negative45Degree[1],
		negative45Degree[2], negative45Degree[3]);
	Vector2 positive45face = CalMatrix(face, positive45Degree[0], positive45Degree[1],
		positive45Degree[2], positive45Degree[3]);
	Vector2 positive90face = CalMatrix(face, positive90Degree[0], positive90Degree[1],
		positive90Degree[2], positive90Degree[3]);
	if (face.x == 0 || face.y == 0)
	{
		// First
		if (!InsertSwordRoute(level, swordRoute[0], negative45face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], negative45face + face)) return;
		if (!InsertSwordRoute(level, swordRoute[2], negative45face + face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[3], negative45face + face * 3)) return;
		// Second
		if (!InsertSwordRoute(level, swordRoute[0], face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[2], face * 3)) return;
		if (!InsertSwordRoute(level, swordRoute[3], face * 4)) return;
		// Third
		if (!InsertSwordRoute(level, swordRoute[0], positive45face)) return;
		if (!InsertSwordRoute(level, swordRoute[1], positive45face + face)) return;
		if (!InsertSwordRoute(level, swordRoute[2], positive45face + face * 2)) return;
		if (!InsertSwordRoute(level, swordRoute[3], positive45face + face * 3)) return;
	}
	else
	{
			// First
			if (!InsertSwordRoute(level, swordRoute[0], negative90face)) return;
			if (!InsertSwordRoute(level, swordRoute[1], negative90face + face)) return;
			if (!InsertSwordRoute(level, swordRoute[2], negative45face + face * 2)) return;
			if (!InsertSwordRoute(level, swordRoute[3], negative45face + face * 3)) return;
			// Second
			if (!InsertSwordRoute(level, swordRoute[0], face)) return;
			if (!InsertSwordRoute(level, swordRoute[1], face * 2)) return;
			if (!InsertSwordRoute(level, swordRoute[2], face * 3)) return;
			if (!InsertSwordRoute(level, swordRoute[3], face * 4)) return;
			// Third
			if (!InsertSwordRoute(level, swordRoute[0], positive45face)) return;
			if (!InsertSwordRoute(level, swordRoute[1], positive45face + face)) return;
			if (!InsertSwordRoute(level, swordRoute[2], positive45face + face * 2)) return;
			if (!InsertSwordRoute(level, swordRoute[3], positive45face + face * 3)) return;
	}
}

void Target::CalcNearSecondPattern()
{
}

void Target::CalcSpinningSlash()
{
}

bool Target::InsertSwordRoute(std::shared_ptr<GameLevel>& level,
	std::vector<Vector2>& swordRoute, const Vector2& vector)
{
	if (!level->CanAttack(position, vector)) { return false; }
	swordRoute.emplace_back(position + vector);
	return true;
}

