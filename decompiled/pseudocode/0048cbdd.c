// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48cbdd; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d6320[4];

void sub_48cbdd(int a0)
{
    LeaveCriticalSection(g_4d6320[a0 >> 5] + (a0 & 31) * 64 + 12);
    return;
}
