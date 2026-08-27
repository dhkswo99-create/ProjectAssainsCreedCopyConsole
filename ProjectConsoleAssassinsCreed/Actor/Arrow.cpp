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
	{ L"→", 0.3f, Color::Red },
	{ L"↗", 0.3f, Color::Red },
	{ L"↑", 0.3f, Color::Red },
	{ L"↖", 0.3f, Color::Red },
	{ L"←", 0.3f, Color::Red },
	{ L"↙", 0.3f, Color::Red },
	{ L"↓", 0.3f, Color::Red },
	{ L"↘", 0.3f, Color::Red }
};

Arrow::Arrow(const Vector2& position, const std::vector<Vector2>& arrowPath)
	: super(L"a", position, Color::Red),
	arrowPos(arrowPath)
{

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
	SetPosition(arrowPos[currentIndex]);


	timer.Reset();
	
	++currentIndex;
}

void Arrow::OnCollision(const std::shared_ptr<Actor>& other)
{
		//받은 객체에서 자신의 방어력, 속성 등을 기반으로 계산해 데미지를 받고
		//체력이 모두 소진되면 죽는 것도 받은 객체 책임.
		//무기 객체를 생성할 때 객체의 데미지, 속성을 넘겨서 무기 객체의 데미지, 속성을 결정할것.
	if (other->IsTypeOf<Player>() // 체력 화살의 데미지, 속성 등을 넘기기.
		|| other->IsTypeOf<Guard>()
		)
	{
		other->Destroy();
	}
}
