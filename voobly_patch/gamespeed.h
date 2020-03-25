#pragma once

#include <vector>

class REC_PERFORMANCE
{
private:
	int perf_inverval;
	float speed;
	std::vector<std::pair<DWORD, float>> perf_data;
public:
	REC_PERFORMANCE(int);
	void set_speed(float);
	void add_perf_data(DWORD, float);
	float get_perf();
};

void setGameSpeedHooks();
