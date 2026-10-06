// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491781; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49f384;

void sub_491781(unsigned int a0)
{
    unsigned int v1;  // ecx
    unsigned int v2;  // esi
    void* v3;  // ebp
    int v4;  // edi

    sub_491e59(4);
    v2 = v1;
    *((unsigned int *)((char *)v3 - 16)) = v2;
    v4 = (int)v3[8];
    sub_46dfce(v4);
    *((unsigned int *)((char *)v3 - 4)) = 0;
    *((char **)v2) = &g_49f384;
    sub_491756(v4 + 12);
    sub_491f31(v4 + 12);
    return;
}
