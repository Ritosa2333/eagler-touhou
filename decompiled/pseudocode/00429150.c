// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x429150; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_429150_0 {
    char padding_0[388];
    unsigned int field_184;
    char padding_188[680];
    unsigned int field_430;
    char padding_434[32];
    unsigned int field_454;
    unsigned int field_458;
    unsigned int field_45c;
    unsigned int field_460;
    char padding_464[16];
    unsigned int field_474;
} st_429150_0;

unsigned int sub_429150(st_429150_0 *idx)
{
    int v8;  // edi
    int v9;  // esi
    unsigned int v18;  // 4170
    unsigned int v20;  // ftop
    unsigned int v21;  // edx
    unsigned int v23;  // ftop
    unsigned int v24;  // 4145
    st_429150_0 *idx1;  // edi
    unsigned int v27;  // ftop
    st_429150_0 *idx2;  // edi
    st_429150_0 *v30;  // edi
    unsigned int v32;  // 4146
    unsigned int v33;  // ecx
    st_429150_0 *index;  // edi
    int v35;  // edx
    unsigned int v11;  // ftop
    unsigned int v12;  // ftop
    unsigned int v13;  // ftop
    unsigned int v14;  // 4336
    unsigned int v16;  // ftop
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x18]
    unsigned int v4;  // [bp-0x14]
    unsigned int v5;  // [bp-0x10]
    unsigned int v6;  // [bp-0xc]

    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_42e880((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), v8, v9);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v12 = v11 - 3;
    v13 = v12 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v14 = _ccall(10, 13, (char)((((unsigned short)v12 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
    if (v14 & 1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v16 = v13 + 1;
        if ((((unsigned short)v16 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = _ccall(10, 13, (char)((((unsigned short)v16 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v18 & 1))
                goto LABEL_4291e8;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v20 = v16 + 1;
            if ((((unsigned short)v20 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                return 0;
            }
        }
        else
        {
LABEL_4291e8:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v20 = v16 + 1;
        }
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v20 = v13 + 2;
    }
    v21 = 0;
    if ((char)idx->field_184 & 1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v23 = v20 + 1;
        v24 = _ccall(10, 13, (char)((((unsigned short)v20 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v24 & 1))
        {
            if (!((char)idx->field_184 & 16))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx1 = &idx->field_454;
                *((int *)&idx1->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((unsigned int *)&idx1->padding_0[4]) = v5;
                *((unsigned int *)&idx1->padding_0[8]) = v6;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_458 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_460 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_474 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_428450(0);
                v23 -= 0;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v21 = 1;
        }
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v23 = v20 + 1;
    }
    if ((char)idx->field_184 & 2)
    {
        /* unsupported instruction */
        v27 = v23 + 1;
        if (!((char)((((unsigned short)v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            if (!((char)idx->field_184 & 16))
            {
                idx2 = &idx->field_454;
                *((int *)&idx2->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((unsigned int *)&idx2->padding_0[4]) = v5;
                *((unsigned int *)&idx2->padding_0[8]) = v6;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_458 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_460 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_474 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_428450(0);
            }
            v21 = 1;
        }
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v27 = v23 + 1;
    }
    if ((char)idx->field_184 & 4)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)((((unsigned short)v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            if (!((char)idx->field_184 & 16))
            {
                v30 = &idx->field_454;
                *((int *)&v30->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((unsigned int *)&v30->padding_0[4]) = v5;
                *((unsigned int *)&v30->padding_0[8]) = v6;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = v5;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v30->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                idx->field_460 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v27 += 1;
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_474 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_428450(0);
            }
            v21 = 1;
        }
    }
    if ((char)idx->field_184 & 8)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v32 = _ccall(10, 13, (char)((((unsigned short)v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v3) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (v32 & 1)
            goto LABEL_4293c9;
        if (!((char)idx->field_184 & 16))
        {
            v33 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            index = &idx->field_454;
            *((unsigned int *)&index->padding_0[0]) = v3;
            *((unsigned int *)&index->padding_0[4]) = v33;
            *((unsigned int *)&index->padding_0[8]) = v5;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v0 = v33;
            *((int *)&index->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            idx->field_460 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_474 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_428450(0);
        }
    }
    else
    {
LABEL_4293c9:
        if (!v21)
            return 0;
    }
    v35 = *((int *)&idx[1].padding_188[56]);
    idx->field_430 = idx->field_430 & 0xfffffeff;
    if (v35 >= 0)
        sub_453d90();
    return 1;
}
