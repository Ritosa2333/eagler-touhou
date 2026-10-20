// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x468d90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_468d90_1 {
    char padding_0[4];
    struct st_468d90_1 *field_4;
    struct st_468d90_0 *field_8;
    char padding_c[4132];
    struct st_468d90_1 *field_1030;
    struct st_468d90_1 *field_1034;
    struct st_468d90_2 *field_1038;
} st_468d90_1;

typedef struct st_468d90_0 {
    char padding_0[4];
    struct st_468d90_1 *field_4;
} st_468d90_0;

typedef struct st_468d90_2 {
    char padding_0[4];
    struct st_468d90_1 *field_4;
} st_468d90_2;

unsigned int sub_468d90(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    unsigned int v13;  // eax
    st_468d90_1 *index;  // edi
    st_468d90_1 *idx;  // esi
    unsigned int v8;  // ebp
    unsigned int v9;  // ftop
    unsigned int v10;  // ftop
    unsigned int v11;  // ftop
    st_468d90_1 *v12;  // ebx
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    v3 = a0;
    v2 = v4;
    v1 = v5;
    idx = &index->field_1030;
    v8 = 1;
    if (idx)
    {
        do
        {
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
            v12 = idx->field_4;
            v0 = a0;
            if (/* unsupported instruction */)
            {
                v0 = /* unsupported instruction */;
                /* unsupported instruction */
                v9 = v11 + 1;
            }
            else
            {
                v0 = nan;
                /* unsupported instruction */
                v9 = v11 + 1;
            }
            *((char [4])&index->field_4) = idx->padding_0;
            v13 = sub_467170();
            if (v8)
            {
                if (!v13)
                    v8 = 0;
                else
                    return 0xffffffff;
            }
            else
            {
                if (v13)
                {
                    sub_46ca4f(index->field_4);
                    if (idx->field_4)
                        idx->field_4->field_8 = idx->field_8;
                    if (idx->field_8)
                        idx->field_8->field_4 = idx->field_4;
                    idx->field_4 = NULL;
                    idx->field_8 = NULL;
                    sub_46ca4f(idx);
                }
            }
            idx = v12;
        } while (idx);
    }
    index->field_4 = &index->field_8;
    return 0;
}
