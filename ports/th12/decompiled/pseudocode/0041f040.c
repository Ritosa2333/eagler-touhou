// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41f040; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41f040_4 {
    char padding_0[148];
    int field_94;
} st_41f040_4;

typedef struct st_41f040_0 {
    char padding_0[975];
    char field_3cf;
    char padding_3d0[19260];
    unsigned int field_4f0c;
    char padding_4f10[7580];
    unsigned int field_6cac;
    char padding_6cb0[4];
    int field_6cb4;
    unsigned int field_6cb8;
    char padding_6cbc[32];
    int field_6cdc;
    char padding_6ce0[8];
    unsigned int field_6ce8;
    char padding_6cec[12];
    unsigned int field_6cf8;
    char padding_6cfc[52];
    unsigned int field_6d30;
    char padding_6d34[4];
    int field_6d38;
    int field_6d3c;
} st_41f040_0;

typedef struct st_41f040_5 {
    char padding_0[28];
    unsigned int field_1c;
    char padding_20[28];
    char field_3c;
} st_41f040_5;

typedef struct st_41f040_2 {
    char padding_0[959];
    char field_3bf;
} st_41f040_2;

typedef struct st_41f040_1 {
    char padding_0[102272];
    unsigned int field_18f80;
    char field_18f83;
    unsigned int field_18f84;
    unsigned int field_18f88;
    char padding_18f8c[12];
    unsigned int field_18f98;
    unsigned int field_18f9c;
    char padding_18fa0[4];
    unsigned int field_18fa4;
    unsigned int field_18fa8;
} st_41f040_1;

extern void g_4b0c48;
extern int g_4b0cd0;
extern void g_4b0cd4;
extern st_41f040_1 *g_4b43b8;
extern st_41f040_4 *g_4b43cc;
extern st_41f040_5 *g_4b43dc;
extern int g_4ce8cc;

