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
#include <Actor/Item/Item.h>
#include <Actor/Item/Clue.h>
#include <Render/Renderer.h>
#include <Input/Input.h>
#include <Util/Bresenham.h>

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

bool GameLevel::CanAttack(const Craft::Vector2& playerPosition, const Craft::Vector2& dPos)
{
	if (isGameOver
		|| targetClear
		|| clientClear)

	{
		return false;
	}

	//공격하려는 곳이 벽인 경우
	if (map[playerPosition.y + dPos.y][playerPosition.x + dPos.x])
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
		actor->SetIsSighted(
			(GetDebuger()) ?
			true : SearchingActorGL(actor));
		//actor->SetIsSighted(true); //디버깅
		if (!actor->IsTypeOf<Enemy>()
			//&& !actor->IsTypeOf<Ground>()
			&& actor->GetIsSighted()
			&& !actor->GetKeepSighted()
			)
		{
			actor->SetKeepSighted(true);
		}
		IsntSighted(actor);
	}
}

void GameLevel::IsntSighted(const std::shared_ptr<Actor>& actor)
{
	if (actor->GetKeepSighted())
	{
		if (actor->IsTypeOf<Wall>())
		{
			if (actor->GetIsSighted())
			{
				actor->SetColor(Color::Gray);
			}
			else
			{
				actor->SetColor(Color::Black);
			}
		}
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
	if (currentPosition.x >= 0 && currentPosition.y >= 0)
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

void GameLevel::ItemOnCollision(const std::wstring DropKey)
{
	Renderer::Get().ScreenSubmit(
		L"Press " + DropKey + L"!!",
		Vector2(22, 22),
		Color::White,
		2,
		true
	);
}

void GameLevel::DropClue(const std::wstring newclue)
{
	if (clue1.size() == 0)
	{
		clue1 = newclue;
		ClueGet(bGetClue1);
	}
	else if (clue2.size() == 0)
	{
		clue2 = newclue;
		ClueGet(bGetClue2);
	}
	else if (clue3.size() == 0)
	{
		clue3 = newclue;
		ClueGet(bGetClue3);
	}
	else
	{
		return;//일단은 최대 단서 갯수 3개
	}
}

void GameLevel::SubmitClue()
{
	if (bGetClue1)
	{
		Renderer::Get().ScreenSubmit(
			clue1,
			Vector2(3, 26),
			Color::White,
			2,
			true
		);
	}
	if (bGetClue2)
	{
		Renderer::Get().ScreenSubmit(
			clue2,
			Vector2(3, 27),
			Color::White,
			2,
			true
		);
	}
	if (bGetClue3)
	{
		Renderer::Get().ScreenSubmit(
			clue3,
			Vector2(3, 28),
			Color::White,
			2,
			true
		);
	}
}

void GameLevel::OnInitialized()
{
	//상위 개체 호출
	Level::OnInitialized();


	//파일을 읽어서 맵 로드
	LoadMap("map_500x500.txt"); 

}

void GameLevel::Draw()
{
	IsSighted();
	Renderer::Get().ScreenSubmit(
		L"┌────────────────────────────────────────────────┐\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n" 
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"├────────────────────────────────────────────────┤\n"
		L"│                                                │\n"
		L"├────────────────────────────────────────────────┤\n"
		L"│                                                │\n"
		L"├────────────────────────────────────────────────┤\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n" 
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n" 
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"│                                                │\n"
		L"└────────────────────────────────────────────────┘",
		Vector2::Zero,
		Color::Yellow,
		0, 
		true 
	);
	SubmitClue();
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

	Renderer::Get().ScreenSubmit(
		L"Eliminate Target.",
		Vector2(17, 24),
		Color::White,
		2,
		true
	);

	// Game에 결과 저장
	game.SetGameStatus(targetClear, clientClear, isGameOver);
	
	if (Input::Get().GetKeyDown(VK_F2))
	{ 
		debugerTrigger = 1 - debugerTrigger;
		SetDebuger( (debugerTrigger == 1) ? true : false );
	}


	// ESC 종료
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		//QuitGame();
		//메뉴 토글
		Game& game = dynamic_cast<Game&>(Engine::Get());
		game.ToMenu();
		game.changeLevelRender();
		return;
	}

	if (isGameOver
		|| targetClear
		|| clientClear)
	{
		game.ToMenu();
		game.changeLevelRender();
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
	int clueCount = 0;
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
		std::string cluePath;
		std::shared_ptr<Clue> clueActor;
		long clueFileSize = 0;
		FILE* clueFile = nullptr;
		char* clueBuffer = nullptr;
		size_t clueReadSize = 0;
		int clueIndex = 0;
		std::wstring newClue;
		////읽은 문자별로 처리.
		switch (mapCharacter)
		{
		case 'i':
			clueActor = SpawnActor<Clue>(position);
			//최종 경로 조립
			switch (clueCount)
			{
			case 0:
				cluePath = std::string("../Assets/") + "clue_1.txt";
				++clueCount;
				break;
			case 1:
				cluePath = std::string("../Assets/") + "clue_2.txt";
				++clueCount;
				break;
			case 2:
				cluePath = std::string("../Assets/") + "clue_3.txt";
				break;
			}

			//파일 열기
			fopen_s(&clueFile, cluePath.c_str(), "rt");
			if (!clueFile)
			{
				assert(false && "failed to open a Item file.");
				return;
			}

			// 파일의 내용을 저장할 버퍼(데이터 저장 공간)
			// 파일 길이 확인 -> 파일 위치를 제일 뒤로 이동 시킨 뒤 값 읽기.
			fseek(clueFile, 0, SEEK_END);
			clueFileSize = ftell(clueFile);

			// 파일 제일 끝 위치를  구한 다음에는 다시 처음으로 되돌리기
			//fseek(file, 0, SEEK_END);
			rewind(clueFile);

			// 구한 위치를 사용해서 버퍼 생성
			clueBuffer = new char[clueFileSize] {};

			//데이터 읽기
			clueReadSize = fread(clueBuffer, sizeof(char), clueFileSize, clueFile);
			assert(clueReadSize > 0 && "No data in the Item file.");

			clueIndex = 0;
			newClue;
			while (true)
			{
				if (clueIndex >= clueFileSize)
				{
					break;
				}
				newClue.push_back(clueBuffer[clueIndex]);
				++clueIndex;
			}
			clueActor->SetClue(newClue);


			delete[] clueBuffer;
			clueBuffer = nullptr;
			fclose(clueFile);
			clueFile = nullptr;

			break;
		case '#': //벽
			SpawnActor<Wall>(position);
			break;
		case '.': //땅
			//SpawnActor<Ground>(position);
			break;
		case 'g': //경비
			SpawnActor<Guard>(position);
			//SpawnActor<Ground>(position);
			break;
		case 'a': //궁수
			SpawnActor<Archer>(position);
			//SpawnActor<Ground>(position);
			break;
		case 't': // 타겟
			SpawnActor<Target>(position);
			//SpawnActor<Ground>(position);
			break;
		case 'c': // 의뢰인
			SpawnActor<Client>(position);
			//SpawnActor<Ground>(position);
			break;
		case 'p': //플레이어
			//SpawnActor<Ground>(position); //플레이어가 이동한 후에 바닥 
			camera = SpawnActor<Camera>();
			player = SpawnActor<Player>(position);
			break;
		}

		//x위치 업데이트
		++position.x;
		if (clueBuffer != nullptr)
		{
			delete[] clueBuffer;
			clueBuffer = nullptr;
		}
		if (clueFile != nullptr)
		{
			fclose(clueFile);
			clueFile = nullptr;
		}
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
	if (actor->IsTypeOf<Player>())
	{
		if (bDebuger)
		{ //디버그 모드 플레이어 시야 
			int sightPosX = (playerPos.x - 4 > 0) ? playerPos.x - 4 : 0;
			int sightPosY = (playerPos.y - 4 > 0) ? playerPos.y - 4 : 0;
			for (; sightPosX <= playerPos.x + 4; ++sightPosX)
			{
				for (;sightPosY <= playerPos.y + 4; ++sightPosY)
				{
					if (sightPosX == playerPos.x && sightPosY == playerPos.y)
					{
						continue;
					}
					Renderer::Get().Submit(
						L"█",
						Vector2(sightPosX, sightPosY),
						Color::Green,
						-1,
						true
					);
				}
			}
		}
		return true;
	}
	// 거리로 조건문 처리할 때는 제곱 형태로 두고 계산하여 성능 향상 
	float distance = static_cast<float>(
		(playerPos.x - actorPos.x)
		* (playerPos.x - actorPos.x)
		+ (playerPos.y - actorPos.y)
		* (playerPos.y - actorPos.y));
	if (distance < 4 * 4 && 1 <= distance) 
	{
		return true;
	}
	else if (distance > 30 * 30)
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
	distance = std::sqrt(distance);// 진짜 거리가 필요할 때 계산해서 적용.
	if (distance * absFace)
	{
		
		if (innerProduct / (distance * absFace) > 1)
		{
			innerProduct = distance * absFace;
		}
		if (innerProduct / (distance * absFace) < -1)
		{
			innerProduct = -1 * distance * absFace;
		}
	 	relativeAngle = acos(innerProduct / (distance * absFace)) * ANGLE;
	}
	else
	{
		relativeAngle = 0;
	}
		Bresenham bresenham(map);
	if (relativeAngle < 40 && 1 <= distance
		&& -40 < relativeAngle)
	{
		//std::vector<Vector2> rayDirectionQueue = RayDirectionQueueInsertGL(actorPos);
		std::vector<Vector2> rayDirectionQueue = bresenham.BresenhamFinder(distance, playerPos, actorPos);
		if (rayDirectionQueue.size())
		{
			rayDirectionQueue.pop_back(); // 경로가 해당 액터까지의 경로 저장이므로 마지막 경로 제외
		}
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