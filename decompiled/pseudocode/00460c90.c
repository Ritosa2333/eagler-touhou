// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460c90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4ced1c;
extern unsigned int g_4cee34;

void sub_460c90(unsigned int a0)
{
    int v1;  // edi
    int v2;  // ebx

    g_4cee34 = &g_4ced1c;
    sub_430910();
    sub_460cb7(v1, v2);
    return;
}
