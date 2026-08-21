#pragma once

#include <Level/Level.h>
#include <Actor/Player.h>


class Camera;
//소코반 게임 레벨 클래스
//게임 클리어 등 게임 규칙 및 전반을 관리
class GameLevel :public Craft::Level
{

public:
	//커스텀 타입 설정
	TYPE_DECLARATIONS(GameLevel, Level)

		//플레이어 이동하려는 위치가 이동 가능한지 판단해주는 함수.
	bool CanMove(
		const Craft::Vector2& nextPosition
	);
	bool CanAttack(
		const Craft::Vector2& playerPosition,
		const Craft::Vector2& face
	);

	void IsSighted();
	void IsntSighted(const std::shared_ptr<Craft::Actor>& actor);

	bool IsWall(const Craft::Vector2& currentPosition);
	
	std::vector<Craft::Vector2> RayDirectionQueueInsertGL(const Craft::Vector2& actorPosition);
	Craft::Vector2 FacingDirectionGL(const Craft::Vector2& currentPos, const Craft::Vector2& actorPosition);
	bool SearchingActorGL(const std::shared_ptr<Craft::Actor>& actor);

	Craft::Vector2 GetPlayerPosition() const { return player->GetPosition(); }
	Craft::Vector2 GetPlayerFace() const { return player->GetFace(); }
	 
	std::vector<std::vector<int>> GetMap() { return map; }
	std::vector<std::vector<int>> GetClearMap() { return clearMap; }

private:
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
	bool targetClear = false;
	bool clientClear = false;
	//게임오버 
	bool isGameOver = false;


	std::vector<std::vector<int>> map;
	std::vector<std::vector<int>> clearMap;
	std::shared_ptr<Camera> camera;
	std::shared_ptr<Player> player;
};


