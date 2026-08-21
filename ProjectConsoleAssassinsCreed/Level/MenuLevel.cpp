#include "MenuLevel.h"
#include <Game/Game.h>
#include <Input/Input.h>
#include <Actor/Camera.h>
#include <Render/Renderer.h>
#include <cassert>

using namespace Craft;

MenuLevel::MenuLevel()
{
	//메뉴 아이템 생성 
	itemList.emplace_back(
		std::make_unique<MenuItem>(
			L"Restart Game",
			[]()
			{
				//메뉴 토글 함수 호출
				Game& game = dynamic_cast<Game&>(Engine::Get());
				game.RestartGame();
			}
		)
	);
	itemList.emplace_back(
		std::make_unique<MenuItem>(
			L"Resume Game",
			[]()
			{
				//메뉴 토글 함수 호출
				Game& game = dynamic_cast<Game&>(Engine::Get());
				game.ToggleMenu();
			}
		)
	);
	itemList.emplace_back(
		std::make_unique<MenuItem>(
			L"Quit Game",
			[]()
			{
				// 게임 종료 호출
				Engine::Get().Quit();
			}
		)
	);
}

void MenuLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);

	Game& game = dynamic_cast<Game&>(Engine::Get());
	if (game.isGameOver)
	{
		Renderer::Get().SetCameraView(Vector2::Zero); //TODO
		Renderer::Get().Submit(
			L"GameOver!!",
			Vector2::Zero,
			Color::White,
			0,
			true
		);
	}
	else if (game.targetClear)
	{
		Renderer::Get().SetCameraView(Vector2::Zero); //TODO
		Renderer::Get().Submit(
			L"The target is dead.But was it really the right choice... ?",
			Vector2::Zero,
			Color::White,
			0,
			true
		);
	}
	else if (game.clientClear)
	{
		Renderer::Get().SetCameraView(Vector2::Zero); //TODO
		Renderer::Get().Submit(
			L"The client is dead.But was it really the right choice... ?",
			Vector2::Zero,
			Color::White,
			0,
			true
		);
	}
	//입력 처리 (위/아래 방향키, 엔터, ESC 키)
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		Game& game = dynamic_cast<Game&>(Engine::Get());
		game.ToggleMenu();

		//인덱스 초기화
		currentIndex = 0;
	}

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

void MenuLevel::Draw()
{
	/*
	* Sokoban Game
	* 
	* Resume Game
	* Exit Game
	*/

	//제목 그리기
	Renderer::Get().Submit(L"", Vector2::Zero);

	// 메뉴 아이템 그리기
	const int count = static_cast<int>(itemList.size());
	for (int ix = 0; ix < count; ++ix)
	{
		// 선택/ 미선택된 아이템 색상처리
		Color textColor = (ix == currentIndex)
			? selectedColor : unselectedColor;

		//아이템 그리기
		Renderer::Get().Submit(
			itemList[ix]->text,
			Vector2(0, 2 + ix),
			textColor,
			0,
			true
		);
	}
}
