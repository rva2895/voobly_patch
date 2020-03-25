#include "iuserpatch.h"
#include "gamespeed.h"
#include <MMSystem.h>
#include <stdio.h>
#include <math.h>

extern IVoobly *g_pVoobly;

REC_PERFORMANCE::REC_PERFORMANCE(int interval)
{
	perf_inverval = interval;
}

void REC_PERFORMANCE::set_speed(float s)
{
	speed = s;
}

void REC_PERFORMANCE::add_perf_data(DWORD at, float interval)
{
	perf_data.push_back(std::pair<DWORD, float>(at, interval));
}

float REC_PERFORMANCE::get_perf()
{
	if (perf_data.size() == 0)
		return 0.0f;

	float avg = 0;
	int n = 0;

	DWORD max_time = perf_data[perf_data.size() - 1].first;
	int remove_until = 0;
	for (int i = 0; i < perf_data.size(); i++)
	{
		if (max_time - perf_data[i].first > perf_inverval)
			remove_until++;
		else
		{
			avg += perf_data[i].second;
			n++;
		}
	}
	auto first = perf_data.begin();
	auto last = perf_data.begin() + remove_until;
	if (remove_until != 0)
		perf_data.erase(first, last);

	return avg / n / 2;
}

REC_PERFORMANCE rec_perf(1000);

//recorded game
void __fastcall printSpeed(int speed)
{
	char* s = "";
	switch (speed)
	{
	case 0:
		s = "0.5";
		break;
	case 1:
		s = "0.7";
		break;
	case 2:
		s = "1.0";
		break;
	case 3:
		s = "1.3";
		break;
	case 4:
		s = "1.6";
		break;
	case 5:
		s = "2.0";
		break;
	case 6:
		s = "4.0";
		break;
	case 7:
		s = "8.0";
		break;
	case 8:
		s = "16.0";
		break;
	case 9:
		s = "unbounded";
		break;
	default:
		s = "";
		break;
	}
	g_pVoobly->ChatMessage("", "Playback Speed: %s", s);
}

__declspec(naked) void onSlowDown() //004FAF6B
{
	__asm
	{
		mov		ecx, [eax + 540h]
		test	ecx, ecx
		jle		_no_slow_down
		dec		ecx
_no_slow_down:
		mov		[eax + 540h], ecx
		//call	printSpeed
		mov     eax, 004FAF75h
		jmp     eax
	}
}

__declspec(naked) void onSpeedUp() //004FAF38
{
	__asm
	{
		mov		ecx, [eax + 540h]
		cmp		ecx, 9
		jge		_no_speed_up
		inc		ecx
_no_speed_up:
		mov		[eax + 540h], ecx
		//call	printSpeed
		mov     eax, 004FAF42h
		jmp     eax
	}
}

__declspec(naked) void onSpeedNormal() //004FAF9F
{
	__asm
	{
		mov		ecx, 2
		mov		[eax + 540h], ecx
		//call	printSpeed
		push	004FAFA9h
		ret
	}
}

#define SPEED_STAT_MAX 20

float rec_speed = 0;
//float avg_interval = 0;
float avg_speed = 0;

DWORD interval_game = 0;
DWORD interval_real = 0;
int n_speed_captured = 0;
//std::vector

int current_frame_interval = 50;

DWORD inn = 0;

float divisor = 1.0f;

void __stdcall onPrintTime2(char* s)
{
	//float avg = avg_interval / 2;

	char s2[0x100];
	strcpy(s2, s);
	sprintf(s, "%s (%2.2f / %2.2f -> %d) (%3.1f)", s2, avg_speed, rec_speed, current_frame_interval, divisor);
}

void __stdcall onPrintTime2_legacy(char* s)
{
	//float avg = avg_interval / 2;

	char s2[0x100];
	strcpy(s2, s);
	if (isRec())
		sprintf(s, "%s (%2.2f)", s2, rec_speed);
	else
		sprintf(s, "%s", s2);
}

__declspec(naked) void onPrintTime() //005E01E6
{
	__asm
	{
		lea     eax, [esp + 64h]
		push    eax
		call    onPrintTime2
		//call    onPrintTime2_legacy

		mov     edi, 20h
		mov     eax, 005E02B2h
		jmp     eax
	}
}

DWORD time_game_last = 0;
DWORD time_real_last = 0;
int time_i = 0;

float rec_speed_last = 0;

