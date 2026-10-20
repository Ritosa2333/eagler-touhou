// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4036f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4036f0_0 {
    char padding_0[172];
    struct st_4036f0_1 *field_ac;
    char padding_b0[12];
    struct st_4036f0_1 *field_bc;
    char padding_c0[36];
    struct st_4036f0_1 *field_e4;
} st_4036f0_0;

typedef struct st_4036f0_1 {
    unsigned int field_0;
} st_4036f0_1;

typedef struct st_4036f0_4 {
    struct st_4036f0_0 *field_0;
} st_4036f0_4;

extern unsigned int g_4ce8cc;
extern st_4036f0_4 *g_4ce8f0;
extern unsigned int g_4ced1c;
extern unsigned int g_4cede8;
extern unsigned int g_4cedec;
extern unsigned int g_4cedf0;
extern unsigned int g_4cedf4;
extern unsigned int g_4cee34;
extern unsigned int g_4cee38;
extern unsigned int g_4cf278;

unsigned int sub_4036f0(void* a0)
{
    void* idx;  // ebx
    char v10;  // al
    unsigned int v19;  // edi
    void* i;  // esi
    unsigned int v12;  // ecx
    unsigned int *iter;  // edi
    unsigned int v14;  // eax
    unsigned int index;  // esi
    unsigned int v16;  // eax
    void* v17;  // esi
    unsigned int v18;  // edi
    st_4036f0_0 *v0;  // [bp-0x80]
    unsigned int v1;  // [bp-0x68]
    unsigned int v2;  // [bp-0x64]
    unsigned int v3;  // [bp-0x60]
    unsigned int v4;  // [bp-0x5c]
    unsigned int v5;  // [bp-0x58]
    unsigned int v6;  // [bp-0x54]
    unsigned int v7;  // [bp-0x50]
    unsigned int v8;  // [bp-0x4c]

    idx = a0;
    v10 = (int)idx[13756];
    if (v10 & 8)
        return 1;
    if (!(v10 & 4) || (int)idx[13764] < 60)
    {
        sub_45a3c0();
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
            *((int *)&idx[14068]) = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            *((unsigned int *)&idx[14068]) = nan;
            /* unsupported instruction */
        }
        i = idx + 13836;
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
        v12 = 70;
        if (/* unsupported instruction */)
        {
            *((int *)&idx[14072]) = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            *((unsigned int *)&idx[14072]) = nan;
            /* unsupported instruction */
        }
        for (iter = &g_4ced1c; v12; i += 4)
        {
            v12 -= 1;
            *(iter) = *((int *)i);
            iter += 1;
        }
        g_4cee34 = &g_4ced1c;
        sub_430a70();
        g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
        g_4cee38 = 2;
        sub_45a3c0(g_4ce8f0, g_4cee34 + 0xcc);
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 1);
        sub_45a3c0(g_4ce8f0, 14, 1);
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 23, 4);
        sub_45a3c0(g_4ce8f0, 23, 4);
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 0x22, (int)idx[14112]);
        sub_45a3c0(g_4ce8f0, 0x22, (int)idx[14112]);
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 36, (int)idx[14088]);
        sub_45a3c0(g_4ce8f0, 36, (int)idx[14088]);
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 37, (int)idx[14092]);
        if ((char)idx[13756] & 4 && (int)idx[13784] < 0x22)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = g_4cede8;
            v4 = g_4cedf4 + g_4cedec;
            v3 = g_4cedf0 + g_4cede8;
            v2 = g_4cedec;
            v0 = g_4ce8f0->field_0;
            /* unsupported instruction */
            g_4ce8f0->field_0->field_ac(g_4ce8f0, 1, &v1, 3, 0, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0);
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = g_4cede8;
            v8 = g_4cedf4 + g_4cedec;
            v7 = g_4cedf0 + g_4cede8;
            v6 = g_4cedec;
            v0 = g_4ce8f0->field_0;
            /* unsupported instruction */
            g_4ce8f0->field_0->field_ac(g_4ce8f0, 1, &v5, 3, (int)idx[14112], (/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0);
        }
    }
    v14 = (int)idx[13756];
    if ((char)(int)idx[13756] & 4)
    {
        if ((int)idx[13764] < 30)
        {
            sub_4529a0(3, 30, 0, 0, 0, 11);
            *((unsigned int *)&idx[13756]) = (int)idx[13756] | 1;
            sub_4067e0(1);
        }
        else
        {
            *((char *)&idx[10103]) = 0;
            *((unsigned int *)&idx[13756]) = v14 & 0xfffffffe;
        }
    }
    index = g_4ce8cc;
    if ((char)idx[10103])
    {
        v16 = (int)idx[10100];
        *((unsigned int *)(g_4ce8cc + 8973648)) = 1;
        *((unsigned int *)(g_4ce8cc + 8973644)) = v16;
        *((char *)&idx[10103]) = 0;
    }
    memset(idx + 13744, 0, 12);
    if ((char)idx[13756] & 1)
    {
        if ((int)idx[1472])
        {
            sub_406550();
            sub_430300();
            sub_45a3c0();
            sub_45a3c0();
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 0);
            v17 = a0 + 460;
            v18 = 8;
            do
            {
                v19 = v18;
                if ((int)v17[1012])
                    sub_45c900(g_4ce8cc);
            } while ((v17 += 1204, v18 = v19 - 1, v19 != 1));
            sub_45a3c0();
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 1);
            g_4cee34 = &g_4ced1c;
            sub_430a70(g_4ce8f0, 14, 1);
            g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
            g_4cee38 = 2;
            idx = a0;
        }
        if (g_4cf278 != 1)
        {
            sub_45a3c0();
            g_4cf278 = 1;
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 28, 1);
        }
        sub_4046c0(idx, 0);
        sub_4046c0(idx, 1);
        sub_4046c0(idx, 2);
        sub_4046c0(idx, 3);
        sub_4046c0(idx, 4);
        sub_4046c0(idx, 5);
        sub_4046c0(idx, 6);
        sub_4046c0(idx, 7);
        sub_45a3c0(idx, 7);
        index = g_4ce8cc;
        if ((int)idx[10096])
            *((unsigned int *)&idx[10096]) = (int)idx[10096] + 1;
    }
    *((unsigned int *)(index + 8973648)) = 0;
    *((unsigned int *)(index + 8973644)) = 0x80808080;
    if ((int)idx[10104])
    {
        *((unsigned int *)(index + 8973648)) = 1;
        *((unsigned int *)(index + 8973644)) = 0xff404040;
    }
    sub_45a3c0();
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 0);
    sub_45a3c0(g_4ce8f0, 14, 0);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 23, 8);
    return 1;
}
