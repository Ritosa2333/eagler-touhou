// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ca40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48ca40_0 {
    unsigned int field_0;
    char field_4;
} st_48ca40_0;

extern unsigned int g_4ad134;
extern char g_4d6308;
extern unsigned int g_4d6320[4];

unsigned int sub_48ca40(int a0)
{
    unsigned int *v5;  // edi
    unsigned int v6;  // esi
    st_48ca40_0 *v7;  // eax
    int v8;  // ecx
    int v9;  // edi
    enum STD_HANDLE v0;  // [bp-0x18]
    HANDLE v1;  // [bp-0x14]
    int v2;  // [bp-0xc]
    int v3;  // [bp-0x8]
    char v4;  // [bp+0x0]

    if (a0 >= 0 && a0 < *((int *)&g_4d6308))
    {
        v5 = &g_4d6320[a0 >> 5];
        v6 = (a0 & 31) * 64;
        v7 = *(v5) + v6;
        if (v7->field_4 & 1 && v7->field_0 != 0xffffffff)
        {
            if (g_4ad134 == 1)
            {
                v8 = a0 - 0;
                if (!a0)
                {
                    v1 = NULL;
                    v0 = STD_INPUT_HANDLE;
                }
                else if (v8 == 1)
                {
                    v1 = NULL;
                    v0 = STD_OUTPUT_HANDLE;
                }
                else if (v8 == 2)
                {
                    v1 = NULL;
                    v0 = STD_ERROR_HANDLE;
                }
                else
                {
                    *((unsigned int *)(v6 + *(v5))) = 0xffffffff;
                    return 0;
                }
                SetStdHandle(v0, v1);
                *((unsigned int *)(v6 + *(v5))) = 0xffffffff;
                return 0;
            }
            else
            {
                *((unsigned int *)(v6 + *(v5))) = 0xffffffff;
                return 0;
            }
        }
    }
    *((unsigned int *)sub_46e96f(v9, v2, v3, &v4)) = 9;
    *((unsigned int *)sub_46e982()) = 0;
    return 0xffffffff;
}
