// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c941; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aa9f8;
extern HANDLE g_4b3c04;
extern unsigned int g_4d6458;

void sub_46c941(unsigned int a0, unsigned int v1)
{
    unsigned int v3;  // esi
    unsigned int *v4;  // eax
    int v0;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x4]

    sub_46fcec(&g_4aa9f8, 12);
    v3 = v1;
    if (v3)
    {
        if (g_4d6458 == 3)
        {
            sub_46ec9a(4);
            v2 = 0;
            v0 = sub_46edc8(v3);
            if (v0)
                sub_46edf8(v0, v3);
            v2 = 0xfffffffe;
            sub_46c997();
            if (v0)
            {
                sub_46fd31();
                return;
            }
            v1 = v1;
        }
        else
        {
            v1 = v3;
            v1 = v3;
        }
        if (!HeapFree(g_4b3c04, HEAP_NONE, v1))
        {
            v4 = sub_46e96f();
            *(v4) = sub_46e92d(GetLastError());
        }
    }
    sub_46fd31();
    return;
}
