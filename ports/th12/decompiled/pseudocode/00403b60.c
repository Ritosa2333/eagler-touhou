// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x403b60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4046c0_6 {
    char padding_0[20];
    unsigned int field_14;
    struct st_4046c0_4 *field_18;
    char padding_1c[428];
    unsigned int field_1c8;
    char padding_1cc[13284];
    unsigned int field_35b0;
    unsigned int field_35b4;
    unsigned int field_35b8;
} st_4046c0_6;

typedef struct st_4046c0_4 {
    unsigned short field_0;
    unsigned short field_2;
    char padding_4[12];
    char field_10;
} st_4046c0_4;

typedef struct st_403b60_0 {
    char padding_0[188];
    struct st_403b60_1 *field_bc;
    char padding_c0[36];
    struct st_403b60_1 *field_e4;
} st_403b60_0;

typedef struct st_403b60_1 {
    unsigned int field_0;
} st_403b60_1;

typedef struct st_403b60_3 {
    struct st_403b60_0 *field_0;
} st_403b60_3;

extern unsigned int g_4ce8cc;
extern st_403b60_3 *g_4ce8f0;
extern unsigned int g_4ced1c;
extern unsigned int g_4cee34;
extern unsigned int g_4cee38;
extern unsigned int g_4cf278;

unsigned int sub_403b60(st_4046c0_6 *idx)
{
    char v3;  // al
    char v4;  // al
    unsigned int *iter;  // edi
    unsigned int v5;  // ecx
    unsigned int v6;  // eax
    int v7;  // edi
    int v8;  // esi
    int v9;  // ebx
    st_4046c0_6 *i;  // esi
    unsigned int v11;  // ecx
    unsigned int v0;  // [bp-0x2c]
    st_403b60_0 **v1;  // [bp-0x28]
    int v2;  // [bp-0x24]

    v3 = *((int *)&idx[1].padding_0[0]);
    if (v3 & 8)
        return 1;
    if (v3 & 4 && *((int *)&idx[1].padding_0[8]) >= 60)
    {
        v4 = *((int *)&idx[1].padding_0[0]);
        if (v4 & 4 && *((int *)&idx[1].padding_0[8]) >= 30)
            idx->padding_1cc[9643] = 0;
        if (v4 & 1)
        {
            sub_45a3c0();
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 0);
            if (g_4cf278 != 1)
            {
                sub_45a3c0();
                g_4cf278 = 1;
                v2 = 28;
                v1 = g_4ce8f0;
                g_4ce8f0->field_0->field_e4(g_4ce8f0, 28, 1);
            }
            sub_4046c0(idx, 8);
            sub_4046c0(idx, 9);
            sub_4046c0(idx, 10);
            sub_4046c0(idx, 11);
            sub_45a3c0(v1, v2);
        }
        *((unsigned int *)(g_4ce8cc + 8973648)) = 0;
        *((unsigned int *)(g_4ce8cc + 8973644)) = 0x80808080;
        if (*((int *)&idx->padding_1cc[9644]))
        {
            *((unsigned int *)(g_4ce8cc + 8973648)) = 1;
            *((unsigned int *)(g_4ce8cc + 8973644)) = 0xff404040;
        }
        if (*((int *)&idx[1].padding_0[8]) > 0)
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
            v0 = v5;
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
            sub_464a20();
            if (*((int *)&idx[1].padding_0[8]) <= 0)
            {
                v6 = *((int *)&idx[1].padding_0[0]);
                idx->padding_1cc[9643] = 0xff;
                if ((char)v6 & 2)
                    *((unsigned int *)&idx[1].padding_0[0]) = v6 | 8;
                *((unsigned int *)&idx[1].padding_0[0]) = *((int *)&idx[1].padding_0[0]) & 0xfffffff9;
                *((unsigned int *)&idx->padding_1cc[9640]) = 0xffffff;
            }
        }
        sub_45a3c0();
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 0);
        sub_45a3c0(g_4ce8f0, 14, 0);
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 23, 8);
        if (!g_4cf278)
            return 1;
        sub_45a3c0();
        g_4cf278 = 0;
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 28, 0);
        return 1;
    }
    sub_45a3c0(v7, v8, v9);
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
        *((int *)&idx[1].padding_1c[284]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned int *)&idx[1].padding_1c[284]) = nan;
        /* unsupported instruction */
    }
    i = &idx[1].padding_1c[52];
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
    v11 = 70;
    if (/* unsupported instruction */)
    {
        *((int *)&idx[1].padding_1c[288]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned int *)&idx[1].padding_1c[288]) = nan;
        /* unsupported instruction */
    }
    for (iter = &g_4ced1c; v11; i = &i->padding_0[4])
    {
        v11 -= 1;
        *(iter) = *((int *)&i->padding_0[0]);
        iter += 1;
    }
    g_4cee34 = &g_4ced1c;
    sub_430a70();
    g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
    g_4cee38 = 2;
    if (g_4cf278)
    {
        sub_45a3c0();
        g_4cf278 = 0;
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 28, 0);
    }
    sub_45a3c0();
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 0);
    sub_45a3c0(g_4ce8f0, 14, 0);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 23, 8);
    sub_4611d0(g_4ce8f0, 23, 8);
    sub_45a3c0();
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 23, 4);
    sub_4611d0(g_4ce8f0, 23, 4);
    sub_45a3c0();
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 23, 4);
    sub_45a3c0(g_4ce8f0, 23, 4);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 0x22, *((int *)&idx[1].padding_1c[328]));
    sub_45a3c0(g_4ce8f0, 0x22, *((int *)&idx[1].padding_1c[328]));
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 36, *((int *)&idx[1].padding_1c[304]));
}
