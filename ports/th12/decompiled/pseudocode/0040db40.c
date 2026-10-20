// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40db40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40db40_1 {
    char padding_0[2432];
    unsigned int field_980;
} st_40db40_1;

typedef struct st_40db40_2 {
    char padding_0[60];
    unsigned int field_3c;
} st_40db40_2;

typedef struct st_40db40_0 {
    char padding_0[13756];
    unsigned int field_35bc;
} st_40db40_0;

extern st_40db40_0 *g_4b43c0;
extern st_40db40_2 *g_4b43c4;
extern st_40db40_1 *g_4b4514;
extern unsigned int g_4ce8cc;

unsigned int sub_40db40(unsigned int a0)
{
    void* idx;  // edi
    int v8;  // ecx
    unsigned int i;  // ebp
    unsigned short v17;  // ftop
    unsigned short v18;  // ftop
    unsigned short v19;  // ftop
    st_40db40_1 *v20;  // ecx
    unsigned int v21;  // fc3210
    unsigned short v22;  // ftop
    unsigned int v23;  // 4116
    void* v24;  // esi
    unsigned int v25;  // ebp
    int v9;  // esi
    void* index;  // edi
    unsigned int j;  // ebp
    unsigned int *idx1;  // eax
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned int v12;  // fc3210
    unsigned short v13;  // ftop
    void* v14;  // esi
    unsigned int v15;  // ebp
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]

    if (!((char)idx[124] & 1))
        return 1;
    *((unsigned int *)&idx[144]) = (int)idx[144] + 1;
    if ((int)idx[40] >= 60)
        g_4b43c0->field_35bc = g_4b43c0->field_35bc & 0xfffffffe;
    if ((int)idx[40] >= 300 && !((char)idx[124] & 8))
    {
        v8 = (int)idx[128] - ((int)idx[132] - ((int)(((int)idx[132] >> 31 & 3) + (int)idx[132]) >> 2)) / ((int)idx[0x88] - 300);
        *((int *)&idx[128]) = v8 - (int)(v8 % 10);
    }
    sub_464a80(v9);
    if ((int)idx[40] >= 120)
    {
        if (!((char)idx[124] & 4))
        {
            v10 = v17 - 1;
            if (/* unsupported instruction */)
            {
                v11 = v10 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v11 = v10 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v12 = CmpF(/* unsupported instruction */, g_4b4514->field_980) * 0x100 & 0x4500;
                /* unsupported instruction */
                v13 = v11 + 1;
            }
            else
            {
                v12 = CmpF(nan, g_4b4514->field_980) * 0x100 & 0x4500;
                /* unsupported instruction */
                v13 = v11 + 1;
            }
            if (!((char)(((v13 & 7) * 0x800 | (unsigned short)v12 & 0x4700) >> 8) & 65))
            {
                v14 = idx + 20;
                v15 = 3;
                do
                {
                    i = v15;
                    sub_461970(*((int *)v14));
                    v14 += 4;
                    v15 = i - 1;
                } while (i != 1);
                *((unsigned int *)&idx[124]) = (int)idx[124] | 4;
            }
        }
        else
        {
            v18 = v17 - 1;
            if (/* unsupported instruction */)
            {
                v19 = v18 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v19 = v18 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v20 = g_4b4514;
            if (/* unsupported instruction */)
            {
                v21 = CmpF(/* unsupported instruction */, g_4b4514->field_980) * 0x100 & 0x4500;
                /* unsupported instruction */
                v22 = v19 + 1;
            }
            else
            {
                v21 = CmpF(nan, g_4b4514->field_980) * 0x100 & 0x4500;
                /* unsupported instruction */
                v22 = v19 + 1;
            }
            v23 = _ccall(10, 13, (char)(((v22 & 7) * 0x800 | (unsigned short)v21 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v23 & 1))
            {
                v24 = idx + 20;
                v25 = 3;
                do
                {
                    j = v25;
                    sub_461970(*((int *)v24));
                    v24 += 4;
                    v25 = j - 1;
                } while (j != 1);
                *((unsigned int *)&index[124]) = (int)index[124] & 0xfffffffb;
            }
        }
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v0 = nan;
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v1 = nan;
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&index[172]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&index[176]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&index[180]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx1 = sub_461920(v20, g_4ce8cc, (int)index[32]);
    if (idx1)
    {
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            idx1[268] = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx1[268] = nan;
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            idx1[269] = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx1[269] = nan;
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            idx1[270] = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx1[270] = nan;
            /* unsupported instruction */
        }
    }
    if (!((char)(int)index[124] & 32) || g_4b43c4->field_3c)
        return 1;
    *((unsigned int *)&index[124]) = (int)index[124] & 0xffffffdf;
    return 1;
}
