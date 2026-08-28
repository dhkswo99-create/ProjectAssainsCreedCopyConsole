#pragma once

#include <Actor/ACTOR.H>
#include <Util/Timer.H>
#include <vector>

using namespace Craft;
class Sword : public Actor
{
	TYPE_DECLARATIONS(Sword, Actor)

	struct SwordFrame
	{
		SwordFrame(
			const std::wstring& frame, //글자값
			float playTime = 0.0f, //유지 시간
			Craft::Color color = Craft::Color::Red) //색상
			: frame(frame), playTime(playTime), color(color)
		{
		}

		~SwordFrame() = default;

		// 문자열 변수 (화면에 보여줄 문자열).
		std::wstring frame;

		// 재생 시간.
		float playTime = 0.0f;

		// 색상.
		Craft::Color color = Craft::Color::White;
	};

public:
	Sword(const Vector2& position, const std::vector<Vector2>& path);
	~Sword() = default;
	virtual void OnCollision(const std::shared_ptr<Actor>& other) override;

private:
	virtual void Tick(float deltaTime) override;

private:
	int currentIndex;
	int effectSequenceCount;
	// 애니메이션 재생에 사용할 타이머.
	// 시퀀스 사이에 시간 계산용.
	Timer timer;
	std::vector<Vector2> swordPos;
};

