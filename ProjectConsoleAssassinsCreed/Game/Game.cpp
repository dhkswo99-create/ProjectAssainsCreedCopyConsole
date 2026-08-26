#include "Game.h"
#include <Level/GameLevel.h>
#include <Level/MenuLevel.h>
#include <Level/StartLevel.h>
#include <Render/Renderer.h>


Game::Game()
{
	// 두 레벨 생성 및 배열에 추가 << Main에서 생성 필요 X
	levelList.emplace_back(std::make_shared<StartLevel>());
	levelList.emplace_back(std::make_shared<GameLevel>());
	levelList.emplace_back(std::make_shared<MenuLevel>());

	//시작상태설정
	state = State::Start;
	changeLevelRender();
	//게임 시작시 활성화할 레벨 설정
	mainLevel = levelList[(int)state];
}

//void Game::ToggleMenu()
//{
//	int stateIndex = static_cast<int>(state);
//	int nextState = 1 - stateIndex; //원마이너스
//
//	//레벨 설정 및 상태 값 업데이트 
//	mainLevel = levelList[nextState];
//	state = static_cast<State>(nextState);
//}

void Game::changeLevelRender()
{
	if (state == State::GamePlay)
	{
		Craft::Renderer::Get().SetIsGameLevel(true);
	}
	else
	{
		Craft::Renderer::Get().SetIsGameLevel(false);
	}
	Craft::Renderer::Get().RenderQueueClear();
}

void Game::PlayGame()
{
	//현재 상태를 게임 플레이로 변경
	state = State::GamePlay;
	//다음 프레임에 게임 레벨로 전환
	nextLevel = levelList[static_cast<int>(state)];
}
void Game::ToMenu()
{
	//현재 상태를 메뉴로 변경
	state = State::Menu;
	//다음 프레임에 메뉴 레벨로 전환
	nextLevel = levelList[static_cast<int>(state)];
} //start를 2로 두고 원마이너스로 토글이 낫나? 

void Game::RestartGame()
{
	// 현재 상태를 게임 플레이로 변경
	state = State::GamePlay;
	// 다음 프레임에 새로운 게임 레벨로 전환 요청
	nextLevel = levelList[static_cast<int>(state)] = std::make_shared<GameLevel>(); // 생성>리스트넣기>넥스트레벨대입
}


void Game::SetGameStatus(bool target, bool client, bool gameOver)
{
	targetClear = target;
	clientClear = client;
	isGameOver = gameOver;
}











