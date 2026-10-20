// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44a5d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44a5d0_2 {
    unsigned int field_0;
    struct st_44a5d0_2 *field_4;
} st_44a5d0_2;

typedef struct st_44a5d0_1 {
    char padding_0[20];
    int field_14;
    char padding_18[16];
    unsigned int field_28;
    char padding_2c[12];
    unsigned int field_38;
    char padding_3c[8];
    unsigned int field_44;
    char padding_48[12];
    unsigned int field_54;
    unsigned int field_58;
    unsigned int field_5c;
    char padding_60[4];
    int field_64;
    int field_68;
} st_44a5d0_1;

typedef struct st_44a5d0_4 {
    char padding_0[64];
    unsigned int field_40;
    char padding_44[1080];
    unsigned int field_47c;
} st_44a5d0_4;

typedef struct st_44a5d0_3 {
    char padding_0[104];
    struct st_44a5d0_2 *field_68;
} st_44a5d0_3;

typedef struct st_44a5d0_0 {
    char padding_0[102272];
    unsigned int field_18f80;
    unsigned int field_18f84;
    unsigned int field_18f88;
    char padding_18f8c[12];
    unsigned int field_18f98;
    unsigned int field_18f9c;
    char padding_18fa0[4];
    unsigned int field_18fa4;
    unsigned int field_18fa8;
} st_44a5d0_0;

extern st_44a5d0_0 *g_4b43b8;
extern st_44a5d0_3 *g_4b43dc;

unsigned int sub_44a5d0(unsigned int a0)
{
    unsigned int v10;  // esi
    unsigned int *idx;  // esi
    st_44a5d0_4 *idx1;  // eax
    unsigned int v12;  // edi
    st_44a5d0_1 *index;  // ebx
    unsigned int v14;  // eax
    unsigned int v15;  // ecx
    unsigned int v16;  // edx
    int v17;  // eax
    st_44a5d0_2 *iter;  // eax
    unsigned int v19;  // eax
    unsigned int v0;  // [bp-0x50]
    double v1;  // [bp-0x4c], Other Possible Types: unsigned long
    unsigned int v2;  // [bp-0x48]
    unsigned int v3;  // [bp-0x44]
    unsigned int v4;  // [bp-0x40]
    unsigned int v5;  // [bp-0x18]
    unsigned int v6;  // [bp-0x14]
    unsigned int v7;  // [bp-0x10]
    unsigned int v8;  // [bp-0xc]
    unsigned int v9;  // [bp-0x8]

    v4 = v10;
    idx = g_4b43b8;
    v3 = v12;
    if (index->field_64 > 0)
    {
        v14 = index->field_54;
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
        v15 = index->field_58;
        v16 = index->field_5c;
        if (/* unsupported instruction */)
            g_4b43b8->field_18f84 = /* unsupported instruction */;
        else
            g_4b43b8->field_18f84 = nan;
        v7 = v14;
        if (/* unsupported instruction */)
        {
            g_4b43b8->field_18f88 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            g_4b43b8->field_18f88 = nan;
            /* unsupported instruction */
        }
        g_4b43b8->field_18f98 = 4;
        g_4b43b8->field_18f9c = 1;
        v17 = index->field_64;
        v9 = v16;
        v8 = v15;
        g_4b43b8->field_18fa4 = 0;
        g_4b43b8->field_18fa8 = 0;
        g_4b43b8->field_18f80 = ((unsigned int)(((int)(v17 % 10) >= 5) + 1) & 8355456) - 0x7f7f80;
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
            v1 = (double)/* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v1 = (double)nan;
            /* unsupported instruction */
        }
        v0 = "*%1.1f";
        sub_4020a0();
        /* unsupported instruction */
        /* unsupported instruction */
        idx = g_4b43b8;
        g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        g_4b43b8->field_18f88 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b43b8->field_18f80 = 0xffffffff;
        /* unsupported instruction */
        /* unsupported instruction */
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        if (index->field_68 > 0)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4021a0();
            /* unsupported instruction */
            /* unsupported instruction */
            idx = g_4b43b8;
        }
        if (/* unsupported instruction */)
            idx[25569] = /* unsupported instruction */;
        else
            idx[25569] = nan;
        if (/* unsupported instruction */)
        {
            idx[25570] = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx[25570] = nan;
            /* unsupported instruction */
        }
        idx[25568] = 0xffffffff;
        idx[25575] = 0;
        idx[25577] = 1;
        idx[25578] = 1;
    }
    if (!index->field_38)
    {
        return 1;
    }
    else if (index->field_14 >= 60)
    {
        iter = g_4b43dc->field_68;
        if (!g_4b43dc->field_68)
            return 1;
        while (*((int *)(iter->field_0 + 10164)) != index->field_38)
        {
            if (!iter->field_4)
                return 1;
        }
        idx[25574] = 2;
        idx[25575] = 1;
        v19 = sub_412530();
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
        v6 = v19;
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
            v7 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v7 = nan;
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
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v8 = nan;
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
        idx[25568] = 0xc080ffc0;
        if (/* unsupported instruction */)
        {
            v9 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v9 = nan;
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
        v5 = index->field_28 - index->field_14;
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
            v7 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v7 = nan;
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
            v5 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v5 = nan;
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
        (&v1)[1] = sub_4931e0() % 100;
        v1 = sub_4931e0(*((unsigned int *)((void*)&v1 + 4)));
        sub_4020a0("%2d:%.2d", v1);
        g_4b43b8->field_18f9c = 0;
        g_4b43b8->field_18f80 = 0xffffffff;
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
        v2 = index->field_44;
        if (/* unsupported instruction */)
        {
            v6 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v6 = nan;
            /* unsupported instruction */
        }
        idx1 = sub_461920();
        if (!idx1)
            index->field_44 = 0;
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
        idx1->field_47c = idx1->field_47c | 8;
        if (/* unsupported instruction */)
        {
            idx1->field_40 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx1->field_40 = nan;
            /* unsupported instruction */
        }
        sub_461e30();
        return 1;
    }
    else
    {
        return 1;
    }
}
