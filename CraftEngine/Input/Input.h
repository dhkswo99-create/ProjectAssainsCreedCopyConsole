#pragma once
#include <Core/Core.h>

namespace Craft
{
	class CRAFT_API Input
	{
		//Engine 클래스에 friend 선언
		friend class Engine;

		//키 입력 상태를 저장하기 위한 구조체
		struct KeyState
		{
			// 현재 프레임에 키가 눌렸는 지 여부
			bool isKeyDown = false;
			
			// 이전 프레임에 키가 눌렸는 지 여부
			bool wasKeyDown = false;
									
		};
	public:
		Input();
		~Input() = default;
		
		//키 눌림/ 해제 여부 확인 함수.
		//이전 프레임에 안눌렸다가 이번 프레임에 누리면 true 반환.
		bool GetKeyDown(int keyCode) const;

		//키눌림 해제 확인
		bool GetKeyUp(int ketCode) const;
			
		//키눌림 유지 확인
		bool GetKey(int keyCode) const;

		//외부에서 접근이 가능하도록 하는 함수
		static Input& Get();

		
	private:
		//현재 프레임에 특정 키 입력이 발생했는지를 처리하는 함수
		//엔진에서 키 입력을 판단하고 저장한 후 액터는 그 데이터를 활용하여 이벤트 처리
		void ProcessInput();


		//이전 프레임의 키눌림 상태를 저장하는 함수.
		void SavePreviousStates();



	private:
		//가상 키의 수 (=처리할 키의 수) = 256;
		const int keyCount = 256;

		//키 상태를 관리할 배열.
		KeyState keyStates[256] = {};

		
		
		//전역 접근이 가능하도록 하는 변수 추가
		static Input* instance;





	};

}
