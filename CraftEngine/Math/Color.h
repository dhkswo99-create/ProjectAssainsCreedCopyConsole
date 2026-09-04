#pragma once

#include <Core/Core.h>
#include <Windows.h>

namespace Craft
{
	//색상을 열거형으로 정의
	enum class CRAFT_API Color : WORD //ushort type
	{
		// 매핑
		Black = FOREGROUND_RED - FOREGROUND_RED, //ⓐ
		Blue = FOREGROUND_BLUE, //ⓑ
		Green = FOREGROUND_GREEN,	//ⓒ
		Cyan = Green + Blue,	//ⓓ
		Red = FOREGROUND_RED,	//ⓔ
		Purple = Red + Blue,	//ⓕ
		Yellow = Red + Green,	//ⓖ
		Gray = Red + Green + Blue,	//ⓗ

		BrightBlack = Black + FOREGROUND_INTENSITY, 	//ⓙ
		BrightBlue = Blue + FOREGROUND_INTENSITY,	//ⓚ
		BrightGreen = Green + FOREGROUND_INTENSITY,	//ⓛ
		BrightCyan = Cyan + FOREGROUND_INTENSITY,	//ⓜ
		BrightRed = Red + FOREGROUND_INTENSITY,	//ⓝ
		BrightPurple = Purple + FOREGROUND_INTENSITY,	//ⓞ
		BrightYellow = Yellow + FOREGROUND_INTENSITY,	//ⓟ
		White = Gray + FOREGROUND_INTENSITY,	//ⓠ 	
		// 매핑
		bBlack = BACKGROUND_RED - BACKGROUND_RED, //ⓐ
		bBlue = BACKGROUND_BLUE, //ⓑ
		bGreen = BACKGROUND_GREEN,	//ⓒ
		bCyan = Green + Blue,	//ⓓ
		bRed = BACKGROUND_RED,	//ⓔ
		bPurple = Red + Blue,	//ⓕ
		bYellow = Red + Green,	//ⓖ
		bGray = Red + Green + Blue,	//ⓗ

		bBrightBlack = Black + BACKGROUND_INTENSITY, 	//ⓙ
		bBrightBlue = Blue + BACKGROUND_INTENSITY,	//ⓚ
		bBrightGreen = Green + BACKGROUND_INTENSITY,	//ⓛ
		bBrightCyan = Cyan + BACKGROUND_INTENSITY,	//ⓜ
		bBrightRed = Red + BACKGROUND_INTENSITY,	//ⓝ
		bBrightPurple = Purple + BACKGROUND_INTENSITY,	//ⓞ
		bBrightYellow = Yellow + BACKGROUND_INTENSITY,	//ⓟ
		bWhite = Gray + BACKGROUND_INTENSITY,	//ⓠ 	
	};
}
