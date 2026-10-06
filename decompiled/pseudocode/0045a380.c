// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45a380; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b56a0;
extern char g_4b56a4;
extern unsigned int g_4ce8cc;

void sub_45a380(void)
{
    unsigned int v1;  // ecx
    unsigned int v2;  // ecx

    v1 = &(&g_4b56a4)[g_4ce8cc];
    *((unsigned int *)(g_4ce8cc + 8607396)) = v1;
    *((unsigned int *)(g_4ce8cc + 8607400)) = v1;
    v2 = g_4ce8cc + 8607408;
    *((unsigned int *)&(&g_4b56a0)[g_4ce8cc]) = 0;
    *((unsigned int *)(g_4ce8cc + 8607404)) = 0;
    *((unsigned int *)(g_4ce8cc + 8935088)) = v2;
    *((unsigned int *)(g_4ce8cc + 8935092)) = v2;
    return;
}
