#include "GameTimer.h"

GameTimer::GameTimer()
{
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)&performanceFrequency))
	{
		hardwareHasPerformanceCounter = TRUE;
		::QueryPerformanceCounter((LARGE_INTEGER*)&lastTime);
		timeScale = 1.0f / performanceFrequency;
	}
	else
	{
		hardwareHasPerformanceCounter = FALSE;
		lastTime = ::timeGetTime();
		timeScale = 0.001f;
	}

	sampleCount = 0;
	currentFrameRate = 0;
	framePerSecond = 0;
	fpsTimeElapsed = 0.0f;

	currentTime = 0;
	for (int i = 0; i < MAX_SAMPLE_COUNT; ++i)
		frameTime[i] = 0;
	m_timeElapsed = 0;
	stopped = false;
}

GameTimer::~GameTimer()
{

}

void GameTimer::Tick(float lockFPS)
{
	if (hardwareHasPerformanceCounter)
		::QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	else
		currentTime = ::timeGetTime();

	float timeElapsed = (currentTime - lastTime) * timeScale;

	if (lockFPS > 0.0f)
	{
		while (timeElapsed < 1.0f / lockFPS)
		{
			if (hardwareHasPerformanceCounter)
				::QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			else
				currentTime = ::timeGetTime();

			timeElapsed = (currentTime - lastTime) * timeScale;
		}
	}

	lastTime = currentTime;

	if (fabsf(timeElapsed - m_timeElapsed) < 1.0f)
	{
		::memmove(&frameTime[1], frameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		frameTime[0] = timeElapsed;
		if (sampleCount < MAX_SAMPLE_COUNT)
			sampleCount++;
	}

	framePerSecond++;
	fpsTimeElapsed += timeElapsed;
	if (fpsTimeElapsed > 1.0f)
	{
		currentFrameRate = framePerSecond;
		framePerSecond = 0;
		fpsTimeElapsed = 0.0f;
	}

	m_timeElapsed = 0.0f;
	
	for (ULONG i = 0; i < sampleCount; ++i)
		timeElapsed += frameTime[i];

	if (sampleCount > 0)
		m_timeElapsed /= sampleCount;
}

unsigned long GameTimer::getFrameRate(LPTSTR lpszString, int nCharacters)
{
	if (lpszString)
	{
		_itow_s(currentFrameRate, lpszString, nCharacters, 10);
		wcscat_s(lpszString, nCharacters, _T("FPS)"));
	}

	return currentFrameRate;
}

float GameTimer::getTimeElapsed()
{
	return m_timeElapsed;
}

void GameTimer::Reset()
{
	__int64 performanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&performanceCounter);

	lastTime = performanceCounter;
	currentTime = performanceCounter;

	stopped = false;
}