// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4501e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4501e0_1 {
    char padding_0[128];
    unsigned int field_80;
} st_4501e0_1;

typedef struct st_4501e0_0 {
    int field_0;
    char padding_4[60];
    unsigned long long field_40;
    char padding_48[32];
    unsigned long long field_68;
    char padding_70[4];
    int field_74;
    unsigned int field_78;
} st_4501e0_0;

typedef struct st_4501e0_3 {
    unsigned int field_0;
} st_4501e0_3;

typedef struct st_4501e0_2 {
    char padding_0[64];
    struct st_4501e0_3 *field_40;
    struct st_4501e0_3 *field_44;
    char padding_48[4];
    struct st_4501e0_3 *field_4c;
} st_4501e0_2;

typedef struct st_4501e0_6 {
    struct st_4501e0_2 *field_0;
} st_4501e0_6;

extern unsigned int g_4b43cc;
extern unsigned int g_4b43e0;
extern st_4501e0_6 *g_4ce8f0;
extern char g_4ce9dc;
extern unsigned int g_4cee5c;

void sub_4501e0(void)
{
    int v3;  // edi
    int v4;  // ebx
    unsigned int v11;  // ecx
    st_4501e0_1 *v12;  // edx
    unsigned int v13;  // edx
    unsigned int v15;  // 4116
    unsigned int v16;  // fc3210
    unsigned int v17;  // ftop
    unsigned int v18;  // 4116
    unsigned int v19;  // fc3210
    unsigned int iter;  // ftop
    st_4501e0_0 *idx;  // esi
    unsigned int v21;  // 4116
    unsigned int v22;  // fc3210
    unsigned int v23;  // 4116
    unsigned int v24;  // edi
    unsigned int v25;  // ecx
    unsigned int v6;  // ftop
    unsigned int v7;  // ftop
    unsigned int v8;  // ftop
    unsigned int v9;  // ftop
    int v10;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    sub_4508b0(v3, v4);
    if (/* unsupported instruction */)
        idx->field_40 = /* unsupported instruction */;
    else
        idx->field_40 = nan;
    v7 = v6 - 1;
    if (/* unsupported instruction */)
    {
        v8 = v7 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v8 = v7 - 1;
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
    v9 = v8 + 1;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = sub_4931e0();
    v11 = idx->field_78;
    v12 = &(&idx->field_0)[3 * v11 + 32];
    idx->field_74 = v10;
    if (*((int *)&v12->padding_0[0]) >= v10)
    {
        *((unsigned int *)&v12->padding_0[0]) = (unsigned int)((v10 < 0) + 1) & v10;
    }
    else
    {
        *((unsigned int *)&idx[1].padding_4[4 + 12 * v11]) = *((int *)&idx[1].padding_4[4 + 12 * v11]) + 1;
        if (*((int *)&idx[1].padding_4[4 + 12 * idx->field_78]) < 15)
            goto LABEL_45026b;
        v13 = idx->field_78 * 3;
        if (*((int *)&idx[1].padding_4[4 * v13]) < (&idx[1].field_0)[v13])
            *((unsigned int *)&idx[1].padding_4[4 * v13]) = *((int *)&idx[1].padding_4[4 * v13]) + 1;
    }
    *((unsigned int *)&idx[1].padding_4[4 + 12 * idx->field_78]) = 0;
LABEL_45026b:
    if (idx->field_74 < 0)
        idx->field_74 = 0;
    v1 = 0;
    sub_4508b0();
    v15 = _ccall(10, 13, (char)((((unsigned short)v9 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, idx->field_68) * 0x100 & 0x4500 : CmpF(nan, idx->field_68) * 0x100 & 0x4500) & 0x4700) >> 8) & 5, 0, 0);
    if (!(v15 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_68 = /* unsupported instruction */;
        else
            idx->field_68 = nan;
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
        v16 = CmpF(/* unsupported instruction */, 4580542482021164813) * 0x100 & 0x4500;
        /* unsupported instruction */
        v17 = v9 + 1;
    }
    else
    {
        v16 = CmpF(nan, 4580542482021164813) * 0x100 & 0x4500;
        /* unsupported instruction */
        v17 = v9 + 1;
    }
    v18 = _ccall(10, 13, (char)((((unsigned short)v17 & 7) * 0x800 | (unsigned short)v16 & 0x4700) >> 8) & 5, 0, 0);
    if (!(v18 & 1))
    {
        sub_4508b0();
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
            v19 = CmpF(/* unsupported instruction */, 4578647611560997945) * 0x100 & 0x4500;
            /* unsupported instruction */
            iter = v17 + 1;
        }
        else
        {
            v19 = CmpF(nan, 4578647611560997945) * 0x100 & 0x4500;
            /* unsupported instruction */
            iter = v17 + 1;
        }
        v21 = _ccall(10, 13, (char)((((unsigned short)iter & 7) * 0x800 | (unsigned short)v19 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v21 & 1))
        {
            do
            {
                Sleep(1);
                sub_4508b0(v3);
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
                    v22 = CmpF(/* unsupported instruction */, 4578647611560997945) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    iter += 1;
                }
                else
                {
                    v22 = CmpF(nan, 4578647611560997945) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    iter += 1;
                }
                v23 = _ccall(10, 13, (char)((((unsigned short)iter & 7) * 0x800 | (unsigned short)v22 & 0x4700) >> 8) & 5, 0, 0);
            } while (!(v23 & 1));
        }
        v0 = 0;
        if (!g_4ce8f0->field_0->field_4c(g_4ce8f0, 0, &v0))
        {
            v24 = v1;
            do
            {
            } while (v24 <= v1 && v1 && !v0 && (v24 = v1, !g_4ce8f0->field_0->field_4c(g_4ce8f0, 0, &v0)));
        }
    }
    else
    {
        v25 = idx->field_78 * 3;
        if (*((int *)&idx[1].padding_4[4 * v25]) > 0)
            *((unsigned int *)&idx[1].padding_4[4 * v25]) = *((int *)&idx[1].padding_4[4 * v25]) - 1;
    }
    sub_4508b0();
    if (/* unsupported instruction */)
    {
        idx->field_68 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_68 = nan;
        /* unsupported instruction */
    }
    sub_450810();
    if (g_4ce8f0->field_0->field_44(g_4ce8f0, 0, 0, 0, 0) < 0)
    {
        sub_431700();
        sub_44f370();
        g_4ce8f0->field_0->field_40(g_4ce8f0, &g_4ce9dc);
        sub_44f400(v3, v4);
        sub_431630();
        sub_451200();
        g_4cee5c = 2;
    }
    if (g_4b43e0)
        sub_41cb70();
    if (g_4b43cc)
        sub_40dd50();
    sub_4508b0();
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (*((int *)&idx[1].padding_4[12 * idx->field_78]) > 0)
        Sleep(*((int *)&idx[1].padding_4[12 * idx->field_78]));
    sub_4508b0();
    if (!/* unsupported instruction */)
    {
        *((unsigned long long *)&idx->padding_48[24]) = nan;
        /* unsupported instruction */
        return;
    }
    *((long long *)&idx->padding_48[24]) = /* unsupported instruction */;
    /* unsupported instruction */
    return;
}
