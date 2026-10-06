// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x408030; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_408030_1 {
    int field_0;
    char padding_4[128];
    unsigned int field_84;
    char padding_88[40];
    struct st_408030_0 *field_b0;
} st_408030_1;

typedef struct st_408030_0 {
    char padding_0[112];
    unsigned int field_70;
} st_408030_0;

typedef struct st_408030_2 {
    char padding_0[124];
    unsigned int field_7c;
} st_408030_2;

extern st_408030_2 *g_4b43cc;

void sub_408030(unsigned int a0)
{
    unsigned int v16;  // ebx
    unsigned int v17;  // esi
    st_408030_1 *idx;  // edi
    unsigned int v19;  // ecx
    st_408030_0 *index;  // eax
    st_408030_1 *v0;  // [bp-0x3c]
    unsigned int v1;  // [bp-0x38]
    unsigned int v2;  // [bp-0x34]
    unsigned int v3;  // [bp-0x30]
    unsigned int v4;  // [bp-0x2c]
    unsigned int v5;  // [bp-0x28]
    unsigned int v6;  // [bp-0x24]
    unsigned int v7;  // [bp-0x20]
    unsigned int v8;  // [bp-0x1c]
    unsigned int v9;  // [bp-0x18]
    unsigned int v10;  // [bp-0x14]
    unsigned int v11;  // [bp-0x10]
    unsigned int v12;  // [bp-0xc]
    unsigned int v13;  // [bp-0x8]
    unsigned int v14;  // [bp-0x4]

    v14 = a0;
    v13 = v16;
    v12 = v17;
    if (idx->field_0)
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
        v11 = a0;
        if (/* unsupported instruction */)
        {
            v11 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v11 = nan;
            /* unsupported instruction */
        }
        sub_453e20();
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
        v10 = 1;
        v9 = ~(g_4b43cc->field_7c) & 1;
        v8 = v9;
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
        sub_40caa0();
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
        v7 = 1;
        v6 = ~(g_4b43cc->field_7c) & 1 | 2;
        v5 = v19;
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
        sub_4286f0();
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
        v4 = 60;
        v3 = 11;
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
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
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v1 = nan;
            /* unsupported instruction */
        }
        v0 = idx->padding_4;
        sub_4390f0();
    }
    sub_461970(idx->field_0);
    index = idx->field_b0;
    idx->field_0 = 0;
    idx->field_84 = 0;
    if (index)
        index->field_70 = index->field_70 & 0xfffffffe;
    idx->field_b0 = NULL;
    return;
}
