#pragma once

#include <Core/Core.h>
#include <memory> //스마트 포인터 사용을 위해서

// CraftEngine 프로젝트 안의  클래스는 Craft 네임 스페이스 사용.
namespace Craft
{
	//전방선언 
	class Level;
	class Input; //포인터류의 변수들을 관리하기 위함
	class Renderer;
	class CollisionSystem;

	// 메인 엔진 클래스.
	// 엔진 루프를 제공.
	// 게임 엔진의 핵심 기능 제공.
	class CRAFT_API Engine
	{
		struct Setting
		{
			// 목표 프레임 수 (초당 프레임).
			float framerate = 120.0f;
			
			//사용할 콘설 화면 너비
			int gameWidth = 0;

			//사용할 콘솔 화면 높이
			int gameHeight = 0;

			//스크린 사이즈
			int itemWidth = 0;

			int itemHeight = 0;
		};

	public:	

		Engine();
		virtual ~Engine();

		// 엔진 실행 함수.
		void Run();

		// 엔진 종료 함수.
		void Quit();

		//레벨 추가 요청 함수
		//공부할 때 팁
		//1. std::is_base_of
		//2. sdt::enable_if_t
		//3. typename = std::enable_if_t<std::is_base_of<Level, T>::value>>
		template<typename T, 
			typename = std::enable_if_t<std::is_base_of<Level, T>::value>>
		void AddNewLevel()
		{
			nextLevel = std::make_shared<T>();
		}


		//전역 접근 함수
		static Engine& Get();

		//게터
		inline int GetGameWidth() const { return setting.gameWidth; }
		inline int GetGameHeight() const { return setting.gameHeight; }
		inline int GetItemWidth() const { return setting.itemWidth; }
		inline int GetItemHeight() const { return setting.itemHeight; }


	protected:
		// 입력 처리 함수 (입력 폴링).
		void ProcessInput();

		// 초기화 함수.
		void OnInitialized();

		// 게임 플레이 이벤트 함수.

		// 게임 플레이 초기화 함수.
		void BeginPlay(); // 뜯어 고칠 때 virtual 

		// 게임 플레이 업데이트 함수.
		void Tick(float deltaTime);

		// 레벨 그리기 함수.
		void Draw();

		// 충돌 처리 함수
		void ProcessCollision();

		// 프레임 간 입력 값 저장을 위한 함수.
		void SavePreviousInputStates();

		// 엔진 종료 시 정리가 필요할 때 사용할 함수.
		void Shutdown();
		
		//엔진 설정 로드 함수
		void LoadEngineSetting();

	protected:
		bool isQuit = false;

		Setting setting;
		//전역 접근이 가능하도록 변수 선언
		static Engine* instance;
		
		//메인 레벨
		std::shared_ptr<Level> mainLevel;

		//추가 요청된 레벨
		std::shared_ptr<Level> nextLevel;

		//입력 시스템 변수
		std::unique_ptr<Input> input; //변수 공유 필요 X >> 전역 접근

		//렌더러
		std::unique_ptr<Renderer> renderer;

		// 충돌 객체 추가
		std::unique_ptr<CollisionSystem> collisionSystem;
	};
}