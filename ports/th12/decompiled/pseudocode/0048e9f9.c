// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48e9f9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4ab188;
extern char g_4d6308;
extern unsigned int g_4d6320[4];

int sub_48e9f9(unsigned int a0)
{
    void* idx;  // ebp
    int v2;  // eax
    unsigned int *v3;  // ebx
    unsigned int v4;  // edi
    int v5;  // 4097
    unsigned int v6;  // 4132
    unsigned int v7;  // 4133
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4ab188, 16);
    v2 = (int)idx[8];
    if (v2 == -0x2)
    {
        *((unsigned int *)sub_46e982()) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        goto LABEL_48ea20;
    }
    if (v2 < 0 || v2 >= *((int *)&g_4d6308) || (v3 = (v2 >> 5) * 4 + &g_4d6320[0], v4 = (v2 & 31) * 64, !(*((char *)(*(v3) + v4 + 4)) & 1)))
    {
        *((unsigned int *)sub_46e982()) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        goto LABEL_48ea48;
    }
    else
    {
        v5 = (int)idx[16];
        v6 = _ccall(12, 0, 0x7fffffff < v5, 0x7fffffff < v5);
        v7 = _ccall(4, 18, -(0x7fffffff < v5) + 1, 0, v6);
        if (v7 & 1)
        {
            *((unsigned int *)sub_46e982()) = 0;
            *((unsigned int *)sub_46e96f()) = 22;
LABEL_48ea48:
            sub_470f2d(0, 0, 0, 0, 0);
LABEL_48ea20:
        }
        else
        {
            sub_48cb3d(v2);
            *((unsigned int *)((char *)idx - 4)) = 0;
            if (*((char *)(*(v3) + v4 + 4)) & 1)
            {
                *((unsigned int *)((char *)idx - 28)) = sub_48e437((int)idx[8], (int)idx[12], (int)idx[16]);
            }
            else
            {
                *((unsigned int *)sub_46e96f()) = 9;
                *((unsigned int *)sub_46e982()) = 0;
                *((unsigned int *)((char *)idx - 28)) = 0xffffffff;
            }
            *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
            sub_48eaec();
        }
    }
    return (unsigned int)sub_46fd31(&g_4ab188, 16, v0, a0);
}
