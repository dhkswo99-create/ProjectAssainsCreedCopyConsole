#include "Heap.h"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>

using namespace Craft;

void SwapNode(int leftIndex, int rightIndex, std::vector<Node*>& nodes)
{
	Node* temNode = nodes[leftIndex];
	nodes[leftIndex] = nodes[rightIndex];
	nodes[rightIndex] = temNode;
}

void InsertHeap(std::vector<Node*>& nodes, Node* newNode)
{
	if (!newNode)
	{
		return;
	}
	nodes.emplace_back(newNode);
	int selected = static_cast<int>(nodes.size()) - 1;
	int upper = (selected - 1) / 2;
	while (true)
	{
		if (nodes[selected]->fCost < nodes[upper]->fCost)
		{
			SwapNode(selected, upper, nodes);
			selected = upper;
			upper = (selected - 1) / 2;
		}
		else
		{
			break;
		}
		if (selected == 0)
		{
			break;
		}
	}
}

void Heapify(std::vector<Node*>& nodes, int index,
	std::function<bool(float, float)> comparer)
{
	while (true)
	{
		int selected = index;
		int leftChild = index * 2 + 1;
		int rightChild = index * 2 + 2;

		if (leftChild < nodes.size()
			&& comparer(nodes[leftChild]->fCost, nodes[selected]->fCost))
		{
			selected = leftChild;
		}
		if (rightChild < nodes.size()
			&& comparer(nodes[rightChild]->fCost, nodes[selected]->fCost))
		{
			selected = rightChild;
		}

		//현재 노드가 컸다면 힙이 유지 -> 종료
		if (selected == index)
		{
			break;
		}

		//값 교환
		SwapNode(index, selected, nodes);

		// 값 교환 위치 자손 노드들과 비교
		index = selected;
	}
}
Node* popHeap(std::vector<Node*>& nodes)
{
	assert(!nodes.empty());
	if (nodes.empty())
	{
		return {};
	}
	Node* popNode = nodes[0];
	if (nodes.size() == 1)
	{
		nodes.pop_back();
		return popNode;
	}
	SwapNode(0, static_cast<int>(nodes.size()) - 1, nodes);
	nodes.pop_back();
	Heapify(nodes, 0);
	return popNode;
}
