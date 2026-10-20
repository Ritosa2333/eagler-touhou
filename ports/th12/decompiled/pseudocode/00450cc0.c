// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x450cc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_450cc0_1 {
    unsigned int field_0;
} st_450cc0_1;

typedef struct st_450cc0_0 {
    char padding_0[8];
    struct st_450cc0_1 *field_8;
    char padding_c[20];
    struct st_450cc0_1 *field_20;
    char padding_24[4];
    struct st_450cc0_1 *field_28;
    char padding_2c[20];
    struct st_450cc0_1 *field_40;
} st_450cc0_0;

typedef struct st_450cc0_5 {
    struct st_450cc0_0 *field_0;
} st_450cc0_5;

extern int g_4a26d4;
extern int g_4a2700;
extern int g_4a272c;
extern int g_4a2758;
extern int g_4a277c;
extern int g_4a27a0;
extern int g_4a27c0;
extern int g_4a27e8;
extern int g_4a2820;
extern int g_4a285c;
extern int g_4a2870;
extern int g_4a2890;
extern int g_4a28e0;
extern int g_4a2930;
extern st_450cc0_5 *g_4ce8ec;
extern st_450cc0_5 *g_4ce8f0;
extern unsigned int g_4ce944;
extern unsigned int g_4ce984;
extern char g_4ce9c4;
extern unsigned int g_4ce9dc;
extern unsigned int g_4cea84;
extern unsigned int g_4cea88;
extern unsigned int g_4cea8c;
extern unsigned int g_4cea90;
extern char g_4ceaca;
extern char g_4ceacd;
extern char g_4cead3;
extern void g_4ceae8;
extern unsigned int g_4cee64;
extern unsigned int g_4cee68;
extern unsigned int g_4cee6c;
extern unsigned int g_4cee78;
extern char g_4cee84;
extern unsigned int g_4ceedc;
extern char g_4cef14;
extern unsigned int g_4cf3f0;
extern unsigned int g_4cf3f4;
extern char g_4cf418;
extern void g_4cf428;

