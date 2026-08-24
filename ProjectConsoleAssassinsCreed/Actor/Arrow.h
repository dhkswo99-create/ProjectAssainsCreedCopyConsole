#pragma once

#include <Actor/Actor.h>
#include <Util/Timer.h>
#include <vector>

using namespace Craft;
class Arrow : public Actor
{
	TYPE_DECLARATIONS(Arrow, Actor)

	struct ArrowFrame
	{
		ArrowFrame(
			const std::wstring& frame, //글자값
			float playTime = 0.0f, //유지 시간
			Craft::Color color = Craft::Color::Red) //색상
			: frame(frame), playTime(playTime), color(color)
		{
		}

		~ArrowFrame() = default;

		// 문자열 변수 (화면에 보여줄 문자열).
		std::wstring frame;

		// 재생 시간.
		float playTime = 0.0f;

		// 색상.
		Craft::Color color = Craft::Color::White;
	};
public:
	Arrow(const Vector2& position, const std::vector<Vector2>& arrowPath);
	~Arrow() = default;
 
	virtual void OnCollision(const std::shared_ptr<Actor>& other) override;
	

private:
	virtual void Tick(float deltaTime) override;

private:
	int currentIndex;
	int effectSequenceCount;
	// 애니메이션 재생에 사용할 타이머.
	// 시퀀스 사이에 시간 계산용.
	Timer timer;

	// 화살 대기 큐 
	std::vector<ArrowFrame> arrowQueue;
	std::vector<Vector2> arrowPos;
};

