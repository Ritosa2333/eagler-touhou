// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4534d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern void g_4cf4e8;
extern unsigned int g_4d4768;

int sub_4534d0(void)
{
    unsigned int v0;  // [bp-0x4]

    g_4d4768 = CreateThread(NULL, NULL, sub_453460, &g_4cf4e8, THREAD_CREATE_RUN_IMMEDIATELY, &v0);
    return g_4d4768;
}
