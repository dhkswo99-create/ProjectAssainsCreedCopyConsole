#pragma once

#include <Level/Level.h>
#include <Actor/Player.h>
#include <Actor/Item/Clue.h>


class Camera;
//게임 클리어 등 게임 규칙 및 전반을 관리
class GameLevel :public Craft::Level
{
public:
	struct sight
	{
		char image = ' ';
		Color data = Color::Black;
		bool isSight = false;
		bool keepSight = false;
	};

public:
	//커스텀 타입 설정
	TYPE_DECLARATIONS(GameLevel, Level)

		//플레이어 이동하려는 위치가 이동 가능한지 판단해주는 함수.
	bool CanMove(
		const Craft::Vector2& nextPosition
	);
	bool CanAttack(
		const Craft::Vector2& playerPosition,
		const Craft::Vector2& dPos
	);
	void IsTileSighted();
	void IsActorSighted();
	void IsBossInfo();
	bool InAngle(const float angle, const float sightAngle, const float resultAngle);
	void SetVisibleCircleVector();
	void IsntSighted(const std::shared_ptr<Craft::Actor>& actor);

	bool IsWall(const Craft::Vector2& currentPosition);
	bool GetDebuger() { return bDebuger; }
	int GetTargetClueCount() { return targetClueCount; }
	int GetClientClueCount() { return clientClueCount; }
	
	std::vector<Craft::Vector2> RayDirectionQueueInsertGL(const Craft::Vector2& actorPosition);
	Craft::Vector2 FacingDirectionGL(const Craft::Vector2& currentPos, const Craft::Vector2& actorPosition);
	bool SearchingActorGL(const std::shared_ptr<Craft::Actor>& actor);

	Craft::Vector2 GetPlayerPosition() const { return player->GetPosition(); }
	Craft::Vector2 GetPlayerFace() const { return player->GetFace(); }
	std::vector<std::vector<int>> GetMap() { return map; }
	std::vector<std::vector<int>> GetClearMap() { return clearMap; }

	//아이템 충돌 이벤트 Press F 출력
	void ItemOnCollision(const std::wstring DropKey);
	//단서를 얻었을 때 이벤트
	void DropTargetClue(const std::wstring newclue);
	void DropClientClue(const std::wstring newclue);
	void SetTileColor(const char color, sight& sightMap);

	void ClueGet(bool& getClue)
	{
		getClue = true;
	}
	void SubmitClue();

private:
	void SpawnActors();
	void SpawnClue(const std::string kindOfClue, const Vector2& position);
	void CalcSight();
	void SetDebuger(const bool debuger) { bDebuger = debuger; }
	void SetCleanViewer(const bool CleanSight) { bCleanSight = CleanSight; }
	//레벨 초기화 함수
	virtual void OnInitialized() override;

	//Draw 이벤트 함수
	virtual void Draw() override;

	//Tick 이벤트 함수
	virtual void Tick(float deltaTime) override;

	//맵 로드 함수
	void LoadMap(const std::string& filename);



private:
	//게임상태설정함수
	void SetGameStatus();

private:
	// 원형 상대 좌표 변수
	std::vector<Vector2> visibleCirclePointsRight;
	std::vector<Vector2> visibleCirclePointsRightUp;
	std::vector<Vector2> visibleCirclePointsUp;
	std::vector<Vector2> visibleCirclePointsLeftUp;
	std::vector<Vector2> visibleCirclePointsLeft;
	std::vector<Vector2> visibleCirclePointsLeftDown;
	std::vector<Vector2> visibleCirclePointsDown;
	std::vector<Vector2> visibleCirclePointsRightDown;

	// 단서 변수
	std::wstring targetClue1;
	std::wstring targetClue2;
	std::wstring targetClue3;
	std::wstring clientClue1;
	std::wstring clientClue2;
	std::wstring clientClue3;
	bool bGetTargetClue1 = false;
	bool bGetTargetClue2 = false;
	bool bGetTargetClue3 = false;
	bool bGetClientClue1 = false;
	bool bGetClientClue2 = false;
	bool bGetClientClue3 = false;
	int targetClueCount = 0;
	int clientClueCount = 0;



	//토글처리
	int debugerTrigger = 0;
	int cleanSightTrigger = 0;
	bool bDebuger = false;
	bool bCleanSight = false;


	bool targetClear = false;
	bool clientClear = false;
	//게임오버 
	bool isGameOver = false;


	std::vector<std::vector<sight>> sightMap;
	// 공간을 좀 쓰긴 하는데 계속 쓸 예정
	std::vector<std::vector<int>> map;
	std::vector<std::vector<int>> clearMap;
	std::shared_ptr<Camera> camera;
	std::shared_ptr<Player> player;
};


