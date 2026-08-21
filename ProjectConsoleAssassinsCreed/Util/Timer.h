#pragma once

class Timer
{
public:
	Timer(float targetTime = 1.0f);

	float GetTime() { return elapsedTime; }

	//타이머 시간 업데이트 함수
	void Tick(float deltaTime);

	//경과 시간 리셋 함수.
	void Reset();

	//목표 시간 재설정 함수
	void SetTargetTime(float targetTime);

	//설정한 시간이 지났는지 확인하는 함수
	bool IsTimeOut() const {	return elapsedTime >= targetTime; }

private:
	//경과시간계산용변수
	float elapsedTime = 0.0f;
	
	//타이머목표시간
	float targetTime = 0.0f;
};

