#pragma once

#include <Core/Core.h>
#include <Core/CraftObject.h>
#include <Math/Color.h>
#include <Math/Vector2.h>
#include <string>
#include <memory> // std:: weak_ptr 사용

namespace Craft
{
	//전방선언
	class Level;
	//가상 공간에 배치될 모든 액터의 기본 클래스

	class CRAFT_API Actor : public CraftObject
	{
		TYPE_DECLARATIONS(Actor, CraftObject)
	public:
		Actor(
			const std::wstring& image = L"",
			const Vector2& position = Vector2::Zero,
			Color color = Color::White
		);
		virtual ~Actor();

		//게임플레이 이벤트 함수
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		//충돌 이벤트 함수
		virtual void OnCollision(const std::shared_ptr<Actor>& other);

		//액터 제거 함수
		void Destroy();
		// 게임 엔진 종료 함수
		void QuitGame();

		//게터 세터
		inline bool HasBeganPlay() const { return hasBeganPlay;}

		inline bool IsActive() const { return isActive && !hasExpired; }

		inline bool HasExpired() const { return hasExpired; }

		inline std::shared_ptr<Level> GetOwner() const { return owner.lock(); }; // 오너가 없다면 null
		//반환
		inline void SetOwner(std::weak_ptr<Level> newOwner) { owner = newOwner; }

		inline Vector2 GetPosition() const { return position; }
		inline Vector2 GetFace() const { return face; }
		
		inline bool GetIsSighted() const { return isSighted; }
		inline void SetIsSighted(bool newIsSighted) { this->isSighted =  newIsSighted; }
		inline bool GetKeepSighted() const { return keepSighted; }
		void SetKeepSighted(bool newKeepSighted) { this->keepSighted =  newKeepSighted; }

		void SetPosition(const Vector2& newposition);
		void SetFace(const Vector2& newFace);
		void SetColor(const Color newcolor) { this->color = newcolor; }

		void SetColiisionEnabled(bool iscollisionEnabled)
		{
			collisionEnabled = iscollisionEnabled;
		}

		bool GetCollisionEnabled() { return collisionEnabled; }

		//이전 위치 반환
		inline Vector2 GetPreviousPosition() const { return previousPosition; }

		//프레임 종료 후 이전 프레임 위치 저장 함수
		inline void SavePreviousState() { previousPosition = position; }

		//너비 반환
		inline int GetWidth() const { return width; }

		//액터의 이미지 설정 함수
		inline void ChangeImage(const std::wstring& newImage)
		{
			// 이미지 길이 설정
			for (wchar_t cha : newImage)
			{
				++width;
				if (cha == '\n')
				{
					++height;
					width = 0;
				}
			}

			// 새로운 글자값 설정
			image = newImage;
		}



	protected:
		bool keepSighted;
		bool isSighted;
		//땅인지
		bool bIsGround = false;
		//충돌 가능 여부
		bool collisionEnabled = false;
		//BeginPlay 생에 한번만 처리
		bool hasBeganPlay = false;
		//액터 활성화 여부
		bool isActive = true;
		//삭제 요청 여부 플래그.
		bool hasExpired = false;
		//오너십 추가 - 이 액터를 소유하는 레벨 객체
		//weak_ptr -> 약참조 -> 실제 사용을 위해서는 해당 위치가 유효한지 확인해야함.
		std::weak_ptr<Level> owner; //사고참조 방지.

		//화면에 그릴 글자
		std::wstring image;
		
		//글자 색상
		Color color = Color::White;

		//글자 길이
		int width = 0;
		int height = 1;

		//렌더링 순서
		int sortingOrder = 0;

		//위치
		Vector2 position;
		Vector2 face;

		//이전 프레임 위치
		Vector2 previousPosition;
	};

}