DWORD last_rec_speed_change = 0;

int __stdcall rec_speed_test(DWORD in, DWORD time_game, int speed)
{
	rec_speed_last = rec_speed;
	switch (speed)
	{
	case 0:
		rec_speed = 0.500f;
		break;
	case 1:
		rec_speed = 0.667f;
		break;
	case 2:
		rec_speed = 1.000f;
		break;
	case 3:
		rec_speed = 1.333f;
		break;
	case 4:
		rec_speed = 1.600f;
		break;
	case 5:
		rec_speed = 2.000f;
		break;
	case 6:
		rec_speed = 4.000f;
		break;
	case 7:
		rec_speed = 8.000f;
		break;
	case 8:
		rec_speed = 16.00f;
		break;
	case 9:
		rec_speed = INFINITY;
		break;
	default:
		rec_speed = NAN;
		break;
	}
	DWORD time_real = timeGetTime();
	
	rec_perf.add_perf_data(time_real, (float)(time_game - time_game_last) / (time_real - time_real_last));

	//interval_game += (time_game - time_game_last);
	//interval_real += (time_real - time_real_last);
	//n_speed_captured++;

	time_game_last = time_game;
	time_real_last = time_real;

	inn = in;

	if (rec_speed_last != rec_speed)
	{
		divisor = rec_speed;
		last_rec_speed_change = timeGetTime();
	}

	DWORD t = timeGetTime();
	if (t - last_rec_speed_change > 1000)
	{
		last_rec_speed_change = t;
		avg_speed = rec_perf.get_perf();

		float r = avg_speed / rec_speed;

		if (r < 0.99)
		{
			divisor /= sqrt(r);
		}
		if (r > 1.01)
		{
			divisor /= sqrt(r);
		}
		if (divisor > 16.0)
			divisor = 16.0;
		if (divisor < 0.25)
			divisor = 0.25;

		/*if (divisor == 16.0)
			current_frame_interval /= sqrt(r);
		else
			current_frame_interval /= sqrt(r);
		if (current_frame_interval > 50)
			current_frame_interval = 50;
		if (current_frame_interval < 16)
			current_frame_interval = 16;*/
	}

	return in / divisor;
}

__declspec(naked) void onCheckSpeed() //0061EB93
{
	__asm
	{
		//
		push    ecx
		mov     ecx, [ebx]
		push    eax
		push    ecx
		mov     eax, [ebp - 10h]
		push    eax
		call    rec_speed_test
		pop     ecx
		//
		/*cmp     eax, 1
		jnz     short _next1
		mov     eax, [ebp - 10h]		//1.5
		lea     eax, [eax + eax * 2]
		shr     eax, 1
		jmp     _end
_next1:
		cmp     eax, 2
		jnz     _next2
		mov     eax, [ebp - 10h]		//1.0
		jmp     _end
_next2:
		cmp     eax, 3
		jnz     _next3
		mov     eax, [ebp - 10h]		//0.75
		lea     eax, [eax + eax * 2]
		shr     eax, 2
		jmp     _end
_next3:
		cmp     eax, 4
		jnz     _next4
		mov     eax, [ebp - 10h]		//0.625
		lea     eax, [eax + eax * 4]
		shr     eax, 3
		jmp     _end
_next4:
		cmp     eax, 5
		jnz     _next5
		mov     eax, [ebp - 10h]		// 1/2
		shr     eax, 1
		jmp     _end
_next5:
		cmp     eax, 6
		jnz     _next6
		mov     eax, [ebp - 10h]		// 1/4
		shr     eax, 2
		jmp     _end
_next6:
		cmp     eax, 7
		jnz     _next7
		mov     eax, [ebp - 10h]		// 1/8
		shr     eax, 3
		jmp     _end
_next7:
		cmp     eax, 8
		jnz     _next8
		mov     eax, [ebp - 10h]		// 1/16
		shr     eax, 4*/
_end:
		mov     [edi + 204h], eax
_next8:
		mov     eax, 0061EBA1h
		jmp     eax
	}
}

bool screen_drawn = false;
bool allow_screen_draw = true;

DWORD last_draw_window = 0;
DWORD last_draw_game = 0;
DWORD last_speed_stat = 0;

DWORD last_screen_drawn = 0;

bool force_update_window = false;
bool force_update_view = false;

DWORD last_screen_draw_main = 0;

