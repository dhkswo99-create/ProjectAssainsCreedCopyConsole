#include "StartLevel.h"
#include <Game/Game.h>
#include <Input/Input.h>
#include <Actor/Camera.h>
#include <Render/Renderer.h>
#include <cassert>

using namespace Craft;

StartLevel::StartLevel()
{
	//메뉴 아이템 생성 
	itemList.emplace_back(
		std::make_unique<StartItem>(
			L"start Game",
			[]()
			{
				//메뉴 토글 함수 호출
				Game& game = dynamic_cast<Game&>(Engine::Get());
				game.PlayGame();
				game.changeLevelRender();
			}
		)
	);
	itemList.emplace_back(
		std::make_unique<StartItem>(
			L"Quit Game",
			[]()
			{
				// 게임 종료 호출
				Engine::Get().Quit();
			}
		)
	);
}

void StartLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);

	//배열의 요소개수
	const int length = static_cast<int>(itemList.size());
	if (Input::Get().GetKeyDown(VK_UP))
	{
		//인덱스 돌리기 (-방향)
		currentIndex = (currentIndex - 1 + length) % length;
	}
	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		//인덱스 돌리기 (+방향)
		currentIndex = (currentIndex + 1) % length;
	}

	//엔터 입력 처리 -> 현재 선택된 메뉴의 로직 실행
	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		//어서트
		assert(currentIndex >= 0
			&& currentIndex < (int)itemList.size()
			&& itemList[currentIndex]->onSelected
		);

		//메뉴아이템에 저장된 로직실행
		itemList[currentIndex]->onSelected();

	}
}

void StartLevel::Draw()
{
	/*
	* Sokoban Game
	*
	* Resume Game
	* Exit Game
	*/

	//제목 그리기
	Renderer::Get().ScreenSubmit(
		L"AssainsCreed2D\n"
		L"Product by WJ ",
		Vector2(53, 10),
		Color::White,
		0,
		true
	);

	// 메뉴 아이템 그리기
	const int count = static_cast<int>(itemList.size());
	for (int ix = 0; ix < count; ++ix)
	{
		// 선택/ 미선택된 아이템 색상처리
		Color textColor = (ix == currentIndex)
			? selectedColor : unselectedColor;

		//아이템 그리기
		Renderer::Get().ScreenSubmit(
			itemList[ix]->text,
			Vector2(55, 20 + ix),
			textColor,
			0,
			true
		);
	}
}
 