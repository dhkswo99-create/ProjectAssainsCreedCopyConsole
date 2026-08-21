#pragma once
#include <random>

namespace Util
{
	//랜덤 엔진 반환 함수
	inline std::mt19937& GetRandomEngine()
	{
		//프로그램 실행 중 한 번만 생성되는 랜덤 엔진.
		static std::mt19937 engine;
		return engine;
	}

	inline int RandomRange(int min, int max)
	{
		std::uniform_int_distribution<int> distribution(min, max);

		return distribution(GetRandomEngine());
	}
	
	inline float RandomRange(float min, float max)
	{
		std::uniform_real_distribution<float> distribution(min, max);

		return distribution(GetRandomEngine());
	}

	//랜덤 시드 설정 함수
	inline void SetRandomSeed()
	{
		//하드웨어 기반 난수 생성기 (시드 값 제공)
		std::random_device randomDevice;
		
		//랜덤 엔진에 종자값 설정
		GetRandomEngine().seed(randomDevice());
	}
}