#include "Engine.h"
#include <Level/Level.h>
#include <Input/Input.h>
#include <Render/Renderer.h>
#include <Physics/CollisionSystem.h>

#include <iostream>
#include <windows.h> //언리얼에서 chrono를 사용하지 않아서 해당 라이브러리로 시계 기능을 사용하지 X
#include <cassert>

namespace Craft
{
	//전역 변수 초기화
	Engine* Engine::instance = nullptr;

	Engine::Engine()
	{
		//instance 초기화
		assert(!instance && "instance is not null"); //!instance <=> instance == nullptr
		instance = this; //하나만 만들것

		//엔진 설정 로드
		LoadEngineSetting();

		//입력 개체 생성
		input = std::make_unique<Input>();
		
		//렌더러 객체 생성 >> 원래 시점에 맞게생성하는데 간단한 예제이므로 생성자에서 생성.
		int renderWidth = setting.gameWidth + setting.itemWidth;
		int renderHeight = setting.gameHeight + setting.itemHeight;
		renderer = std::make_unique<Renderer>(
			Vector2(setting.gameWidth, setting.gameHeight),
			Vector2(setting.itemWidth, setting.itemHeight)
		);

		// 충돌 시스템 객체 생성
		collisionSystem = std::make_unique<CollisionSystem>();
	}
	Engine::~Engine()
	{
		instance = nullptr;
	}
	void Engine::Run()
	{
		//고해상도 타이머 사용.
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		//현재시간 읽기
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter); //현재 시간 반환 함수(매 단위시간 카운터) 
		//프레임 계산 변수
		int64_t current = counter.QuadPart; // QuadPart -> 64비트 지원 시 사용 
		int64_t previous = current;
		// 고정 프레임으로 만들기 위한 값
		float oneFrameTime = 1.0f / setting.framerate; // <<까지 프레임 시간 구하는 코드


		//엔진루프
		while(true)
		{	
			//종료 조건 처리
			if (isQuit)
			{
				break;
			}

			//입력처리
			ProcessInput(); //입력을 큐에 저장하는 형태가 아닐까 싶음
			//저장된 입력을 한 번에 처리하기도 하고. 
			
			//프레임 시간 계산
			//1. 현재 시간 읽기
			QueryPerformanceCounter(&counter);
			current = counter.QuadPart;
			//2.현재시간 - 이전시간 ) / 시간 단위 -> 초단위 변환	
			float deltaTime
				= static_cast<float>(current - previous) //델타타임이 소수점으로(s 단위) 나타나게 하기 위해
				// float 자료형변환 
				/ static_cast<float>(frequency.QuadPart);
			// 고정 프레임 처리
			if (deltaTime >= oneFrameTime) // 해당 시간이 되기까지 previous를 업데이트하지 않음.
				//즉 프레임 시간을 제한하는 역할을 함 >> 처리 성능 향상과 일관성.
			{
				//게임이벤트 함수 호출
				OnInitialized();
				//게임 이벤트 초기화 1회 호출
				BeginPlay();
				//게임업데이트
				Tick(deltaTime);
				//충돌처리
				ProcessCollision();
				//화면 그리기
				Draw();
			
				//여기까지가 프레임처리완료.
				//레벨전환처리
				if (nextLevel) //해당 매커니즘 중요
					//요청을 다음 프레임까지 미뤄서 처리 -> 문제 발생 가능성 낮춤.
				{
					//기존레벨 정리
					if (mainLevel)
					{
						mainLevel.reset();
					}
					//추가 요청된 레벨을 메인 레벨로 설정.
					mainLevel = nextLevel;
					//포인터 정리
					nextLevel.reset(); // ptr = nullptr; , 참조값 하나 --;
				}

				// 추가/제거 요청된 액터 정리.
				if (mainLevel)
				{
					mainLevel->ProcessAddAndDestroyActors();
					mainLevel->SavePreviousActorStates();
				}
				//입력상태 저장
				SavePreviousInputStates();
				//현재 시간을 이전시간으로 저장
				previous = current;
			}
		}

