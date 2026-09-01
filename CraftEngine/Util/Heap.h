#pragma once

#include <Math/Vector2.h>
#include <functional>

struct Node
{
public:
	Node(const Craft::Vector2& position, Node* parent = nullptr)
		: position(position), parent(parent)
	{
	}
	Craft::Vector2 position = Craft::Vector2(0, 0);
	float fCost;
	float gCost;
	float hCost;
	Node* parent = nullptr;
};

void SwapNode(int leftIndex, int rightIndex, std::vector<Node*>& nodes);

void InsertHeap(std::vector<Node*>& nodes, Node& newNode);

Node* popHeap(std::vector<Node*>& nodes);

void Heapify(std::vector<Node*>& nodes, int index,
	std::function<bool(float, float)> comparer = std::less());


















//void HeapSort(Node* array, int length,
//	std::function<bool(int, int)> comparer = std::greater())
//{
//	const int lastParentIndex = length / 2 - 1;
//	for (int ix = lastParentIndex; ix >= 0; --ix)
//	{
//		Heapify(array, length, ix);
//	}
//
//	for (int ix = length - 1; ix > 0; --ix)
//	{
//		std::swap(array[0], array[ix]);
//
//		Heapify(array, length, ix);
//	}
//
//}