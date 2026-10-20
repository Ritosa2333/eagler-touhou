// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b369; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;
extern unsigned int g_4ad13c;

void sub_47b369(void)
{
    unsigned int v7;  // eax
    unsigned int v8;  // edi
    unsigned int v9;  // esi
    unsigned int ch;  // eax
    unsigned int ch1;  // eax
    unsigned int count;  // eax
    unsigned int v13;  // esi
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    long long v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4[2];  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]

    v7 = g_4ad138;
    v3 = 0;
    v5 = 0;
    v1 = v8;
    if (v7 != 3141592654 && v7 & 0xffff0000)
    {
        g_4ad13c = ~(v7);
        return;
    }
    v0 = v9;
    GetSystemTimeAsFileTime(v4);
    ch = GetCurrentProcessId();
    ch1 = GetCurrentThreadId();
    count = GetTickCount();
    QueryPerformanceCounter(&v2);
    v13 = *(&v4[1]) ^ *(&v4[0]) ^ ch ^ ch1 ^ count ^ *((unsigned int *)((void*)&v2 + 4)) ^ (unsigned int)v2;
    if (v13 == 3141592654)
    {
        v13 = 3141592655;
    }
    else if (!(v13 & 0xffff0000))
    {
        v13 |= v13 * 0x10000;
    }
    g_4ad138 = v13;
    g_4ad13c = ~(v13);
    return;
}
