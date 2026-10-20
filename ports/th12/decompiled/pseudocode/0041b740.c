// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41b740; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41b740_0 {
    char padding_0[1164];
    unsigned short field_48c;
    unsigned short field_48e;
    unsigned int field_490;
    unsigned int field_494;
    unsigned int field_498;
    unsigned int field_49c;
    char padding_4a0[4];
    unsigned int field_4a4;
    char padding_4a8[16];
    unsigned int field_4b8;
    char padding_4bc[4];
    unsigned int field_4c0;
    char padding_4c4[12];
    unsigned int field_4d0;
    unsigned int field_4d4;
    unsigned int field_4d8;
    unsigned int field_4dc;
    unsigned int field_4e0;
    unsigned int field_4e4;
    unsigned int field_4e8;
    unsigned int field_4ec;
    unsigned int field_4f0;
    unsigned int field_4f4;
    char padding_4f8[16];
    unsigned int field_508;
    char padding_50c[124];
    unsigned int field_588;
    unsigned int field_58c;
    unsigned int field_590;
    char padding_594[12];
    unsigned int field_5a0;
    unsigned int field_5a4;
    char padding_5a8[220];
    unsigned short field_684;
    unsigned short field_686;
    char padding_688[2];
    unsigned short field_68a;
    unsigned int field_68c;
    unsigned int field_690;
    unsigned int field_694;
} st_41b740_0;

typedef struct st_41b740_1 {
    char padding_0[20];
    struct st_41b740_2 *field_14;
} st_41b740_1;

typedef struct st_41b740_2 {
    unsigned int field_0;
} st_41b740_2;

typedef struct st_41b740_3 {
    struct st_41b740_1 *field_0;
    char padding_4[76];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
} st_41b740_3;

typedef struct st_41b740_5 {
    char padding_0[24];
    struct st_41b740_3 *field_18;
    char padding_1c[1136];
    struct st_41b740_4 *field_48c;
} st_41b740_5;

typedef struct st_41b740_4 {
    char padding_0[8];
    struct st_41b740_4 *field_8;
} st_41b740_4;

extern st_41b740_5 *g_4b44f4;

unsigned int sub_41b740(st_41b740_0 *idx)
{
    st_41b740_0 *index;  // esi
    int v27;  // edi
    unsigned int v36;  // edx
    unsigned int v37;  // cc_op
    unsigned int v38;  // cc_dep1
    unsigned int v39;  // 4170
    int v28;  // esi
    int v29;  // ebx
    st_41b740_3 *idx1;  // edi
    st_41b740_5 *v31;  // ecx
    st_41b740_5 *v32;  // ecx
    unsigned int v33;  // ftop
    unsigned int v34;  // ebp
    st_41b740_5 *v0;  // [bp-0xc4]
    unsigned int v1;  // [bp-0xb8]
    st_41b740_5 *v2;  // [bp-0xb0], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0xa4]
    unsigned int v4;  // [bp-0xa0]
    unsigned int v5;  // [bp-0xa0]
    unsigned int v6;  // [bp-0x9c]
    unsigned int v7;  // [bp-0x9c]
    unsigned int v8;  // [bp-0x98]
    unsigned int v9;  // [bp-0x94]
    unsigned int v10;  // [bp-0x90]
    unsigned int v11;  // [bp-0x88]
    unsigned int v12;  // [bp-0x84]
    unsigned int v13;  // [bp-0x80]
    unsigned int v14;  // [bp-0x7c]
    unsigned int v15;  // [bp-0x78]
    unsigned int v16;  // [bp-0x74]
    unsigned int v17;  // [bp-0x70]
    unsigned int v18;  // [bp-0x6c]
    unsigned int v19;  // [bp-0x68]
    unsigned int v20;  // [bp-0x64]
    unsigned int v21;  // [bp-0x60]
    unsigned int v22;  // [bp-0x5c]
    unsigned int v23;  // [bp-0x58]
    char v24;  // [bp-0x4]

    index = &idx->field_48c;
    sub_477420(index, 0, 532, v27, v28, &v24, v29);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_49c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_4a4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned short *)&index->padding_0[0]) = 23;
    idx->field_68a = 1;
    idx->field_684 = 1;
    idx->field_48e = 0;
    idx->field_686 = 1;
    idx->field_690 = 22;
    idx->field_694 = 40;
    idx->field_68c = 131;
    idx1 = g_4b44f4->field_18;
    g_4b44f4->field_48c = idx1;
    if (!idx1)
        return 0;
    /* unsupported instruction */
    /* unsupported instruction */
    sub_4938c0();
    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_4939f0();
    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_4938c0();
    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_4939f0();
    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    while (1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = v32;
        /* unsupported instruction */
        /* unsupported instruction */
        v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_41c580((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v0 = v31;
        /* unsupported instruction */
        sub_41c580((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = idx1->field_58;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v8 = idx1->field_50;
        v9 = idx1->field_54;
        sub_41c580((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_508 = 0x2000;
        *((unsigned int *)&index->padding_0[80]) = 1;
        *((unsigned int *)&index->padding_0[72]) = 0;
        *((unsigned int *)&index->padding_0[76]) = 0x400;
        *((unsigned int *)&index->padding_0[0x44]) = 180;
        *((unsigned int *)&index->padding_0[52]) = 0x200;
        *((unsigned int *)&index->padding_0[44]) = 1200;
        idx->field_588 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_590 = 1;
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_58c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v34 = 0;
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_5a0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_5a4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v33 += 2;
        v5 = v4;
        v7 = v6;
        if (!((char)((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            while (1)
            {
                v36 = v34;
                v37 = 15;
                v38 = v36 & 0x80000001;
                if ((v36 & 0x80000001) < NULL)
                {
                    v37 = 18;
                    v38 = ((v36 & 0x80000001) - 1 | 0xfffffffe) + 1;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&index->padding_0[104]) = 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&index->padding_0[92]) = 180;
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&index->padding_0[96]) = 9;
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&index->padding_0[100]) = 0x2000000;
                v39 = _ccall(4, v37, v38, 0, 0);
                if (v39 & 1)
                {
                    *((int *)&index->padding_0[84]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&index->padding_0[88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&index->padding_0[84]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&index->padding_0[88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_490 = v5;
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                idx->field_494 = v7;
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_498 = v8;
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_40b5e0();
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_590 = 0;
                /* unsupported instruction */
                /* unsupported instruction */
                v34 += 1;
                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                    break;
            }
        }
        idx1->field_0->field_14(0, 0);
        v32 = g_4b44f4;
        if (!g_4b44f4->field_48c)
            return 0;
        idx1 = g_4b44f4->field_48c->field_8;
        g_4b44f4->field_48c = idx1;
        if (!idx1)
            return 0;
    }
}
