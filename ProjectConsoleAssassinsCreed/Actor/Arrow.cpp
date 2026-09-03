#include "Arrow.h"

#define ANGLE 180/3.14

#include <Actor/Enemy/Guard.h>
#include <Actor/Player.h>
#include <Render/Renderer.h>
#include <cmath>

using namespace Craft;
using ArrowFrame = Arrow::ArrowFrame;
static const ArrowFrame arrow[] =
{
	{ L"→", 0.3f, Color::Red , Vector2( 1, 0)},
	{ L"↗", 0.3f, Color::Red , Vector2( 1,-1)},
	{ L"↑", 0.3f, Color::Red , Vector2( 0,-1)},
	{ L"↖", 0.3f, Color::Red , Vector2(-1,-1)},
	{ L"←", 0.3f, Color::Red , Vector2(-1, 0)},
	{ L"↙", 0.3f, Color::Red , Vector2(-1, 1)},
	{ L"↓", 0.3f, Color::Red , Vector2( 0, 1)},
	{ L"↘", 0.3f, Color::Red , Vector2( 1, 1)}
};

Arrow::Arrow(const Vector2& position, const std::vector<Vector2>& arrowPath)
	: super(L"a", position, Color::Red),
	arrowPos(arrowPath)
{
	//디폴트 화살 데미지
	damage = 20;
	hp = 5;
	Vector2 destination = arrowPath[arrowPath.size() - 1];
	sortingOrder = 1;
	isSighted = true;
	timer.SetTargetTime(0.05f);
	int count = 0;
	Vector2 previousPath;
	Vector2 face;
	for (Vector2 path : arrowPath)
	{
		//Vector2 face = FacingDirection(path, destination);
		if (count == 0)
		{
			++count;
			previousPath = path;
			face = path - position;
		}
		else
		{
			face = path - previousPath;
			previousPath = path;
		}
		int faceCheck = face.x * 3 + face.y;
		switch (faceCheck)
		{
		case 1: //하단 ( x = 0 y = 1 ) 
			arrowQueue.emplace_back(arrow[6]);
			break;
		case 2: //우상단 ( x =1 y = -1 )
			arrowQueue.emplace_back(arrow[1]);
			break;
		case 3: //우 ( x = 1 y = 0 )
			arrowQueue.emplace_back(arrow[0]);
			break;
		case 4: //우하단 ( x = 1 y = 1 )
			arrowQueue.emplace_back(arrow[7]);
			break;
		case -1: //상단
			arrowQueue.emplace_back(arrow[2]);
			break;
		case -2: //좌하단
			arrowQueue.emplace_back(arrow[5]);
			break;
		case -3: //좌
			arrowQueue.emplace_back(arrow[4]);
			break;
		case -4: //좌상단
			arrowQueue.emplace_back(arrow[3]);
			break;
		}
	}
	effectSequenceCount = static_cast<int>(arrowPath.size());
	currentIndex = 0;
	//충돌 허용
	SetColiisionEnabled(true);
}


void Arrow::Tick(float deltaTime)
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

	ChangeImage(arrowQueue[currentIndex].frame);
	SetFace(arrowQueue[currentIndex].face);
	SetPosition(arrowPos[currentIndex]);


	timer.Reset();
	
	++currentIndex;
}

void Arrow::OnCollision(const std::shared_ptr<Actor>& other)
{
	// DoAttack에서 분기처리됨.
	// 화살을 쏜 주체한테는 발사되지 않음.
	DoAttack(other, damage);
}

void Arrow::DoAttack(const std::shared_ptr<Actor>& other, int damage)
{
	if (other->IsTypeOf<Player>() // 체력 화살의 데미지, 속성 등을 넘기기.
		|| other->IsTypeOf<Enemy>()
		|| other->IsTypeOf<Sword>()
		)
	{
		other->BeAttacked(GetFace(), damage);
	}
}

void Arrow::BeAttacked(const Vector2& face, int damage)
{
	this->hp -= damage;
	// 체력 0 이하
	if (this->hp <= 0)
	{
		// 화살 소멸
		Destroy();
	}
}
