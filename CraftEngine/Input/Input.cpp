#include <Input/Input.h>
#include <cassert>
#include <Windows.h>

namespace Craft
{
	Input* Input::instance = nullptr;

	Input::Input()
	{
		assert(!instance && "instance should be null here");
		instance = this;
	}
	bool Input::GetKeyDown(int keyCode) const
	{
		return !keyStates[keyCode].wasKeyDown
			&& keyStates[keyCode].isKeyDown;
	}
	bool Input::GetKeyUp(int keyCode) const
	{
		return keyStates[keyCode].wasKeyDown
			&& !keyStates[keyCode].isKeyDown;
	}
	bool Input::GetKey(int keyCode) const
	{

		return keyStates[keyCode].isKeyDown; // 원래 이전, 지금 둘다 눌린 것을 구현해야 함.
	}
	Input& Input::Get()
	{
		//여기에서는 instance는 null이면 안됨.
		assert(instance && "instance should not be null here.");
		return *instance;
	}

	void Input::ProcessInput()
	{
		//현재 프레임에 키 입력이 발생했는지 확인.
		for (int ix = 0; ix < keyCount; ++ix) 
		{
			//키 눌림 저장.
			//0x8000은 최상위 비트.
			keyStates[ix].isKeyDown = ( (GetAsyncKeyState(ix) & 0x8000) != 0 );
		}
	}
	void Input::SavePreviousStates()
	{
		//이전 프레임 입력 값 저장.
		for (KeyState& state : keyStates)
		{
			state.wasKeyDown = state.isKeyDown;
		}
	}
}