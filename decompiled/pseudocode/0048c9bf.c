// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c9bf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad134;
extern char g_4d6308;
extern unsigned int g_4d6320[4];

unsigned int sub_48c9bf(int a0, HANDLE a1)
{
    unsigned int v4;  // edi
    unsigned int *v5;  // edi
    unsigned int v6;  // esi
    unsigned int v7;  // ebx
    HANDLE v8;  // ebx
    enum STD_HANDLE v0;  // [bp-0x18]
    HANDLE v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]

    v3 = v4;
    if (a0 >= 0 && a0 < *((int *)&g_4d6308))
    {
        v5 = &g_4d6320[a0 >> 5];
        v6 = (a0 & 31) * 64;
        if (*((int *)(v6 + *(v5))) == 0xffffffff)
        {
            v2 = v7;
            v8 = a1;
            if (g_4ad134 != 1)
            {
                *((HANDLE *)(v6 + *(v5))) = v8;
                return 0;
            }
            if (!a0)
            {
                v1 = v8;
                v0 = STD_INPUT_HANDLE;
                v1 = v8;
            }
            else if (a0 == 1)
            {
                v1 = v8;
                v0 = STD_OUTPUT_HANDLE;
                v1 = v8;
            }
            else if (a0 == 2)
            {
                v1 = v8;
                v0 = STD_ERROR_HANDLE;
                v1 = v8;
            }
            else
            {
                *((HANDLE *)(v6 + *(v5))) = v8;
                return 0;
            }
            SetStdHandle(v0, v1);
            *((HANDLE *)(v6 + *(v5))) = v8;
            return 0;
        }
    }
    *((unsigned int *)sub_46e96f()) = 9;
    *((unsigned int *)sub_46e982()) = 0;
    return 0xffffffff;
}
