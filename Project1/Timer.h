#pragma once


//Keeps track of elapsed time so we can use it things like physics calculations
class Timer {
public:
	Timer();
	~Timer();

	float tick(); //update timer
	float getElapsedTimeMS(); //amount of time passed in milliseconds
	float getElapsedTimeSeconds(); //
	float getCurrentTime(); //amount of time since start of program
private:
	float currentTime, previousTime, elapsedTime;
};