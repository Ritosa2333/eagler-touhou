// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x428780; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_428780_2 {
    struct st_428780_0 *field_0;
    char padding_4[4];
    struct st_428780_2 *field_8;
    unsigned int field_c;
} st_428780_2;

typedef struct st_428780_3 {
    char padding_0[24];
    struct st_428780_2 *field_18;
} st_428780_3;

typedef struct st_428780_0 {
    char padding_0[36];
    struct st_428780_1 *field_24;
} st_428780_0;

typedef struct st_428780_1 {
    unsigned int field_0;
} st_428780_1;

int sub_428780(void)
{
    st_428780_3 *v4;  // eax
    st_428780_2 *v5;  // ecx
    unsigned int v6;  // esi
    unsigned int v7;  // ftop
    st_428780_2 *v8;  // ecx
    st_428780_2 *v9;  // esi
    unsigned int v10;  // ftop
    unsigned int v11;  // ftop
    unsigned int v12;  // ebx
    unsigned int v0;  // [bp-0x10]
    st_428780_2 *v1;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x8]

    v5 = v4->field_18;
    if (!v4->field_18)
        return;
    v2 = v6;
    do
    {
        v8 = v5;
        v9 = v8->field_8;
        if (v8->field_c != 1)
        {
            v10 = v7 - 1;
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
            v1 = v8;
            if (/* unsupported instruction */)
            {
                v1 = /* unsupported instruction */;
                /* unsupported instruction */
                v7 = v11 + 1;
            }
            else
            {
                v1 = nan;
                /* unsupported instruction */
                v7 = v11 + 1;
            }
            v0 = v12;
            v8->field_0->field_24();
        }
    } while ((v5 = v9, v8->field_8));
    return;
}
