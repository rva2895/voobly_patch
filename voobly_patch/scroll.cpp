#include "iuserpatch.h"

extern IVoobly *g_pVoobly;

void setScrollHooks()
{
	g_pVoobly->Write(0x005FD5E5, "EB");
	g_pVoobly->Write(0x005FD61F, "EB");
	g_pVoobly->Write(0x005FD659, "EB");
	g_pVoobly->Write(0x005FD692, "EB");
}
