#include "iuserpatch.h"

extern IVoobly *g_pVoobly;

struct timeline_colors
{
	DWORD offset1;
	DWORD offset2;
	char* color1;
	char* color2;
};

timeline_colors colors[8] =
{
	{ 0x005E1F44, 0x005E1F49, "829BCC", "0A007F" },	//blue
	{ 0x005E1F50, 0x005E1F55, "FF0000", "800000" },	//red
	{ 0x005E1F5C, 0x005E1F61, "00FF00", "008000" },	//green
	{ 0x005E1F68, 0x005E1F6D, "FFFF00", "808000" },	//yellow
	{ 0x005E1F74, 0x005E1F79, "00FFFF", "008080" },	//cyan
	{ 0x005E1F80, 0x005E1F85, "FF00FF", "7F1079" },	//purple
	{ 0x005E1F8C, 0x005E1F91, "D1D1D2", "808080" },	//gray
	{ 0x005E1F98, 0x005E1F9D, "F44E2E", "AD1900" }	//orange
};

void setTimelineHooks()
{
	for (int i = 0; i < 8; i++)
	{
		g_pVoobly->Write(colors[i].offset1, colors[i].color1);
		g_pVoobly->Write(colors[i].offset2, colors[i].color2);
	}
}
