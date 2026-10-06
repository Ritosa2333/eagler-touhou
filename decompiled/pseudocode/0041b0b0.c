// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41b0b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41b0b0_1 {
    char padding_0[576];
    unsigned int field_240;
    char padding_244[44];
    unsigned int field_270;
    char padding_274[1072];
    unsigned int field_6a4;
    unsigned int field_6a8;
    unsigned int field_6ac;
} st_41b0b0_1;

typedef struct st_41b0b0_0 {
    char padding_0[96];
    unsigned int field_60;
    char field_64;
    char padding_65[1219];
    char field_528;
    char padding_529[109];
    unsigned short field_596;
    char padding_598[1216];
    short field_a58;
    char padding_a5a[2];
    char field_a5c;
    char padding_a5d[1219];
    char field_f20;
} st_41b0b0_0;

extern st_41b0b0_0 *g_4b43c8;

unsigned int sub_41b0b0(void)
{
    st_41b0b0_0 *index;  // ecx
    unsigned int v8;  // ebx
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    st_41b0b0_1 *idx;  // edi
    unsigned int v10;  // esi
    unsigned int v11;  // ebp
    unsigned int v12;  // ftop
    unsigned int v13;  // ftop
    unsigned int v14;  // ftop
    unsigned int v15;  // ftop
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    index = g_4b43c8;
    v8 = &g_4b43c8->field_64;
    if (idx->field_270 % idx->field_240)
        return 0;
    v0 = v10;
    v11 = v8 + 1220;
    v1 = 2000;
    do
    {
        v2 = v1;
        if (*((char *)v8) & 1 && *((short *)(v11 + 110)) == 1 && *((int *)(208 * *((short *)(v11 + 1328)) + 4911744)) == 0xa1)
        {
            v13 = v12 - 1;
            if (/* unsupported instruction */)
            {
                v14 = v13 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v14 = v13 - 1;
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
                v3 = /* unsupported instruction */;
                /* unsupported instruction */
                v15 = v14 + 1;
            }
            else
            {
                v3 = nan;
                /* unsupported instruction */
                v15 = v14 + 1;
            }
            v16 = v15 - 1;
            if (/* unsupported instruction */)
            {
                v17 = v16 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v17 = v16 - 1;
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
                v4 = /* unsupported instruction */;
                /* unsupported instruction */
                v18 = v17 + 1;
            }
            else
            {
                v4 = nan;
                /* unsupported instruction */
                v18 = v17 + 1;
            }
            v19 = v18 - 1;
            if (/* unsupported instruction */)
            {
                v20 = v19 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v20 = v19 - 1;
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
            idx->field_6a4 = v3;
            idx->field_6a8 = v4;
            if (/* unsupported instruction */)
            {
                v5 = /* unsupported instruction */;
                /* unsupported instruction */
                v21 = v20 + 1;
            }
            else
            {
                v5 = nan;
                /* unsupported instruction */
                v21 = v20 + 1;
            }
            v22 = v21 - 1;
            if (/* unsupported instruction */)
            {
                v23 = v22 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v23 = v22 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            idx->field_6ac = v5;
            if (/* unsupported instruction */)
            {
                index->field_60 = /* unsupported instruction */;
                /* unsupported instruction */
                v12 = v23 + 1;
            }
            else
            {
                index->field_60 = nan;
                /* unsupported instruction */
                v12 = v23 + 1;
            }
            sub_40b5e0();
            /* unsupported instruction */
            /* unsupported instruction */
            index = g_4b43c8;
            g_4b43c8->field_60 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
    } while ((v8 += 2552, v11 += 2552, v1 = v2 - 1, v2 != 1));
    return 0;
}
