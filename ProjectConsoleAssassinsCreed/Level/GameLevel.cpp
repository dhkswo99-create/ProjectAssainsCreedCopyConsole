#include "GameLevel.h"
#include <Actor/Wall.h>
#include <Game/Game.h>
#include <Actor/Ground.h>
#include <Actor/Enemy/Guard.h>
#include <Actor/Enemy/Archer.h>
#include <Actor/Arrow.h>
#include <Actor/Enemy/Target.h>
#include <Actor/Enemy/Client.h>
#include <Actor/Player.h>
#include <Actor/Camera.h>
#include <Render/Renderer.h>

#include <iostream>
#include <cassert>

#define ANGLE 180/3.14

using namespace Craft;



bool GameLevel::CanMove(const Craft::Vector2& nextPosition)
{
	//게임 클리어인 경우 처리 안함
	if (isGameOver 
		|| targetClear
		|| clientClear)
	{
		return false;
	}

	
	// 이동하려는 곳에 벽이 있을 경우 
	// 이동하려는 곳에 이동 객체가 있는가
	for (const std::shared_ptr<Actor>& actor : collisionEnabledActorList)
	{
		if (actor->GetPosition() == nextPosition)
		{
			if (actor->IsTypeOf<Player>())
			{
				return false;
			}
			if (actor->IsTypeOf<Enemy>())
			{
				return false;
			}
			return true; // 박스는 이미 처리됨
		}
	}
	if (map[nextPosition.y][nextPosition.x])
	{
		return false;
	}
	else
	{
		return true;
	}
	return false; // 예상치 못한 처리 - 이동 불가
}

bool GameLevel::CanAttack(const Craft::Vector2& playerPosition, const Craft::Vector2& face)
{
	if (isGameOver
		|| targetClear
		|| clientClear)

	{
		return false;
	}

	//공격하려는 곳이 벽인 경우
	if (map[playerPosition.y + face.y][playerPosition.x + face.x])
	{
		return false;
	}
	else
	{
		return true;
	}
	return false;
}

void GameLevel::IsSighted()
{
	for (const std::shared_ptr<Actor>& actor : actorList)
	{
		//actor->SetIsSighted(SearchingActorGL(actor)); //실제 게임
		actor->SetIsSighted(true); //디버깅
	}
}

// 현재 위치가 벽인지
bool GameLevel::IsWall(const Craft::Vector2& currentPosition)
{
	//게임 클리어인 경우 처리 안함
	if (isGameOver
		|| targetClear
		|| clientClear)

	{
		return false;
	}

	// 플레이어가 이동하려는 곳에 벽이 있을 경우 
	if (currentPosition.x >= 0 && currentPosition.x >= 0)
	{
		if (map[currentPosition.y][currentPosition.x])
		{
			return true;
		}
	}
	//for (const std::shared_ptr<Actor>& actor : actorList)
	//{
	//	if (actor->GetPosition() == currentPosition)
	//	{
	//		if (actor->IsTypeOf<Wall>())
	//		{
	//			return true;
	//		}
	//	}
	//}
	return false; // 예상치 못한 처리 - 이동 불가
}

std::vector<Vector2> GameLevel::RayDirectionQueueInsertGL(const Vector2& actorPosition)
{
	std::vector<Vector2> rayDirectionQueue;
	Vector2 currentPos = GetPlayerPosition(); // 출발지
	Vector2 dPos = actorPosition; // 목적지
	while (dPos != currentPos)
	{
		Vector2 faceDirction = FacingDirectionGL(currentPos, actorPosition);
		currentPos = currentPos + faceDirction;
		rayDirectionQueue.emplace_back(currentPos);
	} //마지막 액터 위치 안들어가는 게 맞는 로직
	rayDirectionQueue.pop_back();
	return rayDirectionQueue;
}