int __stdcall skip_frames()
{
	if (!isRec())
		return true;

	DWORD t = timeGetTime();

	if (current_frame_interval <= 16)
	{
		last_draw_window = t;
		return true;
	}

	/*if (t - last_screen_drawn > 100)
	{
		last_draw_window = t;
		return true;
	}*/

	if (force_update_window)
	{
		last_draw_window = t;
		force_update_window = false;
		return true;
	}

	if (t - last_draw_window > current_frame_interval)
	{
		last_draw_window = t;
		return true;
		//allow_screen_draw = true;
	}
	else
		return false;
}

__declspec(naked) void onDrawScreen2() //00428B02
{
	__asm
	{
		call    skip_frames
		test    eax, eax
		jz      do_skip2
		mov     eax, [esi]
		mov     ecx, esi
		call    dword ptr [eax + 120h]
		jmp     do_ok2
do_skip2:
		add     esp, 10h
		xor     eax, eax
		inc     eax
do_ok2:
		mov     ecx, 00428B08h
		jmp     ecx
	}
}

int delta = 0;
bool direction_asc = false;

int __stdcall allow_screen_draw_test()
{
	if (!isRec())
		return true;

	DWORD t = timeGetTime();
	last_screen_drawn = t;
	if (t - last_speed_stat > 200) //if it's time to evaluate performance
	{
		/*if (n_speed_captured == 0)
		{
			current_frame_interval = 16;
			return true;
		}

		avg_speed = (float)interval_game / interval_real / 2;
		interval_game = 0;
		interval_real = 0;

		bool direction_asc_now;

		if ((avg_speed / rec_speed) > 0.75)
		{
			current_frame_interval -= delta;
			direction_asc_now = false;
		}
		else if ((avg_speed / rec_speed) < 0.50)
		{
			current_frame_interval += delta;
			direction_asc_now = true;
		}

		if (direction_asc == direction_asc_now)
			delta++;
		else
			delta = 0;
		direction_asc = direction_asc_now;

		if (current_frame_interval < 16)
			current_frame_interval = 16;
		if (current_frame_interval > 100)
			current_frame_interval = 100;
		last_speed_stat = t;
		n_speed_captured = 0;*/
		
	}

	if (current_frame_interval < 50)
		return true;
	
	if (force_update_view)
	{
		force_update_view = false;
		return true;
	}

	if (t - last_draw_game > current_frame_interval)
	{
		last_draw_game = t;
		return true;
		allow_screen_draw = true;
	}
	else
		return false;
}

__declspec(naked) void onDrawView() //004B6A00
{
	__asm
	{
		push    ecx
		//call    allow_screen_draw_test
		call    skip_frames
		pop     ecx
		test    eax, eax
		jnz     allow_draw
		ret
allow_draw:
		push    esi
		mov     esi, ecx
		mov     eax, [esi + 20h]
		mov     ecx, 004B6A06h
		jmp     ecx
	}
}

__declspec(naked) void onDrawView2() //005FC160
{
	__asm
	{
		push    ecx
		call    allow_screen_draw_test
		pop     ecx
		test    eax, eax
		jz      no_allow_draw
		push    esi
		mov     esi, ecx
		mov     eax, 00618DC0h
		call    eax
		mov     eax, 005FC168h
		jmp     eax
no_allow_draw:
		ret
	}
}

__declspec(naked) void onScrollView() //004C2010
{
	__asm
	{
		xor     eax, eax
		inc     eax
		mov     force_update_window, al
		mov     force_update_view, al
		mov     eax, 006A35E0h
		mov     eax, [eax]
		mov     edx, 004C2015h
		jmp     edx
	}
}

void setGameSpeedHooks()
{
	//recorded game
	writeByte(0x0061EBC2, 9); //9 in new ver
	setHook((void*)0x0061EB93, onCheckSpeed);
	setHook((void*)0x004FAF6B, onSlowDown);
	setHook((void*)0x004FAF38, onSpeedUp);
	setHook((void*)0x004FAF9F, onSpeedNormal);

	setHook((void*)0x005E01E6, onPrintTime);

	//setHook((void*)0x00471CB0, onDrawScreen); //obsolete
	setHook((void*)0x00428B02, onDrawScreen2);
	//setHook((void*)0x004B6A00, onDrawView); //obsolete
	//setHook((void*)0x005FC160, onDrawView2);

	setHook((void*)0x004C2010, onScrollView);
	//writeByte(0x005FC160, 0xC3);
}
