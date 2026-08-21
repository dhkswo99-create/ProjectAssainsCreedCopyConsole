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
	isSighted = true;
	timer.SetTargetTime(0.07f);
	for (Vector2 path : arrowPath)
	{
		Vector2 face = FacingDirection(path, destination);
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

	if (!timer.IsTimeOut())
	{
		return;
	}

	if (effectSequenceCount - 1 == currentIndex)
	{
		Destroy();
		return;
	}

	timer.Reset();

	ChangeImage(arrowQueue[currentIndex].frame);
	SetPosition(arrowPos[currentIndex]);
	
	++currentIndex;
}

void Arrow::OnCollision(const std::shared_ptr<Actor>& other)
{
	if (other->IsTypeOf<Player>()
		|| other->IsTypeOf<Guard>()
		)
	{
		other->Destroy();
	}
}

Vector2 Arrow::FacingDirection(const Vector2& currentPosition, const Vector2 destination)
{
	Vector2 rightVector = Vector2(1, 0);
	float innerProduct = static_cast<float>(
		(destination.x - currentPosition.x) * rightVector.x
		+ (destination.y - currentPosition.y) * rightVector.y
		);
	float rayDistance = static_cast<float>(std::sqrt(
		std::pow(destination.x - currentPosition.x, 2)
		+ std::pow(destination.y - currentPosition.y, 2)
	));
	float absFace = static_cast<float>(std::sqrt(
		std::pow(rightVector.x, 2)
		+ std::pow(rightVector.y, 2)
	));
	double facingAngle = 0;
	if (rayDistance > 0)
	{
		facingAngle = acos(innerProduct / (rayDistance * absFace)) * ANGLE;
	}
	if (destination.y - currentPosition.y < 0)
	{
		if (facingAngle < 23)
		{
			return Vector2(1, 0);
		}
		else if (facingAngle > 23
			&& facingAngle < 68)
		{
			return Vector2(1, -1);
		}
		else if (facingAngle > 68
			&& facingAngle < 113)
		{
			return Vector2(0, -1);
		}
		else if (facingAngle > 113
			&& facingAngle < 158)
		{
			return Vector2(-1, -1);
		}
		else if (facingAngle > 158)
		{
			return Vector2(-1, 0);
		}
	}
	else
	{
		if (facingAngle < 23)
		{
			return Vector2(1, 0);
		}
		else if (facingAngle > 23
			&& facingAngle < 68)
		{
			return Vector2(1, 1);
		}
		else if (facingAngle > 68
			&& facingAngle < 113)
		{
			return Vector2(0, 1);
		}
		else if (facingAngle > 113
			&& facingAngle < 158)
		{
			return Vector2(-1, 1);
		}
		else if (facingAngle > 158)
		{
			return Vector2(-1, 0);
		}
	}
	return Vector2(0, 0);
}