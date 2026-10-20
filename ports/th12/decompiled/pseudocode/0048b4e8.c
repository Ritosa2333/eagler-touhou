// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b4e8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aaff8;
extern HANDLE g_4b3c04;
extern char g_4b40bc;
extern char g_4d6448;
extern char g_4d6458;

int sub_48b4e8(void)
{
    void* idx;  // ebp
    unsigned int v3;  // ecx
    unsigned int v4;  // eax
    unsigned int v5;  // 4107
    unsigned int v6;  // 4152
    unsigned int v7;  // 4153
    unsigned int v8;  // esi
    unsigned int v9;  // ebx
    unsigned int v0;  // [bp-0xc]

    sub_46fcec(&g_4aaff8, 12);
    v3 = (int)idx[8];
    if (v3 > 0)
    {
        v0 = 0xffffffe0;
        v4 = 0xffffffe0 / v3;
        v5 = (int)idx[12];
        v6 = _ccall(12, 0, v4 < v5, v4 < v5);
        v7 = _ccall(4, 18, -(v4 < v5) + 1, 0, v6);
        if (!(v7 & 1))
            goto LABEL_48b52b;
        *((unsigned int *)sub_46e96f()) = 12;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
LABEL_48b52b:
        v8 = (int)idx[12] * v3;
        *((unsigned int *)&idx[8]) = v8;
        if (!v8)
            v8 = 1;
        do
        {
            v9 = 0;
            *((unsigned int *)((char *)idx - 28)) = 0;
            if (v8 > 0xffffffe0)
                continue;
            if (*((int *)&g_4d6458) == 3 && !(v8 = v8 + 15 & 0xfffffff0, *((unsigned int *)&idx[12]) = v8, (int)idx[8] > *((int *)&g_4d6448)))
            {
                sub_46ec9a(4);
                *((unsigned int *)((char *)idx - 4)) = 0;
                *((int **)((char *)idx - 28)) = sub_46fa07((int)idx[8]);
                *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
                sub_48b5e4();
                v9 = *((int *)((char *)idx - 28));
                if (!v9)
                    goto LABEL_48b59d;
                sub_477420(v9, 0, (int)idx[8]);
            }
            if (v9)
                return sub_46fd31();
LABEL_48b59d:
            v9 = HeapAlloc(g_4b3c04, HEAP_ZERO_MEMORY, v8);
            if (v9)
                return sub_46fd31();
            if (!*((int *)&g_4b40bc))
            {
                if (v9 || !(int)idx[16])
                    return sub_46fd31();
                *((unsigned int *)(int)idx[16]) = 12;
                return sub_46fd31();
            }
        } while (sub_46ff67(v8));
        if ((int)idx[16])
            *((unsigned int *)(int)idx[16]) = 12;
    }
    return sub_46fd31();
}
