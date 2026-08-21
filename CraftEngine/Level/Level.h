#pragma once

#include <Actor/Actor.h>
#include <Core/CraftObject.h>

#include <memory> // std::unique_ptr / std::shared_ptr
#include <vector> // std:: vector 동적 배열

namespace Craft
{
	//게임에 배치된모든 액터를 관리하는 클래스
	//public std::enable_shared_from_this<Level>
	// : shared_from_this / weak_from_this() 사용을 위해
	// : shared_from_this() - this 포인터를 shared_ptr로 변환
	// : weak_from_this() - this 포인터를 weak_ptr로 변환.



	class CRAFT_API Level
		: public CraftObject,
		public std::enable_shared_from_this<Level>
	{
		//커스텀 타입 설정
		TYPE_DECLARATIONS(Level, CraftObject)

		friend class Engine;

	public:
		Level();
		virtual ~Level();
		
		//초기화함수
		virtual void OnInitialized();

		//게임 플레이 이벤트 함수
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		//액터 추가 함수(템플릿). 이부분은 따라치고 모르겠으면 찾아보길 권장
		template<typename T, typename ...Args,
			typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>
			//위 코드를 쓰면 if의 조건에 따라 해당 템플릿을 유효하게 할지, 무효하게 할지 결정.
		std::shared_ptr<T> SpawnActor(Args&& ...args)
		{
			//새로운 객체 생성
			std::shared_ptr<T> newActor // 스마트 포인터는 생성 전용 함수가 존재
				= std::make_shared<T>(std::forward<Args>(args)...);

			//추가요청 목록에 포함
			addRequestedActorList.emplace_back(newActor);
			
			//TODO: 오너십 설정
			newActor->SetOwner(weak_from_this());
			

			//생성한 액터 반환
			return newActor;
		}


		//액터 검색 함수(템플릿).
		template<typename T,
			typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>
			std::shared_ptr<T> FindActor()
		{
			// 검색 - 형변환.
			for (const auto& actor : actorList)
			{
				// T타입으로 형변환 시도
				std::shared_ptr<T> targetActor
					= std::dynamic_pointer_cast<T>(actor);//액터가 T타입이 아니라면 null이 반환됨.
				if (targetActor)
				{
					return targetActor;
				}
				
			}
			//못찾은 경우 null 반환
			return nullptr;
		}


		inline bool HasInitialized() const {	 return hasInitialized; }

	protected:
		//이전 프레임에 추가/제거 요청된 액터 처리 함수.
		void ProcessAddAndDestroyActors();

		//액터의 이전 상태 처리 함수
		void SavePreviousActorStates();

	protected:
		bool hasInitialized = false;

		//레벨에 배치된 모든 액터
		std::vector<std::shared_ptr<Actor>> actorList; //sharedptr 소유권 이전 할 수 있는 포인터
		//충돌이 허용된 액터 리스트
		std::vector<std::shared_ptr<Actor>> collisionEnabledActorList; 

		//레벨에 추가 요청된 액터를 저장해두는 목록.
		//현재 프레임을 처리하는 과정에서 액터 추가 요청이 발생하면
		//해당 액터를 바로 추가하면 기존 액터 처리에 문제가 발생가능
		//현재프레임을 모두 처리한 후에 추가 요청된 액터를 actorList로 옮김.
		//큐 같은 시스템.
		std::vector<std::shared_ptr<Actor>> addRequestedActorList;
	};

}