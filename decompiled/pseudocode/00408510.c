// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x408510; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_408510_2 {
    char padding_0[124];
    unsigned int field_7c;
} st_408510_2;

typedef struct st_408510_1 {
    char padding_0[1280];
    struct st_408510_0 *field_500;
} st_408510_1;

typedef struct st_408510_0 {
    char padding_0[132];
    unsigned int field_84;
} st_408510_0;

extern st_408510_2 *g_4b43cc;

int sub_408510(void)
{
    unsigned int v10;  // ebx
    unsigned int v11;  // esi
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v12;  // edi
    st_408510_1 *v13;  // eax
    unsigned int v14;  // esi
    unsigned int v15;  // edi
    unsigned int v16;  // ftop
    unsigned int v17;  // edi
    unsigned int v18;  // ftop
    unsigned int v0;  // [bp-0x3c]
    unsigned int v1;  // [bp-0x38]
    unsigned int v2;  // [bp-0x34]
    st_408510_2 *v3;  // [bp-0x30], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x2c]
    unsigned int v5;  // [bp-0x28]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x8]
    unsigned int v8;  // [bp-0x4]

    v8 = v10;
    v7 = v11;
    v6 = v12;
    v14 = &v13->field_500->padding_0[4];
    v15 = 8;
    do
    {
        v17 = v15;
        if (*((int *)(v14 + 128)))
        {
            v18 = v16 - 1;
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
            v5 = 1;
            v4 = ~(g_4b43cc->field_7c) & 1;
            v3 = g_4b43cc;
            if (/* unsupported instruction */)
            {
                v3 = /* unsupported instruction */;
                /* unsupported instruction */
                v20 = v19 + 1;
            }
            else
            {
                v3 = nan;
                /* unsupported instruction */
                v20 = v19 + 1;
            }
            sub_40caa0();
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
            v2 = 1;
            v1 = ~(g_4b43cc->field_7c) & 1 | 2;
            v0 = v1;
            if (/* unsupported instruction */)
            {
                v0 = /* unsupported instruction */;
                /* unsupported instruction */
                v16 = v22 + 1;
            }
            else
            {
                v0 = nan;
                /* unsupported instruction */
                v16 = v22 + 1;
            }
            sub_4286f0();
        }
    } while ((v14 += 180, v15 = v17 - 1, v17 != 1));
    return;
}
