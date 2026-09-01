#include "CollisionSystem.h"
#include "Actor/Actor.h"

namespace Craft
{
	void CollisionSystem::ProcessCollision(
		const std::vector<std::shared_ptr<Actor>>& collisionEnabledActorList)
	{
		// 예외처리.
		if (collisionEnabledActorList.empty())
		{
			return;
		}

		// 충돌한 액터에 이벤트를 한번에 정리해 전달하기 위한 배열.
		std::vector<CollisionPair> collidedActorList;

		// 레벨에 배치된 액터 수.
		const int count = static_cast<int>(collisionEnabledActorList.size());

		// 모든 액터를 순회하면서 충돌 검사.
		for (int ix = 0; ix < count; ++ix)
		{
			const std::shared_ptr<Actor>& left = collisionEnabledActorList[ix];
			if (!left || !left->IsActive())
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

	bool CollisionSystem::Test(
		const std::shared_ptr<Actor>& left,
		const std::shared_ptr<Actor>& right)
	{
		if (!left || !right)
		{
			return false;
		}

		// AABB (Axis Aligned Bounding Box).
		// y는 크기가 1이기 때문에 x좌표만 고려.

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
}