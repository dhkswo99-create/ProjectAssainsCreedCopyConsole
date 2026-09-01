 #pragma once

#include <Core/Core.h>
#include <Windows.h>

namespace Craft
{
	class CRAFT_API Vector2
	{
	public:
		Vector2(int x = 0, int y = 0);
		~Vector2() = default;

		//연산자 오버로딩
		
		//Windows 콘솔 좌표계로 변환하는 연산자 오버로딩
		operator COORD() const;
		operator COORD();
		
		//일반 연산자 오버로딩.
		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;
		Vector2 operator*(const Vector2& other) const;
		Vector2 operator*(const int multuplier) const;
		Vector2 operator/(const Vector2& other) const;

		//대입연산자 오버로딩
		Vector2& operator=(const Vector2& other);

		//비교연산자 오버로딩
		bool operator==(const Vector2& other) const;
		bool operator!=(const Vector2& other) const;



		//자주사용할 값을 스태틱으로 지정
		static Vector2 Zero;
		static Vector2 One;
		static Vector2 Right;
		static Vector2 Left;
		static Vector2 Up; //(0, -1)




	public:
		int x = 0;
		int y = 0;
	};

}

