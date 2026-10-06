// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46cfd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern HANDLE g_4b3c04;
extern unsigned int g_4d6458;

void* sub_46cfd0(unsigned int a0)
{
    unsigned int v2;  // esi
    unsigned int v3;  // esi
    void* v4;  // eax
    unsigned int v0;  // [bp-0x8]
    char v1;  // [bp+0x0]

    if (!g_4b3c04)
    {
        sub_47315e(&v1);
        sub_472f8d(30);
        sub_472c61(0xff); /* do not return */
    }
    else if (g_4d6458 == 1)
    {
        if (!a0)
            a0 = 1;
        return HeapAlloc(g_4b3c04, HEAP_NONE, a0);
    }
    else
    {
        v0 = v2;
        v3 = a0;
        if (g_4d6458 == 3)
        {
            v4 = sub_46cf81(v3);
            if (v4)
                return v4;
        }
        if (!v3)
            v3 = 1;
        return HeapAlloc(g_4b3c04, HEAP_NONE, v3 + 15 & 0xfffffff0);
    }
}
