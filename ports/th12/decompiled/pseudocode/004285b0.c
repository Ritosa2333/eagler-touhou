// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4285b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4a06ac;

int sub_4285b0(void)
{
    int v2;  // eax
    int v0;  // [bp-0x4]

    sub_427ec0(v0);
    *((char **)v2) = &g_4a06ac;
    sub_477420(v2 + 1108, 0, 480);
    sub_4027e0();
    sub_4027e0();
    return;
}
