// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41cb70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41cb70_0 {
    char padding_0[20];
    unsigned long long field_14;
    unsigned int field_1c;
    int field_20;
    unsigned long long field_24;
    unsigned long long field_2c;
    unsigned int field_34;
} st_41cb70_0;

extern st_41cb70_0 *g_4b43e0;
extern void* g_4b44e8;
extern char g_4ceace;

unsigned int sub_41cb70(void)
{
    unsigned short v4;  // ftop
    unsigned short v13;  // ftop
    unsigned short v14;  // ftop
    unsigned short v15;  // ftop
    unsigned int v5;  // 4116
    unsigned int v7;  // 4146
    int v8;  // eax
    unsigned short v10;  // ftop
    unsigned int v11;  // 4211
    unsigned short v12;  // ftop
    int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x4]

    sub_4508b0(v0);
    v5 = _ccall(10, 13, (char)(((v4 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, *((long long *)&(g_4b43e0->padding_0)[1])) * 0x100 & 0x4500 : CmpF(nan, *((long long *)&(g_4b43e0->padding_0)[1])) * 0x100 & 0x4500) & 0x4700) >> 8) & 5, 0, 0);
    if (!(v5 & 1))
    {
        if (/* unsupported instruction */)
            *((long long *)&(g_4b43e0->padding_0)[1]) = /* unsupported instruction */;
        else
            *((unsigned long long *)&(g_4b43e0->padding_0)[1]) = nan;
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
    v7 = _ccall(10, 13, (char)(((v4 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
    if (!(v7 & 1))
    {
        v8 = g_4b43e0->field_1c;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((long long *)&(g_4b43e0->padding_0)[1]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (v8 < NULL)
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
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)&g_4b43e0->field_2c + 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = v4 - 0;
        v11 = _ccall(10, 13, (char)(((v10 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v11 & 1))
            *((unsigned int *)((char *)&g_4b43e0->field_14 + 4)) = *((int *)((char *)&g_4b43e0->field_14 + 4)) + 1;
        else
            *((unsigned int *)((char *)&g_4b43e0->field_14 + 4)) = 0;
        if (!g_4b44e8)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b43e0->field_1c = 0;
        }
        else if (!((char)g_4b44e8[96] & 20))
        {
            v12 = v10 - 1;
            if (/* unsupported instruction */)
            {
                v13 = v12 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v13 = v12 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v14 = v13 - 1;
            if (/* unsupported instruction */)
            {
                v15 = v14 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v15 = v14 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((long long *)((char *)&g_4b43e0->field_24 + 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)(((v15 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x404c800000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((long long *)&g_4b43e0->field_20) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((unsigned int *)&g_4b44e8[96]) = (int)g_4b44e8[96] & 0xffffff7f;
                g_4b43e0->field_1c = 0;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((long long *)&g_4b43e0->field_20) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((unsigned int *)&g_4b44e8[96]) = (int)g_4b44e8[96] & 0xffffff7f;
                g_4b43e0->field_1c = 0;
            }
        }
        else
        {
            *((unsigned int *)&g_4b44e8[96]) = (int)g_4b44e8[96] & 0xffffff7f;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b43e0->field_1c = 0;
        }
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    g_4b43e0->field_1c = g_4b43e0->field_1c + g_4ceace + 1;
    return 1;
}
