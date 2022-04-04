#pragma once

#include "stdafx.h"

const ULONG MAX_SAMPLE_COUNT = 50;

class GameTimer
{
private:
	bool hardwareHasPerformanceCounter;
	float timeScale;
	float m_timeElapsed;
	__int64 currentTime;
	__int64 lastTime;
	__int64 performanceFrequency;

	float frameTime[MAX_SAMPLE_COUNT];
	ULONG sampleCount;

	unsigned long currentFrameRate;
	unsigned long framePerSecond;
	float fpsTimeElapsed;

	bool stopped;

public:
	GameTimer();
	virtual ~GameTimer();

	void Start() {  }
	void Stop() {  }
	void Reset();
	void Tick(float lockFPS = 0.0f);
	unsigned long getFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	float getTimeElapsed();
};

