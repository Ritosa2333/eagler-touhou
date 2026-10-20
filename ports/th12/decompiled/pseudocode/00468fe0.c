// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x468fe0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_468fe0_1 {
    char padding_0[12];
    struct st_468fe0_2 *field_c;
} st_468fe0_1;

typedef struct st_468fe0_0 {
    char padding_0[4104];
    unsigned int field_1008;
} st_468fe0_0;

typedef struct st_468fe0_3 {
    char padding_0[4116];
    struct st_468fe0_4 *field_1014;
} st_468fe0_3;

typedef struct st_468fe0_4 {
    struct st_468fe0_1 *field_0;
} st_468fe0_4;

typedef struct st_468fe0_2 {
    unsigned int field_0;
} st_468fe0_2;


int sub_468fe0(void)
{
    unsigned short v2;  // ftop
    unsigned short v3;  // ftop
    unsigned int v12;  // ecx
    unsigned int v13;  // eax
    int v14;  // edi
    unsigned int v15;  // eax
    st_468fe0_3 *v16;  // eax
    unsigned short v5;  // ftop
    unsigned short v6;  // ftop
    unsigned int v8;  // 4136
    st_468fe0_0 *idx;  // eax
    unsigned int v10;  // eax
    unsigned int v11;  // 4108
    int v0;  // [bp-0x4]
    unsigned int v1;  // [bp+0x4]

    /* unsupported instruction */
    /* unsupported instruction */
    v3 = v2 - 2;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (!(((v3 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
    {
        sub_4931e0(v0);
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
        v5 = v3 - 0;
        if (/* unsupported instruction */)
        {
            v6 = v5 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v6 = v5 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        v8 = _ccall(10, 13, (char)(((v6 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v8 & 1)
        {
            v15 = sub_4931e0(v14);
            v16->field_1014->field_0->field_c(v15);
            return;
        }
        v10 = idx->field_1008 - 4;
        v11 = _ccall(8, 3, idx->field_1008, 0xfffffffc, 0);
        if (v11 & 1)
            return;
        idx->field_1008 = v10;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v12 = *((int *)&idx->padding_0[8 + v10]);
        v13 = v10 - 4;
        idx->field_1008 = v13;
        v1 = v12;
        if (idx->padding_0[8 + v13] != 0x66 && idx->padding_0[8 + v13] == 105)
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
                v1 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v1 = nan;
                /* unsupported instruction */
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
