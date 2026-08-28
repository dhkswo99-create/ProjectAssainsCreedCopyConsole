#pragma once

#include <Math/Vector2.h>

#include <Windows.h>
#include <cwchar>

namespace Craft
{
	//이중 버퍼링 구현을 위한 화면 버퍼 클래스
	//콘솔 핸들을 관리
	class ScreenBuffer
	{
	public:
		ScreenBuffer(const Vector2& screenSize);
		~ScreenBuffer();

		//콘솔 초기화 - 화면 지우기
		void Clear() const;

		//전달된 글자 값 그리는 함수
		void Draw(const CHAR_INFO* const charInfo) const;

		inline HANDLE GetBuffer() const { return buffer; } 
		// HANDLE -> 보이드 포인터 모든 타입을 다 저장하기 위해서.
		private:
		//화면 버퍼 핸들
		HANDLE buffer = nullptr;

		//화면 크기
		Vector2 size;

	};

}

