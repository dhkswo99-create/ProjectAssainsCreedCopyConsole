#pragma once

#include <Actor/ACTOR.H>

using namespace Craft;
class Item : public Actor
{
	TYPE_DECLARATIONS(Item, Actor)

public:
	Item(const std::wstring& image,
		const Vector2& position,
		Color color);
	virtual ~Item() = default;

	virtual void OnCollision(const std::shared_ptr<Actor>& other) override;

private:


private:
	std::wstring dropKey;
};