unsigned int sub_450cc0(void)
{
    int v43;  // edi
    int v44;  // esi
    int v45;  // ebp
    int v46;  // ebx
    unsigned int v47;  // eax
    unsigned int v48;  // esi
    unsigned int v49;  // ecx
    unsigned int *i;  // esi
    unsigned int *iter;  // edi
    unsigned int v0;  // [bp-0x148]
    unsigned int v1;  // [bp-0x144]
    unsigned int v2;  // [bp-0x140]
    unsigned int v3;  // [bp-0x13c]
    unsigned int v4;  // [bp-0x138]
    unsigned int v5;  // [bp-0x134]
    char *v6;  // [bp-0x130]
    char *v7;  // [bp-0x12c]
    char *v8;  // [bp-0x128], Other Possible Types: unsigned int
    unsigned int v9;  // [bp-0x118]
    unsigned int v10;  // [bp-0x10c]
    st_450cc0_0 **v11;  // [bp-0x108], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0x104]
    unsigned int v13;  // [bp-0x100]
    unsigned int v14;  // [bp-0xfc]
    unsigned int v15;  // [bp-0xf8]
    unsigned int v16;  // [bp-0xf4]
    unsigned int v17;  // [bp-0xf0]
    unsigned int v18;  // [bp-0xec]
    unsigned int v19;  // [bp-0xe8]
    char v20;  // [bp-0xd5]
    unsigned long long v21;  // [bp-0xd4]
    char v22;  // [bp-0xb8]
    int v23;  // [bp-0x9c]
    char v24;  // [bp-0x75]
    unsigned int v25;  // [bp-0x6c]
    unsigned int v26;  // [bp-0x68]
    unsigned int v27;  // [bp-0x58]
    unsigned int v28;  // [bp-0x54]
    unsigned int v29;  // [bp-0x50]
    char v30;  // [bp-0x4c], Other Possible Types: unsigned int
    unsigned int v31;  // [bp-0x48]
    unsigned int v32;  // [bp-0x44]
    unsigned int v33;  // [bp-0x40]
    char v34;  // [bp-0x3c]
    unsigned int v35;  // [bp-0x30]
    unsigned int v36;  // [bp-0x28]
    unsigned int v37;  // [bp-0x24]
    unsigned int v38;  // [bp-0x20]
    unsigned int v39;  // [bp-0x1c]
    unsigned int v40;  // [bp-0x18]
    unsigned int v41;  // [bp-0x14]

    v24 = 1;
    sub_477420(&v34, 0, 56, v43, v44, v45, v46);
    g_4ce8ec->field_0->field_20(g_4ce8ec, 0, &v30);
    v47 = v29;
    g_4cea84 = v27;
    g_4cea88 = v28;
    g_4cea8c = v47;
    g_4cea90 = v30;
    if (g_4ceacd)
    {
        if (v47 != 60)
        {
            v23 = &g_4a26d4;
            sub_464220(&g_4a26d4);
            v47 = v29;
            *((unsigned int *)&g_4cf428) = *((int *)&g_4cf428) & 0xffffffef;
        }
        if (!g_4ceacd)
            goto LABEL_450d4e;
        v33 = v30;
        v35 = 1;
        if (g_4cf428 & 16 || g_4cead3 == 3 || (v41 = 1, v47 != 60))
            v41 = 0x80000000;
        v36 = 1;
    }
    else
    {
LABEL_450d4e:
        if (g_4ceae8 & 1)
        {
            v33 = 23;
            g_4ceaca = 1;
        }
        else if (g_4ceaca == 0xff)
        {
            v33 = 22;
            g_4ceaca = 0;
            sub_464220(&g_4a2700);
        }
        else
        {
            v33 = (unsigned int)((g_4ceaca) + 22);
        }
        if (g_4cf418)
        {
            g_4cee64 = 1;
            goto LABEL_450e1b;
        }
        else if (!g_4cee64)
        {
            v40 = 60;
            if (g_4cf428 & 16)
                v41 = 0x80000000;
            else
                v41 = ((unsigned int)((g_4cead3 != 3) + 1) & 0x7fffffff) + 1;
            v23 = &g_4a272c;
            sub_464220(&g_4a272c);
            v35 = 1;
        }
        else
        {
LABEL_450e1b:
            v23 = &g_4a2758;
            v40 = 0;
            v35 = 1;
            v41 = 0x80000000;
            sub_464220(&g_4a2758);
        }
    }
    g_4cee78 = g_4cee78 | 2;
    v31 = 640;
    v32 = 480;
    v37 = 1;
    v38 = 80;
    v39 = 1;
    g_4cee68 = 1;
    v48 = 0;
    while (1)
    {
        if (!(g_4ceae8 & 2))
        {
            v14 = g_4cf3f0;
            v11 = g_4ce8ec;
            if (g_4ce8ec->field_0->field_40(g_4ce8ec, 0, 1, g_4cf3f0, 64, &v23, &g_4ce8f0) < 0)
            {
                if (v48)
                    sub_464220(&g_4a277c);
                v9 = g_4cf3f0;
                if (g_4ce8ec->field_0->field_40(g_4ce8ec, 0, 1, g_4cf3f0, 32, &v22, &g_4ce8f0) >= 0)
                {
                    sub_464220(&g_4a285c);
                    g_4cee78 = g_4cee78 & 0xfffffffe;
                    goto LABEL_45103c;
                }
                else if (v48)
                {
                    sub_464220(&g_4a27a0);
                    goto LABEL_450f4d;
                }
            }
            else
            {
                sub_464220(&g_4a2870);
                g_4cee78 = g_4cee78 | 1;
                goto LABEL_45103c;
            }
        }
        else
        {
LABEL_450f4d:
            *((st_450cc0_5 ***)&v21) = &g_4ce8f0;
            if (g_4ce8ec->field_0->field_40(g_4ce8ec, 0, 2, g_4cf3f0, 32, &v46, v21) >= 0)
            {
                sub_464220(&g_4a2820);
                g_4cee78 = g_4cee78 & 0xfffffffe;
                v20 = 0;
LABEL_45103c:
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
                v49 = 14;
                i = &v21;
                if (/* unsupported instruction */)
                {
                    v17 = /* unsupported instruction */;
                    /* unsupported instruction */
                    break;
                }
                else
                {
                    v17 = nan;
                    /* unsupported instruction */
                    break;
                }
            }
            else if (!g_4cee64)
            {
                sub_464220(&g_4a27c0);
                v25 = 0;
                g_4cee68 = 0;
                v48 = 1;
            }
            else if (v26 == 1)
            {
                v26 = 0x80000000;
            }
            else
            {
                sub_464300(&g_4a27e8);
                if (!g_4ce8ec)
                    return 1;
                g_4ce8ec->field_0->field_8(g_4ce8ec);
                g_4ce8ec = 0;
                return 1;
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
    for (iter = &g_4ce9dc; v49; i += 1)
    {
        v49 -= 1;
        *(iter) = *(i);
        iter += 1;
    }
    if (/* unsupported instruction */)
    {
        v18 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v18 = nan;
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
    v8 = 0;
    if (/* unsupported instruction */)
    {
        v8 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v8 = nan;
        /* unsupported instruction */
    }
    sub_4317d0(v8);
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
    v8 = &v11;
    v7 = &v14;
    v6 = &v17;
    v5 = &g_4ce944;
    if (/* unsupported instruction */)
    {
        v10 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v10 = nan;
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
    v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    D3DXMatrixLookAtLH();
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
        v4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v4 = nan;
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v1 = nan;
        /* unsupported instruction */
    }
    v0 = &g_4ce984;
    D3DXMatrixPerspectiveFovLH();
    g_4ce8f0->field_0[2].field_28(g_4ce8f0, 2, &g_4ce944);
    g_4ce8f0->field_0[2].field_28(g_4ce8f0, 3, &g_4ce984);
    (*((int *)&g_4ce8f0->field_0[2].padding_2c[12]))(g_4ce8f0, &g_4ce9c4);
    (*((int *)&g_4ce8f0->field_0->padding_c[16]))(g_4ce8f0, &g_4cee84);
    if (!(g_4cef14 & 64))
        sub_464220(&g_4a2890);
    if (g_4ceedc <= 0x100)
        sub_464220(&g_4a28e0);
    if (!(g_4ceae8 & 1) && *((char *)((void*)&3 + 3)))
    {
        if (!g_4ce8ec->field_0->field_28(g_4ce8ec, 0, 1, v9, 0, 3, 21))
        {
            g_4cee78 = g_4cee78 | 4;
        }
        else
        {
            g_4cee78 = g_4cee78 & 0xfffffffb;
            *((unsigned int *)&g_4ceae8) = *((int *)&g_4ceae8) | 1;
            sub_464220(&g_4a2930);
        }
    }
    sub_451200();
    sub_451e60();
    g_4cf3f4 = 0;
    g_4cee6c = 0;
    return 0;
}
