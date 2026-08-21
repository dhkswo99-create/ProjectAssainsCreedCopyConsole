#include "Astar.h"
#include <cassert>
#include <cmath>
#include <set>
#include <stack>

using cCoord = std::pair<double, std::pair<int, int>>;

Astar::Astar(
    std::vector<std::vector<int>> map,
    const Vector2& startPosition,
    const Vector2& destination)
    : map(map),
    startPosition(startPosition),
    destination(destination)
{
    if (this->map.empty())
    {
        return;
    }

    ROW = this->map.size();
    COL = this->map[0].size();
}

std::vector<Vector2> Astar::AstarFinder(
    std::vector<std::vector<int>>& map,
    const Vector2& startPosition,
    const Vector2& destination)
{
    std::vector<Vector2> nullVector;

    // 범위 검사 먼저
    if (!IsInRange(startPosition.x, startPosition.y)
        || !IsInRange(destination.x, destination.y))
    {
        return nullVector;
    }

    // 시작 위치는 이동 가능하게 처리
    map[startPosition.y][startPosition.x] = 0;

    if (!IsUnblocked(map, startPosition.x, startPosition.y)
        || !IsUnblocked(map, destination.x, destination.y))
    {
        assert(true && "Unblock?");
        return nullVector;
    }

    if (IsDestination(
        startPosition.x,
        startPosition.y,
        destination))
    {
        assert(true && "Destination?");
        return nullVector;
    }

    // [y][x]
    std::vector<std::vector<bool>> closedList(
        ROW,
        std::vector<bool>(COL, false)
    );

    Node baseNode;

    baseNode.parentx = -1;
    baseNode.parenty = -1;
    baseNode.f = INF;
    baseNode.g = INF;
    baseNode.h = INF;

    // [y][x]
    std::vector<std::vector<Node>> node(
        ROW,
        std::vector<Node>(COL, baseNode)
    );

    // 시작 노드
    node[startPosition.y][startPosition.x].f = 0.0;
    node[startPosition.y][startPosition.x].g = 0.0;
    node[startPosition.y][startPosition.x].h = 0.0;

    node[startPosition.y][startPosition.x].parentx =
        startPosition.x;

    node[startPosition.y][startPosition.x].parenty =
        startPosition.y;

    std::set<cCoord> openList;

    openList.insert(
        {
            0.0,
            {
                startPosition.x,
                startPosition.y
            }
        }
    );

    while (!openList.empty())
    {
        cCoord cc = *openList.begin();
        openList.erase(openList.begin());

        int x = cc.second.first;
        int y = cc.second.second;

        // [y][x]
        closedList[y][x] = true;

        double nf;
        double ng;
        double nh;

        // 상하좌우
        for (int ix = 0; ix < 4; ++ix)
        {
            int nx = x + dx1[ix];
            int ny = y + dy1[ix];

            if (IsInRange(nx, ny))
            {
                if (IsDestination(nx, ny, destination))
                {
                    // [ny][nx]
                    node[ny][nx].parentx = x;
                    node[ny][nx].parenty = y;

                    return TracePath(node, destination);
                }
                else if (
                    !closedList[ny][nx]
                    && IsUnblocked(map, nx, ny))
                {
                    // 현재 노드도 [y][x]
                    ng = node[y][x].g + 1.0;
                    nh = CalcHValue(nx, ny, destination);
                    nf = ng + nh;

                    // 다음 노드 [ny][nx]
                    if (node[ny][nx].f == INF
                        || node[ny][nx].f > nf)
                    {
                        node[ny][nx].f = nf;
                        node[ny][nx].g = ng;
                        node[ny][nx].h = nh;

                        node[ny][nx].parentx = x;
                        node[ny][nx].parenty = y;

                        openList.insert(
                            {
                                nf,
                                { nx, ny }
                            }
                        );
                    }
                }
            }
        }

        // 대각선
        for (int ix = 0; ix < 4; ++ix)
        {
            int nx = x + dx2[ix];
            int ny = y + dy2[ix];

            if (IsInRange(nx, ny))
            {
                if (IsDestination(nx, ny, destination))
                {
                    node[ny][nx].parentx = x;
                    node[ny][nx].parenty = y;

                    return TracePath(node, destination);
                }
                else if (
                    !closedList[ny][nx]
                    && IsUnblocked(map, nx, ny))
                {
                    ng = node[y][x].g + 1.414;
                    nh = CalcHValue(nx, ny, destination);
                    nf = ng + nh;

                    if (node[ny][nx].f == INF
                        || node[ny][nx].f > nf)
                    {
                        node[ny][nx].f = nf;
                        node[ny][nx].g = ng;
                        node[ny][nx].h = nh;

                        node[ny][nx].parentx = x;
                        node[ny][nx].parenty = y;

                        openList.insert(
                            {
                                nf,
                                { nx, ny }
                            }
                        );
                    }
                }
            }
        }
    }

    return nullVector;
}

// 목적지 확인
bool Astar::IsDestination(
    int x,
    int y,
    const Vector2& destination)
{
    return (
        x == destination.x
        && y == destination.y
        );
}

// 유효한 좌표인지
bool Astar::IsInRange(int x, int y)
{
    return (
        x >= 0
        && x < COL
        && y >= 0
        && y < ROW
        );
}

// 벽이 있는지
bool Astar::IsUnblocked(
    std::vector<std::vector<int>>& map,
    int x,
    int y)
{
    if (y < 0 || y >= static_cast<int>(map.size()))
    {
        return false;
    }

    if (x < 0 || x >= static_cast<int>(map[y].size()))
    {
        return false;
    }

    // map[y][x]
    return map[y][x] == 0;
}

double Astar::CalcHValue(
    int x,
    int y,
    const Vector2& destination)
{
    return static_cast<double>(
        std::sqrt(
            std::pow(x - destination.x, 2)
            + std::pow(y - destination.y, 2)
        )
        );
}

std::vector<Vector2> Astar::TracePath(
    const std::vector<std::vector<Node>>& node,
    const Vector2& destination)
{
    std::vector<Vector2> moveDirection;

    int x = destination.x;
    int y = destination.y;

    // node[y][x]
    while (!(node[y][x].parentx == x
        && node[y][x].parenty == y))
    {
        int tempX = node[y][x].parentx;
        int tempY = node[y][x].parenty;

        moveDirection.emplace_back(
            x - node[y][x].parentx,
            y - node[y][x].parenty
        );

        x = tempX;
        y = tempY;
    }

    return moveDirection;
}