Vector2 GameLevel::FacingDirectionGL(const Vector2& currentPos, const Vector2& actorPosition)
{
	float innerProduct = static_cast<float>(
		(-currentPos.x + actorPosition.x) * 1
		+ (-currentPos.y + actorPosition.y) * 0
		);
	float rayDistance = static_cast<float>(std::sqrt(
		std::pow(currentPos.x - actorPosition.x, 2)
		+ std::pow(currentPos.y - actorPosition.y, 2)
	));
	float absFace = 1;
	double facingAngle = 0;
	if (rayDistance > 0)
	{ //각도 계산.
		facingAngle = acos(innerProduct / (rayDistance * absFace)) * ANGLE;
	}
	if (currentPos.y - actorPosition.y < 0) // 플레이어보다 액터가 아래.
	{
		if (facingAngle < 23)
		{
			return Vector2(1, 0);
		}
		else if (facingAngle < 68)
		{
			return Vector2(1, 1);
		}
		else if (facingAngle < 113)
		{
			return Vector2(0, 1);
		}
		else if (facingAngle < 158)
		{
			return Vector2(-1, 1);
		}
		else if (facingAngle >= 158)
		{
			return Vector2(-1, 0);
		}
	}
	else
	{
		if (facingAngle < 23)
		{
			return Vector2(1, 0);
		}
		else if (facingAngle < 68)
		{
			return Vector2(1, -1);
		}
		else if (facingAngle < 113)
		{
			return Vector2(0, -1);
		}
		else if (facingAngle < 158)
		{
			return Vector2(-1, -1);
		}
		else if (facingAngle >= 158)
		{
			return Vector2(-1, 0);
		}
	}
	return Vector2(0, 0);
}
// 위 두 함수들을 이용해 플레이어 시야 내에 객체가 존재하는지 판별.

void GameLevel::OnInitialized()
{
	//상위 개체 호출
	Level::OnInitialized();

	//파일을 읽어서 맵 로드
	LoadMap("ACMap.txt");

}

void GameLevel::Draw()
{
	IsSighted();
	//게임 클리어표시
	if (isGameOver
		|| targetClear
		|| clientClear)
	{
	}
	else
	{
		Level::Draw();
	}
}

void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	SetGameStatus();
	Game& game = dynamic_cast<Game&>(Engine::Get());

	// Game에 결과 저장
	game.SetGameStatus(targetClear, clientClear, isGameOver);

	if (isGameOver
		|| targetClear
		|| clientClear)
	{
		game.ToggleMenu();
	}
}

void GameLevel::LoadMap(const std::string& filename)  
{
	//최종 경로 조립
	std::string path = std::string("../Assets/") + filename;

	//파일 열기
	FILE* file = nullptr; 
 	fopen_s(&file, path.c_str(), "rt");
	if (!file)
	{
		assert(false && "failed to open a gameMap file.");
		return;
	}

	// 파일의 내용을 저장할 버퍼(데이터 저장 공간)
	// 파일 길이 확인 -> 파일 위치를 제일 뒤로 이동 시킨 뒤 값 읽기.
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);

	// 파일 제일 끝 위치를  구한 다음에는 다시 처음으로 되돌리기
	//fseek(file, 0, SEEK_END);
	rewind(file);

	// 구한 위치를 사용해서 버퍼 생성
	char* buffer = new char[fileSize] {};

	//데이터 읽기
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);
	assert(readSize > 0 && "No data in the gameMap file.");

	// 읽은 데이터를 기반으로 로직 제작
	// 1. 화면에 액터 그리기
	//문자열에 저장된 값을 접근할 때 사용할 인덱스.
	int index = 0;
	map.clear();
	map.emplace_back();
	clearMap.clear();
	clearMap.emplace_back();
	
	//액터 생성에 사용할 위치값
	Vector2 position = Vector2::Zero;
	while (true)
	{
		//종료 조건 모두 읽었는지 파악
		if (index >= fileSize)
		{
			break;
		}

		//이번에 확인할 문자값
		char mapCharacter = buffer[index];
		//인덱스 증가처리
		++index;

		//현재 문자가 개행 문자라면 로직은 건더뛰고 위치값만 설정
		if (mapCharacter == '\n')
		{
			position.x = 0;
			++position.y;
			map.emplace_back();
			clearMap.emplace_back();
			continue;
		}

		clearMap.emplace_back(0);
		if (mapCharacter == '#')
		{
			map[position.y].emplace_back(1);
		}
		else
		{
			map[position.y].emplace_back(0);
		}

		////읽은 문자별로 처리.
		switch (mapCharacter)
		{
		case '#': //벽
			SpawnActor<Wall>(position);
			break;
		case '.': //땅
			SpawnActor<Ground>(position);
			break;
		case 'g': //경비
			SpawnActor<Guard>(position);
			SpawnActor<Ground>(position);
			break;
		case 'a': //궁수
			SpawnActor<Archer>(position);
			SpawnActor<Ground>(position);
			break;
		case 't': // 타겟
			SpawnActor<Target>(position);
			SpawnActor<Ground>(position);
			break;
		case 'c': // 의뢰인
			SpawnActor<Client>(position);
			SpawnActor<Ground>(position);
			break;
		case 'p': //플레이어
			SpawnActor<Ground>(position); //플레이어가 이동한 후에 바닥
			camera = SpawnActor<Camera>();
			player = SpawnActor<Player>(position);
			break;
		}

		//x위치 업데이트
		++position.x;
	}



	//모두 사용한 버퍼해제
	delete[] buffer;
	buffer = nullptr;

	//파일 닫기
	fclose(file);
	file = nullptr;
	
}

