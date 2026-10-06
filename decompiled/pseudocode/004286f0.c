// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4286f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4286f0_2 {
    struct st_4286f0_0 *field_0;
    char padding_4[4];
    struct st_4286f0_2 *field_8;
    unsigned int field_c;
} st_4286f0_2;

typedef struct st_4286f0_0 {
    char padding_0[28];
    struct st_4286f0_1 *field_1c;
} st_4286f0_0;

typedef struct st_4286f0_1 {
    unsigned int field_0;
} st_4286f0_1;

typedef struct st_4286f0_3 {
    char padding_0[24];
    struct st_4286f0_2 *field_18;
    char padding_1c[1108];
    unsigned int field_470;
    unsigned int field_474;
    unsigned int field_478;
} st_4286f0_3;

extern st_4286f0_3 *g_4b44f4;

unsigned int sub_4286f0(unsigned int a0, unsigned int a1, unsigned int a2)
{
    st_4286f0_2 *v5;  // ecx
    unsigned int *idx;  // esi
    unsigned int v7;  // ebx
    unsigned int v8;  // edi
    unsigned int v9;  // ftop
    st_4286f0_2 *v10;  // ecx
    unsigned int v11;  // ftop
    unsigned int v12;  // ftop
    unsigned int *v0;  // [bp-0x1c]
    st_4286f0_2 *v1;  // [bp-0x18], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]

    v5 = g_4b44f4->field_18;
    g_4b44f4->field_470 = *(idx);
    g_4b44f4->field_474 = idx[1];
    v7 = 0;
    g_4b44f4->field_478 = idx[2];
    if (!v5)
        return 0;
    v4 = v8;
    do
    {
        v10 = v5;
        if (v10->field_c != 1)
        {
            v11 = v9 - 1;
            if (/* unsupported instruction */)
            {
                v12 = v11 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v12 = v11 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v3 = a2;
            v2 = a1;
            v1 = v10;
            if (/* unsupported instruction */)
            {
                v1 = /* unsupported instruction */;
                /* unsupported instruction */
                v9 = v12 + 1;
            }
            else
            {
                v1 = nan;
                /* unsupported instruction */
                v9 = v12 + 1;
            }
            v0 = idx;
            v7 += v10->field_0->field_1c();
        }
    } while ((v5 = (st_4286f0_2 *)v10->field_8, v10->field_8));
    return v7;
}
