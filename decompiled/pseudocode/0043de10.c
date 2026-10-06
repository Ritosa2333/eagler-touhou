// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43de10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43de10_0 {
    char padding_0[1244];
    unsigned int field_4dc;
    char padding_4e0[8];
    int field_4e8;
    unsigned int field_4ec;
    unsigned int field_4f0;
    struct st_43de10_1 *field_4f4;
} st_43de10_0;

typedef struct st_43de10_1 {
    unsigned int field_0;
} st_43de10_1;

int sub_43de10(void)
{
    unsigned int v4;  // ftop
    unsigned int v5;  // ftop
    st_43de10_0 *v14;  // eax
    st_43de10_0 *iter;  // esi
    unsigned int v16;  // edi
    unsigned int v17;  // edi
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // edx
    unsigned int *v23;  // ecx
    unsigned int v6;  // ftop
    unsigned int v7;  // esi
    unsigned int v8;  // ftop
    unsigned int v9;  // ftop
    unsigned int v10;  // ftop
    unsigned int v11;  // ftop
    unsigned int v12;  // edi
    unsigned int v13;  // ftop
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v5 = v4 - 1;
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
    v1 = v7;
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
    v10 = v9 - 1;
    if (/* unsupported instruction */)
    {
        v11 = v10 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v11 = v10 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v0 = v12;
    v13 = v11 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    iter = &v14->field_4f0;
    v16 = 13;
    do
    {
        v17 = v16;
        if (!iter->padding_0[20])
            continue;
        v18 = v13 - 1;
        if (/* unsupported instruction */)
        {
            v19 = v18 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v19 = v18 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v20 = v19 - 1;
        if (/* unsupported instruction */)
        {
            v21 = v20 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v21 = v20 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)(&iter->padding_0[0] - 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v22 = *((int *)(&iter->padding_0[0] - 4));
        v23 = *((int *)&iter->padding_0[4]);
        *((unsigned int *)(&iter->padding_0[0] - 8)) = v22;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v13 = v21 + 2;
        if (!((char)((((unsigned short)v13 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)((((unsigned short)v13 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v23)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)(&iter->padding_0[0] - 4)) = v22 + 1;
                *((int *)&iter->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                goto LABEL_43de8f;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v13 -= 1;
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&iter->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)(&iter->padding_0[0] - 4)) = sub_4931e0();
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
LABEL_43de8f:
        if (*((int *)(&iter->padding_0[0] - 4)) > 60)
            iter->padding_0[20] = 0;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    } while ((iter += 64, v16 = v17 - 1, v17 != 1));
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
    return;
}
