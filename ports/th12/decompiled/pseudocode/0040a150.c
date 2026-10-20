// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40a150; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40a150_0 {
    char padding_0[52];
    unsigned int field_34;
    char padding_38[1012];
    unsigned int field_42c;
    unsigned int field_430;
    unsigned int field_434;
    char padding_438[76];
    unsigned int field_484;
    char padding_488[176];
    struct st_40a150_0 *field_538;
} st_40a150_0;

extern int g_4ce8cc;

int sub_40a150(void)
{
    unsigned int v4;  // esi
    unsigned int v5;  // eax
    unsigned int v13;  // ftop
    unsigned int v14;  // ftop
    unsigned int v15;  // ftop
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v6;  // ecx
    unsigned int v23;  // ftop
    st_40a150_0 *v7;  // esi
    unsigned int v8;  // ftop
    st_40a150_0 *idx;  // esi
    unsigned int v10;  // ftop
    unsigned int v11;  // ftop
    unsigned int v12;  // ftop
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x4]

    v2 = v4;
    v7 = *((int *)(v5 + v6 * 4 + 20));
    if (!v7)
        return;
    do
    {
        idx = v7;
        v10 = v8 - 1;
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
            idx->field_42c = /* unsupported instruction */;
            /* unsupported instruction */
            v12 = v11 + 1;
        }
        else
        {
            idx->field_42c = nan;
            /* unsupported instruction */
            v12 = v11 + 1;
        }
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
            idx->field_430 = /* unsupported instruction */;
            /* unsupported instruction */
            v15 = v14 + 1;
        }
        else
        {
            idx->field_430 = nan;
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
            idx->field_434 = /* unsupported instruction */;
            /* unsupported instruction */
            v8 = v17 + 1;
        }
        else
        {
            idx->field_434 = nan;
            /* unsupported instruction */
            v8 = v17 + 1;
        }
        if (idx->field_484 & 0x20000000)
        {
            v18 = v8 - 1;
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
            if (/* unsupported instruction */)
            {
                v1 = /* unsupported instruction */;
                /* unsupported instruction */
                v20 = v19 + 1;
            }
            else
            {
                v1 = nan;
                /* unsupported instruction */
                v20 = v19 + 1;
            }
            v21 = v20 - 1;
            if (/* unsupported instruction */)
            {
                v22 = v21 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v22 = v21 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v0 = /* unsupported instruction */;
                /* unsupported instruction */
                v23 = v22 + 1;
            }
            else
            {
                v0 = nan;
                /* unsupported instruction */
                v23 = v22 + 1;
            }
            sub_464640();
            idx->field_484 = idx->field_484 | 4;
            if (/* unsupported instruction */)
            {
                idx->field_34 = /* unsupported instruction */;
                /* unsupported instruction */
                v8 = v23 + 1;
            }
            else
            {
                idx->field_34 = nan;
                /* unsupported instruction */
                v8 = v23 + 1;
            }
        }
        sub_45c900(g_4ce8cc);
        v7 = idx->field_538;
    } while (idx->field_538);
    return;
}
