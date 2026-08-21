#include "Renderer.h"
#include "ScreenBuffer.h"

#include <cassert>
#include <iostream>
#include <Windows.h>


namespace Craft
{
	//--------------------Frame-------------//
	Renderer::Frame::Frame(int bufferCount)
	{
		charInfoArray = std::make_unique<CHAR_INFO[]>(bufferCount);
		sortingOrderArray = std::make_unique<int[]>(bufferCount);
	}
	Renderer::Frame::~Frame() {}

	void Renderer::Frame::Clear(const Vector2& screenSize) //렌더러가 가지고 있음.
	{
		const int width = screenSize.x;
		const int height = screenSize.y;
		
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				//1차원 배열을 2차원 배열로 사용할 때 필요한 인덱스 좌표 변환
				const int index = (y * width) + x;
				
				//글자 항목 초기화
				CHAR_INFO& info = charInfoArray[index];
				info.Char.AsciiChar = ' '; //빈문자 설정 - > 기존 설정 값 지우기
				info.Attributes = 0; // 색상 표기 X

				// 그리기 순서 배열 항목 초기화.
				sortingOrderArray[index] = -1;

			}
		}
	
	}
	//정적 변수 초기화
	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2& screenSize)
		: screenSize(screenSize)
	{
		assert(!instance && "instance should be null");
		instance = this;

		//프레임 객체 생성.
		const int bufferCount = screenSize.x * screenSize.y;
		frame = std::make_unique<Frame>(bufferCount);

		//생성 후 프레임 지우기.
		frame->Clear(screenSize);

		//이중 버퍼 구현을 위한 콘솔 버퍼 생성 및 초기화
		screenBufferArray[0] = std::make_unique<ScreenBuffer>(screenSize);
		screenBufferArray[0]->Clear();
		
		screenBufferArray[1] = std::make_unique<ScreenBuffer>(screenSize);
		screenBufferArray[1]->Clear();

		SetConsoleActiveScreenBuffer(screenBufferArray[0]->GetBuffer());

	}

	Renderer::~Renderer()
	{
		instance = nullptr;

		//콘솔 창 복구
		SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));

	}

	
	void Renderer::Submit(
		const std::wstring& image,
		const Vector2& position,
		Color color,
		int sortingOrder,
		bool isSighted,
		bool keepSighted
	)
	{
		//viewPosition -> 플레이어 중심으로 렌더링
		RenderCommand command;
		command.image = image;
		command.position =
			position
			- cameraView;
		command.color = color;
		command.sortingOrder = sortingOrder;
		command.isSighted = isSighted;
		command.keepSighted = keepSighted;
		renderQueue.emplace_back(command);
	}

	void Renderer::Draw()
	{
		//3단계로 나뉨
		//화면 지우기
		Clear();
		
		//그리기
		DrawRenderQueue();

		// 화면 표시
		Present();
	}

	Renderer& Renderer::Get()
	{
		assert(instance && "instance should not be null");
		return *instance;
	}

	void Renderer::Clear()
	{
		//프레임 값 초기화
		frame->Clear(screenSize);

		//콘솔 버퍼 초기화
		GetCurrentBuffer()->Clear();
	}

	void Renderer::DrawRenderQueue()
	{
		//렌더 큐 순회하면서 그리기 명령 실행
		for (const RenderCommand& command : renderQueue)
		{
			//그릴 문자값이 없으면 건너뛰기 // 너비는 width 높이는 height로 설정 
			if (command.image.empty())
			{
				continue;
			}
			if (!command.keepSighted && command.isSighted == false)
			{
				continue;
			}
			int width = 0;
			int height = 1;

			for ( wchar_t cha : command.image )
			{
				if (cha == '\0') break;
				++width;
				if (cha == '\n')
				{
					++height;
					width = 0;
				}
			}

			//y위치가 화면을 벗어났으면 건너뛰기
			if (command.position.y < 0 
				|| command.position.y >= screenSize.y)
			{
				continue;
			}

			//그리려는 문자열 값
			//const int length = static_cast<int>(command.image.length()); todo 필요한지 확인

			//글자의 시작 위치
			const int startX = command.position.x;
			const int startY = command.position.y;


			//글자의 끝 위치
			const int endX = startX + width - 1;
			const int endY = startY + height - 1;

			// 위치가 화면을 벗어났는지 확인
			if (endX < 0 || startX >= screenSize.x
				|| endY < 0 || startY >= screenSize.y)
			{
				continue;
			}

			//실제 그릴 글자의 위치 구하기 화면 위치
			const int visibleStartX = startX < 0 ? 0 : startX;
			const int visibleEndX = endX >= screenSize.x ? screenSize.x - 1 : endX;
			const int visibleStartY = startY < 0 ? 0 : startY;
			const int visibleEndY = endY >= screenSize.y ? screenSize.y - 1 : endY;

			//루프 순회하면서 글자를 2차원 배열에 하나씩 기록
			for (int x = visibleStartX; x < startX +
				static_cast<int>(command.image.length()); ++x)
			{
				// 문자열에서 글자값을 가져올 때 사용할 인덱스
				const int sourceIndex = x - startX;
				if (command.image[sourceIndex] == '\n') continue;

				//글자 2차원 배열 인덱스 //찍히는 위치. 
				int realX = sourceIndex % width;
				int realY = sourceIndex / width;


				const int index = ( ( command.position.y + realY ) * screenSize.x) 
					+ command.position.x + realX;

				//정렬 우선 순위를 비교해서 그릴지 말지 판정
				//지금 설정은 덮어쓰기 동일 우선 순위시
				if (frame->sortingOrderArray[index] > command.sortingOrder)
				{
					continue;
				}

				//2차원 배열에 글자, 속성 설정 
				frame->charInfoArray[index].Char.UnicodeChar
					= command.image[sourceIndex];
				
				//글자 색상 값 설정s
				frame->charInfoArray[index].Attributes
					= static_cast<DWORD>(command.color);

				//그리기 우선 순위 값 설정
				frame->sortingOrderArray[index] = command.sortingOrder;
			}
		}
		// 앞에서 설정한 2차원 배열을 콘솔에 그리기.
		GetCurrentBuffer()->Draw(frame->charInfoArray.get());


		//렌더 큐 비우기 //그리기가 끝났으니
		renderQueue.clear();

		//콘솔색상초기화
		SetConsoleTextAttribute(
			GetCurrentBuffer()->GetBuffer(),
			static_cast<DWORD>(Color::White)
		);
	}

	void Renderer::Present()
	{
		//현재 순번의 콘솔 버퍼를 활성화 처리
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());

		//인덱스 업데이트
		currentBufferIndex = 1 - currentBufferIndex; //One Minus

	}
	const ScreenBuffer* const Renderer::GetCurrentBuffer() const
	{
		return screenBufferArray[currentBufferIndex].get();
	}
}