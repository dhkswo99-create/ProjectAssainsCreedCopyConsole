#include "Level.h"

namespace Craft
{
	Level::Level()
	{

	}
	Level::~Level()
	{
	}
	void Level::OnInitialized()
	{
		//초기화 됐다고 설정
		hasInitialized = true;
	}
	void Level::BeginPlay()
	{
		//액터 초기화 시 1번 호출되는 이벤트.
		for (std::shared_ptr<Actor>& actor : actorList)
		{
			//검증 이미 BeginPlay가 처리된 경우 건너뛰기
			if (actor->HasBeganPlay())
			{
				continue;
			}
			actor->BeginPlay();
		}
	}
	void Level::Tick(float deltaTime)
	{
		for (std::shared_ptr<Actor>& actor : useTickActorList)
		{
			//검증 활성화 되지 않았으면 건너뛰기
			if (!actor->IsActive())
			{
				continue;
			}
			actor->Tick(deltaTime);
		}
	}
	void Level::Draw()
	{
		for (std::shared_ptr<Actor>& actor : useTickActorList)
		{
			//검증 활성화 되지 않았으면 건너뛰기
			if (!actor->IsActive())
			{
				continue;
			}
			actor->Draw();
		}
	}
	void Level::ProcessAddAndDestroyActors()
	{
		//액터 제거 처리
		//이터레이터 기반 루프
		for (auto iterator = actorList.begin(); iterator != actorList.end();)
		{
			//제거요청된 액터인지 확인
			auto actor = *iterator; // auto == std::shared_ptr<Actor>
			if (actor->HasExpired())
			{
				iterator = actorList.erase(iterator); //지워졌을 때 해당 칸을 가리키는 이터레이터 처리
				//위 코드가 어려운 코드. 처리되면 컨트롤 블럭에서 참조횟수 차감
				//-> 0이 되면 sharedptr이 메모리 해제
				continue;
			}
			//다음 순번을 처리하기 위해 이터레이터(반복자, 포인터) 증가 처리.
			++iterator;
		}
		for (auto iterator = collisionEnabledActorList.begin(); iterator != collisionEnabledActorList.end();)
		{
			//제거요청된 액터인지 확인
			auto actor = *iterator; // auto == std::shared_ptr<Actor>
			if (actor->HasExpired())
			{
				iterator = collisionEnabledActorList.erase(iterator); //지워졌을 때 해당 칸을 가리키는 이터레이터 처리
				//위 코드가 어려운 코드. 처리되면 컨트롤 블럭에서 참조횟수 차감
				//-> 0이 되면 sharedptr이 메모리 해제
				continue;
			}
			//다음 순번을 처리하기 위해 이터레이터(반복자, 포인터) 증가 처리.
			++iterator;
		}
		for (auto iterator = useTickActorList.begin(); iterator != useTickActorList.end();)
		{
			//제거요청된 액터인지 확인
			auto actor = *iterator; // auto == std::shared_ptr<Actor>
			if (actor->HasExpired())
			{
				iterator = useTickActorList.erase(iterator); //지워졌을 때 해당 칸을 가리키는 이터레이터 처리
				//위 코드가 어려운 코드. 처리되면 컨트롤 블럭에서 참조횟수 차감
				//-> 0이 되면 sharedptr이 메모리 해제
				continue;
			}
			//다음 순번을 처리하기 위해 이터레이터(반복자, 포인터) 증가 처리.
			++iterator;
		}
		//추가 처리
		//추가요청된 목록이 없으면 종료
		if (addRequestedActorList.empty())
		{
			return;
		}
		for (const auto& actor : addRequestedActorList)
		{
			actorList.emplace_back(actor);
			if (actor->GetCollisionEnabled())
			{
				collisionEnabledActorList.emplace_back(actor);
			}
			if (actor->GetUseTick())
			{
				useTickActorList.emplace_back(actor);
			}
		}
		//추가 처리된 목록 정리
		addRequestedActorList.clear();
	}
	void Level::SavePreviousActorStates()
	{
		//액터 순회하면서 이전 상태 저장 처리
		for (auto actor : actorList)
		{
			if (!actor->IsActive())
			{
				continue;
			}

			//상태 저장
			actor->SavePreviousState();
		}
	}
}




