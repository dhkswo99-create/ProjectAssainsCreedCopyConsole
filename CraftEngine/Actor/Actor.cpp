#include "Actor.h"
#include <Engine/Engine.h>
#include <Render/Renderer.h>

namespace Craft
{
	Actor::Actor(
		const std::wstring& image,
		const Vector2& position,
		Color color	
	)
		: image(image), position(position), color(color),
		width(static_cast<int>(image.length()))
	{

	}
	Actor::~Actor()
	{
	}
	void Actor::BeginPlay()
	{
		//이벤트 처리했다고 설정
		hasBeganPlay = true;
	}
	void Actor::Tick(float deltaTime) //필요한 곳에서 구현해서 사용
	{

	}
	void Actor::Draw() //필요한 곳에서 구현해서 사용
	{
		//비활성 상태이면 종료
		if (!IsActive())
		{
			return;
		}

		//렌더러에 필요한 데이터 제출.
		Renderer::Get().Submit(
			image, position, color, sortingOrder, isSighted);
	}
	void Actor::OnCollision(const std::shared_ptr<Actor>& other)
	{
	}
	void Actor::Destroy()
	{
		hasExpired = true;
	}
	void Actor::QuitGame()
	{
		//엔진 종료 요청
		Engine::Get().Quit(); // Get을 스태틱에 참조로 반환한 이유 객체 자체를 Get
	}
	void Actor::SetPosition(const Vector2& newPosition)
	{
		//변경하려는 위치값이 기존값과 동일하면 종료
		if (position == newPosition)
		{
			return;
		}
		position = newPosition; //연산자 모두 오버로딩한 연산자.
	}
	void Actor::SetFace(const Vector2& newFace)
	{
		//변경하려는 위치값이 기존값과 동일하면 종료
		if (face== newFace)
		{
			return;
		}
		face= newFace; //연산자 모두 오버로딩한 연산자.
	}
}