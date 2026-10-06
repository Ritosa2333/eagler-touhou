// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x468ec0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_468ec0_1 {
    char padding_0[12];
    struct st_468ec0_2 *field_c;
} st_468ec0_1;

typedef struct st_468ec0_5 {
    char padding_0[16];
    unsigned int field_10;
} st_468ec0_5;

typedef struct st_468ec0_3 {
    char padding_0[4];
    struct st_468ec0_0 *field_4;
    char padding_8[4096];
    unsigned int field_1008;
    char padding_100c[8];
    struct st_468ec0_4 *field_1014;
} st_468ec0_3;

typedef struct st_468ec0_4 {
    struct st_468ec0_1 *field_0;
} st_468ec0_4;

typedef struct st_468ec0_2 {
    unsigned int field_0;
} st_468ec0_2;

typedef struct st_468ec0_0 {
    char padding_0[8];
    unsigned short field_8;
} st_468ec0_0;


int sub_468ec0(void)
{
    st_468ec0_3 *idx;  // eax
    unsigned int v3;  // ecx
    unsigned int v12;  // eax
    unsigned int v13;  // 4108
    unsigned int v14;  // ecx
    unsigned int v15;  // eax
    unsigned int v16;  // eax
    st_468ec0_5 *v4;  // ecx
    unsigned int v5;  // fc3210
    unsigned short v6;  // ftop
    unsigned int v7;  // 4148
    unsigned short v8;  // ftop
    unsigned short v9;  // ftop
    unsigned int v10;  // fc3210
    unsigned int v11;  // 4132
    unsigned int v0;  // [bp-0x4]

    if (idx->field_4->field_8 & 1 << ((char)v3 & 31))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v4 = &idx->field_4->padding_0[4 * v3 + 16];
        v5 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&v4->padding_0[0])) * 0x100 & 0x4500;
        /* unsupported instruction */
        v7 = _ccall(10, 13, (char)(((v6 & 7) * 0x800 | (unsigned short)v5 & 0x4700) >> 8) & 65, 0, 0);
        if (!(v7 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4931e0();
            if (!/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                return;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            return;
        }
        else
        {
            v8 = v6 - 1;
            if (/* unsupported instruction */)
            {
                v9 = v8 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v9 = v8 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v10 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&v4->padding_0[0])) * 0x100 & 0x4500;
            /* unsupported instruction */
            v11 = _ccall(10, 13, (char)(((v9 + 1 & 7) * 0x800 | (unsigned short)v10 & 0x4700) >> 8) & 0x44, 0, 0);
            if (v11 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v16 = sub_4931e0();
                idx->field_1014->field_0->field_c(v16);
                return;
            }
            v12 = idx->field_1008 - 4;
            v13 = _ccall(8, 3, idx->field_1008, 0xfffffffc, 0);
            if (!(v13 & 1))
            {
                idx->field_1008 = v12;
                v14 = *((int *)&idx->padding_8[v12]);
                v15 = v12 - 4;
                idx->field_1008 = v15;
                v0 = v14;
                if (idx->padding_8[v15] != 0x66 && idx->padding_8[v15] == 105)
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
                        v0 = /* unsupported instruction */;
                        /* unsupported instruction */
                    }
                    else
                    {
                        v0 = nan;
                        /* unsupported instruction */
                    }
                }
            }
            if (!/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                return;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            return;
        }
    }
    else if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
}
