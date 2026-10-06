// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460f50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4ceaec;
extern unsigned int g_4cee34;

void sub_460f50(unsigned int a0)
{
    int v1;  // edi
    int v2;  // esi

    g_4cee34 = &g_4ceaec;
    sub_430910();
    sub_460f77(v1, v2);
    return;
}