void GameLevel::SetGameStatus()
{
	if (!collisionEnabledActorList.size())
	{
		return;
	}
	//점수
	int leftClient = 0;
	int leftTarget = 0;
	bool playerArlive = false;
	//하고싶은 일 : 박스가 타겟에 위치에 모두 배치되었는지 확인

	//박스 목록/ 타겟 목록 저장
	std::vector<std::shared_ptr<Actor>> clientList;
	std::vector<std::shared_ptr<Actor>> targetList;
	std::vector<std::shared_ptr<Actor>> playerList;

	//게임 레벨의 모든 액터를 순회하면서 박스와 타겟 목록에 저장
	for (const std::shared_ptr<Actor>& actor : collisionEnabledActorList)
	{
		if (actor->IsTypeOf<Client>())
		{ //클라이언트인 경우 목록 추가
			clientList.emplace_back(actor);
			++leftClient;
			continue;
		}

		if (actor->IsTypeOf<Target>())
		{ //타겟인 경우 목록 추가
			targetList.emplace_back(actor);
			++leftTarget;
			continue;
		}

		if (actor->IsTypeOf<Player>())
		{
			playerList.emplace_back(actor);
			playerArlive = true;
		}
	}
	if (!leftClient)
	{
		clientClear = true;
	}
	if (!leftTarget)
	{
		targetClear = true;
	}
	if (!playerArlive)
	{
		isGameOver = true;
	}
}


bool GameLevel::SearchingActorGL(const std::shared_ptr <Actor>& actor)
{
	Vector2 playerPos = GetPlayerPosition();
	Vector2 actorPos = actor->GetPosition();
	float distance = static_cast<float>(std::sqrt(
		std::pow(playerPos.x - actorPos.x, 2)
		+ std::pow(playerPos.y - actorPos.y, 2)
	));
	if (distance < 5)
	{
		return true;
	}
	else if (distance > 20)
	{
		return false;
	}
	Vector2 playerFace = GetPlayerFace();
	float innerProduct = static_cast<float>(
		(- playerPos.x + actorPos.x) * playerFace.x + (- playerPos.y + actorPos.y) * playerFace.y
		);

	float absFace = static_cast<float>(std::sqrt(
		std::pow(playerFace.x, 2)
		+ std::pow(playerFace.y, 2)
	));
	double relativeAngle = 0;
	if (distance * absFace)
	{
		relativeAngle = acos(innerProduct / (distance * absFace)) * ANGLE;
	}
	else
	{
		relativeAngle = 0;
	}
	if (relativeAngle < 50)
	{
		std::vector<Vector2> rayDirectionQueue = RayDirectionQueueInsertGL(actorPos);
		bool isWall = false;
		for (Vector2 path : rayDirectionQueue)
		{
			isWall = IsWall(path);
			if (isWall)
			{
				break;
			}
		}
		if (!isWall)
		{
			{
				return true;
			}
		}
	}
	return false;
}