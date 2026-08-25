#pragma once

#include <Core/Core.h>
#include <Math/Color.h>
#include <Math/Vector2.h>
#include <string>
#include <vector>
#include <memory>


namespace Craft
{
	//전방선언
	class ScreenBuffer;

	// 그리기 기능을 전담하는 전문 객체.  //렌더러가 여러 개가 될 수도 있음.
	class CRAFT_API Renderer
	{
		//프레임 데이터 구조체 
		struct Frame
		{
			Frame(int bufferCount);
			~Frame();

			void Clear(const Vector2& fullScreenSize); //렌더러가 가지고 있음.

			//화면에 그릴 2차원 배열 문자값
			std::unique_ptr<CHAR_INFO[]> charInfoArray;

			//그리기 정렬 값 이차원 배열
			std::unique_ptr<int[]> sortingOrderArray;
		};

		//화면에 그릴 데이터를 명령 단위로 저장하기 위한 구조체.
		struct RenderCommand //내부에서만 사용할 목적이므로 밖으로 내보낼 필요가 없음.
		{
			//화면에 그릴 문자값.
			std::wstring image;
			
			//위치
			Vector2 position = Vector2::Zero;

			//색상
			Color color = Color::White;

			//그리기 정렬 순서 값이 크면 우선 순위가 높음.
			int sortingOrder = -1; // << 경쟁 상황에서 우선 순위를 결정할 때 사용.

			bool isSighted = false;

			bool keepSighted = false;
		};

	public:
		Renderer(const Vector2& gameScreenSize, const Vector2& itemScreenSize);
		~Renderer();

		//화면에 그릴 데이터를 제출하는 함수.
		void Submit(
			const std::wstring& image,
			const Vector2& position,
			Color color = Color::White,
			int sortingOrder = 0,
			bool isSighted = false,
			bool keepSighted = false
		);

		void ScreenSubmit(
			const std::wstring& image,
			const Vector2& position,
			Color color = Color::White,
			int sortingOrder = 0,
			bool isSighted = false,
			bool keepSighted = false
		);

		inline void SetCameraView(const Vector2& newCameraView) 
		{
			cameraView = newCameraView;
		}

		// Draw 이벤트 함수 -Engine에서 호출
		void Draw();

		//전역 접근 함수
		static Renderer& Get();

	private:
		//그리기 작업을 시작할 때 프레임(화면)을 지우는 함수.
		void Clear();

		//전달 받은 렌더 명령을 활용해 화면을 그리는 함수.
		void DrawRenderQueue();

		//그린 결과를 화면에 표시하는 함수.
		void Present();

		//게터 
		const ScreenBuffer* const GetCurrentBuffer() const;


	private:
		//전역 접근 가능하도록 변수 선언
		static Renderer* instance;

		// 카메라
		Vector2 cameraView = Vector2::Zero;
		
		//이번프레임에 그릴 렌더 명령을 모아두는 배열
		//큐처럼 사용.
		std::vector<RenderCommand> renderQueue;
		std::vector<RenderCommand> itemRenderQueue;

		// 게임 화면 크기
		Vector2 gameScreenSize;
		// 아이템 화면 크기
		Vector2 itemScreenSize;
		// 전체 화면 크기
		Vector2 fullScreenSize;

		//글자 그리기 순서 2차원 배열을 관리하는 프레임 객체
		std::unique_ptr<Frame> frame;

		//이중 버퍼링 구현을 위한 화면 버퍼 2개
		std::unique_ptr<ScreenBuffer> screenBufferArray[2];

		//버퍼 인덱스
		int currentBufferIndex = 0;

	};
}
