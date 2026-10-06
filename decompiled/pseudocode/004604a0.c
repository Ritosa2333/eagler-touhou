// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4604a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b50c0;

void sub_4604a0(void)
{
    unsigned int v1;  // esi
    unsigned int v2;  // ebx
    int v3;  // edi

    if (v1 >= 32)
        return;
    if (!*((int *)&(&g_4b50c0)[4 * v1 + v2]))
        return;
    sub_4604e0(v3);
    sub_46ca4f(*((int *)&(&g_4b50c0)[4 * v1 + v2]));
    *((unsigned int *)&(&g_4b50c0)[4 * v1 + v2]) = 0;
    return;
}