unsigned int sub_41f040(st_41f040_0 *a0)
{
    st_41f040_0 *idx;  // esi
    int v15;  // edi
    int v23;  // edx
    int v24;  // eax
    int v25;  // ecx
    int v26;  // edx
    int v27;  // edx
    char v28;  // 4160
    unsigned int v29;  // fc3210
    unsigned int v30;  // ftop
    unsigned int v31;  // 4144
    st_41f040_0 *v32;  // ecx
    int v16;  // esi
    unsigned int v34;  // 4142
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ebx
    unsigned int i;  // ebx
    unsigned int v40;  // eax
    int v17;  // ebx
    st_41f040_1 *index;  // edi
    unsigned int v19;  // ecx
    st_41f040_2 *v20;  // eax
    char v21;  // 4108
    unsigned int v22;  // ftop
    unsigned int j;  // [bp-0x44]
    unsigned int v1;  // [bp-0x40]
    unsigned int v2;  // [bp-0x3c]
    st_41f040_0 *v3;  // [bp-0x38], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x34]
    st_41f040_2 *idx1;  // [bp-0x30], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x2c]
    unsigned int v7;  // [bp-0x28]
    unsigned int v8;  // [bp-0x24]
    unsigned int v9;  // [bp-0x20]
    unsigned int v10;  // [bp-0x1c]
    unsigned int v11;  // [bp-0x18]
    unsigned int v12;  // [bp-0x14]
    unsigned int v13;  // [bp-0x10]

    idx = a0;
    if (!sub_461920(idx->field_6cac, v15, v16, v17))
    {
        index = g_4b43b8;
        idx->field_6cac = 0;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v19 = idx->field_6cac;
        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v20 = sub_461920(v19);
        if (!v20)
            idx->field_6cac = 0;
        v21 = v20->field_3bf;
        *((unsigned int *)&g_4b43b8->padding_18fa0[0]) = 0;
        g_4b43b8->field_18fa4 = 0;
        *((char *)&g_4b43b8->field_18f80 + 3) = v21;
        g_4b43b8->field_18f98 = 1;
        *((unsigned int *)&g_4b43b8->padding_18f8c[8]) = 3;
        sub_401610();
        index = g_4b43b8;
        *((unsigned int *)&g_4b43b8->padding_18f8c[8]) = 0;
        g_4b43b8->field_18f98 = 0;
        *((char *)&g_4b43b8->field_18f80 + 3) = 0xff;
        *((unsigned int *)&g_4b43b8->padding_18fa0[0]) = 1;
        g_4b43b8->field_18fa4 = 1;
    }
    if (idx->field_6cb8)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx1 = sub_461920(idx->field_6cb4);
        if (!idx1)
            idx->field_6cb4 = 0;
        if (!g_4b43cc)
        {
            idx->field_6cb8 = 0;
            sub_461a70(idx->field_6cb4);
            idx->field_6cb4 = 0;
        }
        else if (!idx1)
        {
            idx->field_6cb8 = 0;
        }
        else
        {
            *((char *)&index->field_18f80 + 3) = idx1->field_3bf;
            index->field_18f98 = 1;
            *((unsigned int *)&index->padding_18f8c[8]) = 3;
            v23 = (int)((unsigned int)(2290649225 * g_4b43cc->field_94 >> 32) + g_4b43cc->field_94) >> 5;
            v24 = ((unsigned int)(v23) >> 31) + v23;
            if (v24 >= 1000)
                v24 = 999;
            sub_4015c0("%3d.", v24);
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&g_4b43b8->field_18f83) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v25 = g_4b43cc->field_94;
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v26 = (int)((unsigned int)(2290649225 * v25 >> 32) + v25) >> 5;
            v27 = (int)((unsigned int)(2290649225 * ((v25 - (((unsigned int)(v26) >> 31) + v26) * 60) * 100) >> 32) + (v25 - (((unsigned int)(v26) >> 31) + v26) * 60) * 100) >> 5;
            sub_4015c0("%.2ds", ((unsigned int)(v27) >> 31) + v27);
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            g_4b43b8->field_18f80 = 0xff808080;
            /* unsupported instruction */
            /* unsupported instruction */
            v28 = idx1->field_3bf;
            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&g_4b43b8->field_18f83) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            *((char *)&g_4b43b8->field_18f80 + 3) = v28;
            sub_41d0b0(&idx1);
            sub_4015c0("%3d.", idx1);
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&g_4b43b8->field_18f83) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_4015c0("%.2ds", v17);
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&g_4b43b8->field_18f83) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            *((char *)&g_4b43b8->field_18f80 + 3) = 0xff;
            *((unsigned int *)&g_4b43b8->padding_18f8c[8]) = 0;
            idx = a0;
            g_4b43b8->field_18f98 = 0;
            g_4b43b8->field_18f80 = 0xffffffff;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v29 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_6ce8) * 0x100 & 0x4500;
    /* unsupported instruction */
    v30 = v22;
    v31 = _ccall(10, 13, (char)((((unsigned short)v30 & 7) * 0x800 | (unsigned short)v29 & 0x4700) >> 8) & 5, 0, 0);
    if (!(v31 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_451f30();
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_451f30();
        v32 = &idx->field_6cf8;
        v3 = v32;
        v4 = 4;
        do
        {
            v3 = v32;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v34 = _ccall(10, 13, (char)((((unsigned short)v30 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&v32->padding_0[0])) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
            if (v34 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v36 = v30;
                if (!((char)((((unsigned short)v36 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    v37 = v36 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v37 = v36 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v30 = v37 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_451f30();
                v32 = v3;
            }
        } while ((v32 += 8, v4 -= 1, v3 = v32, v4 != 1));
    }
    v38 = 4;
    do
    {
        i = v38;
        sub_45c900(g_4ce8cc);
        v38 = i - 1;
    } while (i != 1);
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&g_4b43b8->padding_18f8c[8]) = 3;
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((char *)&g_4b43b8->field_18f80 + 3) = idx->field_3cf;
    idx1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&g_4b43b8->padding_18fa0[0]) = 2;
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4b43b8->field_18fa4 = 1;
    sub_401610();
    /* unsupported instruction */
    /* unsupported instruction */
    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = v6;
    v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v4 = v7;
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx1 = v8;
    sub_401610();
    /* unsupported instruction */
    /* unsupported instruction */
    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&g_4b43b8->padding_18fa0[0]) = 1;
    g_4b43b8->field_18fa4 = 1;
    v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx1 = v8;
    v4 = v7;
    v3 = v6;
    sub_4015c0("%d.", *((int *)&g_4b0c48) / *((int *)&g_4b0cd4));
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&g_4b43b8->field_18f83) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    sub_4015c0("%.2d", *((int *)&g_4b0c48) % *((int *)&g_4b0cd4) * 100 / *((int *)&g_4b0cd4));
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&g_4b43b8->field_18f83) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    sub_4015c0("/%d.", g_4b0cd0 / *((int *)&g_4b0cd4));
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
    *((int *)&g_4b43b8->field_18f83) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    sub_4015c0("00");
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&g_4b43b8->field_18f83) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((unsigned int *)&g_4b43b8->padding_18fa0[0]) = 2;
    g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4b43b8->field_18fa4 = 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v3 = v6;
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v4 = v7;
    idx1 = v8;
    sub_401610();
    /* unsupported instruction */
    /* unsupported instruction */
    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = v6;
    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v4 = v7;
    idx1 = v8;
    sub_401610();
    g_4b43b8->field_18f80 = 0xffffffff;
    *((unsigned int *)&g_4b43b8->padding_18fa0[0]) = 1;
    g_4b43b8->field_18fa4 = 1;
    *((char *)&g_4b43b8->field_18f80 + 3) = 0xff;
    *((unsigned int *)&g_4b43b8->padding_18f8c[8]) = 0;
    g_4b43b8->field_18f98 = 0;
    if (!g_4b43dc)
    {
        return 1;
    }
    else if (a0->field_6d38 < NULL)
    {
        return 1;
    }
    else if (!g_4b43dc->field_1c)
    {
        return 1;
    }
    else if (g_4b43dc->field_3c & 1)
    {
        return 1;
    }
    else if (!a0->field_6d30)
    {
        v2 = 2;
        do
        {
            j = v19;
            sub_45c900(g_4ce8cc);
            v19 = j - 1;
        } while (j != 1);
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v40 = a0->field_4f0c;
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        g_4b43b8->field_18f98 = 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        g_4b43b8->field_18f80 = v40;
        *((unsigned int *)&g_4b43b8->padding_18f8c[8]) = 3;
        sub_4015c0(".");
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&g_4b43b8->field_18f83) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_4015c0("%.2d", a0->field_6d3c);
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&g_4b43b8->field_18f83) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        g_4b43b8->field_18f80 = 0xffffffff;
        g_4b43b8->field_18f98 = 0;
        *((unsigned int *)&g_4b43b8->padding_18f8c[8]) = 0;
        return 1;
    }
    else
    {
        return 1;
    }
}