		Shutdown();
	}
	void Engine::Quit()
	{	
		// 엔진 종료 플래그 설정.
		isQuit = true;
	}

	Engine& Engine::Get()
	{
		assert(instance && "instance is null");// 검증 어서트
		//무조건(필수로) 통화해야하는 조건이 있을 때 사용
		//디버그 모드에서만 동작.
		return *instance;
	}
	void Engine::ProcessInput()
	{
		assert(input && "input should not be null here.");
		if (!input)
		{
			return;
		}

		input->ProcessInput();
	}
	void Engine::OnInitialized()
	{
		if(!mainLevel || mainLevel->HasInitialized())
		{
			return;
		}
		// 초기화 이벤트 호출.
		mainLevel->OnInitialized();
	}
	void Engine::BeginPlay()
	{
		if (!mainLevel)
		{
			return;
		}
		//레벨에 이벤트 전달
		mainLevel->BeginPlay();
	}
	void Engine::Tick(float deltaTime)
	{
		if (!mainLevel)
		{
			return;
		}
		mainLevel->Tick(deltaTime);
	}
	void Engine::Draw()
	{
		if (!mainLevel)
		{
			return;
		}
		
		mainLevel->Draw();

		if (!renderer)
		{
			return;
		}

		renderer->Draw();
	}
	void Engine::ProcessCollision()
	{
		if (!mainLevel || !collisionSystem)
		{
			return;
		}
		
		//충돌 처리
		//의존성 주입 -> 액터리스트를 관리할 필요 없이 받아서 사용하는 것.
		collisionSystem->ProcessCollision(mainLevel->collisionEnabledActorList);

	}
	void Engine::SavePreviousInputStates()
	{
		assert(input && "input should not be null here.");
		if (!input)
		{
			return;
		}

		input->SavePreviousStates();
	}
	void Engine::Shutdown()
	{
	}
	void Engine::LoadEngineSetting()
	{
		FILE* file = nullptr;
		fopen_s(&file, "../Config/Setting.txt", "rt");

		//예외처리
		if (!file)
		{
			std::cout << "Failed to open engine setting file.\n";
			//디버그모드에서 강제 중단 시키는 기능
			__debugbreak();
			return;
		}
		//데이터 읽어오기
		const int bufferSize = 2048;
		char buffer[bufferSize] = {};
		size_t readSize =
			fread(buffer, sizeof(char), bufferSize, file);

		//값 저장을 위해 서식 해석 파싱
		//문자열 자르기 Split
		char* context = nullptr;
		char* token = nullptr;
		//파일에서 읽은 전체 문자열을 개행 문자 기준으로 처음 자르기.
		token = strtok_s(buffer, "\n", &context);

		//반복
		while (token)
		{
			//공백 전까지 읽은 문자열을 저장할 변수
			char key[15] = {};

			//포맷을 지정한 문자열 읽기
			sscanf_s(token, "%s", key, 15);

			//키 값을 비교해서 값 설정
			if (strcmp(key, "framerate") == 0)
			{
				sscanf_s(token, "framerate = %f", &setting.framerate);
			}
			else if (strcmp(key, "gameWidth") == 0)
			{
				sscanf_s(token, "gameWidth = %d", &setting.gameWidth);
			}
			else if (strcmp(key, "gameHeight") == 0)
			{
				sscanf_s(token, "gameHeight = %d", &setting.gameHeight);
			}
			else if (strcmp(key, "itemWidth") == 0)
			{
				sscanf_s(token, "itemWidth = %d", &setting.itemWidth);
			}
			else if (strcmp(key, "itemHeight") == 0)
			{
				sscanf_s(token, "itemHeight = %d", &setting.itemHeight);
			}
			
			//나머지 문자열 자르기
			token = strtok_s(nullptr, "\n", &context);

		}
		fclose(file);
		file = nullptr;
	}
}