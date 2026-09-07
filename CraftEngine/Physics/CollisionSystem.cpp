#include "CollisionSystem.h"
#include "Actor/Actor.h"

namespace Craft
{
	void CollisionSystem::ProcessCollision(
		const std::shared_ptr<Actor>& player,
		const std::vector<std::shared_ptr<Actor>>& collisionEnabledActorList)
	{
		// 예외처리.
		if (collisionEnabledActorList.empty())
		{
			return;
		}
		if (!player || !player->IsActive())
		{
			return;
		}

		// 충돌한 액터에 이벤트를 한번에 정리해 전달하기 위한 배열.
		std::vector<CollisionPair> collidedActorList;

		// 레벨에 배치된 액터 수.
		const int count = static_cast<int>(collisionEnabledActorList.size());

		Vector2 playerPos = player->GetPosition();
		// 모든 액터를 순회하면서 충돌 검사.
		for (int ix = 0; ix < count; ++ix)
		{
			const std::shared_ptr<Actor>& left = collisionEnabledActorList[ix];
			if (!left || !left->IsActive())
			{
				continue;
			}
			Vector2 leftPos = left->GetPosition();
			// 범위 내 액터만 처리
			if ((playerPos.x - leftPos.x)
				* (playerPos.x - leftPos.x)
				+ (playerPos.y - leftPos.y)
				* (playerPos.y - leftPos.y)
				> 100
				)
			{
				continue;
			}

			for (int jx = ix + 1; jx < count; ++jx)
			{
				
				const std::shared_ptr<Actor>& right = collisionEnabledActorList[jx];
				if (!right || !right->IsActive())
				{
					continue;
				}
				if (left == right)
				{
					continue;
				}


				Vector2 rightPos = right->GetPosition();
				// 범위 내 액터만 처리
				if ((playerPos.x - rightPos.x)
					* (playerPos.x - rightPos.x)
					+ (playerPos.y - rightPos.y)
					* (playerPos.y - rightPos.y)
					> 100
					)
				{
					continue;
				}
				// 충돌 검사.
				if (Test(left, right))
				{
					// 이벤트 발행할 목록에 추가할 데이터 생성.
					CollisionPair pair = {};
					pair.actor = left;
					pair.other = right;

					// 목록에 추가.
					collidedActorList.emplace_back(pair);
				}
			}
		}

		// 충돌 발생한 액터 목록 확인. 충돌한 액터가 없으면 함수 종료.
		if (collidedActorList.empty())
		{
			return;
		}

		// 충돌한 액터에 이벤트 전달.
		for (const CollisionPair& pair : collidedActorList)
		{
			// 이미 삭제되거나 비활성화된 액터는 제외.
			if (!pair.actor->IsActive() || !pair.other->IsActive())
			{
				continue;
			}

			// 충돌 이벤트 전달.
			pair.actor->OnCollision(pair.other);
			pair.other->OnCollision(pair.actor);
		}
	}

	void CollisionSystem::ProcessCollisionPlayer(
		const std::shared_ptr<Actor>& player,
		const std::vector<std::shared_ptr<Actor>>& collisionEnabledActorList)
	{
		// 예외처리.
		if (!player && collisionEnabledActorList.empty())
		{
			return;
		}

		// 충돌한 액터에 이벤트를 한번에 정리해 전달하기 위한 배열.
		std::vector<CollisionPair> collidedActorList;

		// 레벨에 배치된 액터 수.
		const int count = static_cast<int>(collisionEnabledActorList.size());

		const std::shared_ptr<Actor>& left = player;
		// 모든 액터를 순회하면서 충돌 검사.
		for (int ix = 0; ix < count; ++ix)
		{
			const std::shared_ptr<Actor>& right = collisionEnabledActorList[ix];
			if (!right || !right->IsActive())
			{
				continue;
			}
			if (left == right)
			{
				continue;
			}

			// 충돌 검사.
			if (TestLarge(left, right))
			{
				// 이벤트 발행할 목록에 추가할 데이터 생성.
				CollisionPair pair = {};
				pair.actor = left;
				pair.other = right;

				// 목록에 추가.
				collidedActorList.emplace_back(pair);
			}
		}

		// 충돌 발생한 액터 목록 확인. 충돌한 액터가 없으면 함수 종료.
		if (collidedActorList.empty())
		{
			return;
		}

		// 충돌한 액터에 이벤트 전달.
		for (const CollisionPair& pair : collidedActorList)
		{
			// 이미 삭제되거나 비활성화된 액터는 제외.
			if (!pair.actor->IsActive() || !pair.other->IsActive())
			{
				continue;
			}

			// 충돌 이벤트 전달.
			pair.actor->OnCollision(pair.other);
		}
	}
	
	bool CollisionSystem::Test(
		const std::shared_ptr<Actor>& left,
		const std::shared_ptr<Actor>& right)
	{
		if (!left || !right)
		{
			return false;
		}

		// AABB (Axis Aligned Bounding Box).

		// left 액터의 현재/이전 위치.
		const Vector2 leftCurrent = left->GetPosition();
		const Vector2 leftPrevious = left->GetPreviousPosition();

		// right 액터의 현재/이전 위치.
		const Vector2 rightCurrent = right->GetPosition();
		const Vector2 rightPrevious = right->GetPreviousPosition();

		//충돌 발생
		if (leftCurrent == rightCurrent || rightPrevious == leftCurrent
			|| leftPrevious == rightCurrent || rightPrevious == leftPrevious)
		{
			return true;
		}
		//충돌 미발생
		return false;
	}
	// 플레이어 넓은 범위 충돌 판정
	bool CollisionSystem::TestLarge(const std::shared_ptr<Actor>& left, const std::shared_ptr<Actor>& right)
	{
		if (!left || !right)
		{
			return false;
		}
		

		// left 액터의 현재/이전 위치.
		const Vector2 leftCurrent = left->GetPosition();
		const Vector2 leftPrevious =  left->GetPreviousPosition();
		// right 액터의 현재/이전 위치.
		const Vector2 rightCurrent = right->GetPosition();
		const Vector2 rightPrevious = right->GetPreviousPosition();

		//충돌 발생
		if (((leftCurrent.x - rightCurrent.x) * (leftCurrent.x - rightCurrent.x)
				+ (leftCurrent.y - rightCurrent.y) * (leftCurrent.y - rightCurrent.y) <= 17)
			|| ((leftCurrent.x - rightPrevious.x) * (leftCurrent.x - rightPrevious.x)
				+ (leftCurrent.y - rightPrevious.y) * (leftCurrent.y - rightPrevious.y) <= 17)

			|| ((leftPrevious.x - rightCurrent.x) * (leftPrevious.x - rightCurrent.x)
				+ (leftPrevious.y - rightCurrent.y) * (leftPrevious.y - rightCurrent.y) <= 17)
			|| ((leftPrevious.x - rightPrevious.x) * (leftPrevious.x - rightPrevious.x)
				+ (leftPrevious.y - rightPrevious.y) * (leftPrevious.y - rightPrevious.y) <= 17)
			)
		{
			return true;
		}
		
		//충돌 미발생
		return false;
	}
}