// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492530; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492530_2 {
    char padding_0[12];
    struct st_492530_1 *field_c;
} st_492530_2;

typedef struct st_492530_3 {
    char padding_0[28];
    struct st_492530_2 *field_1c;
} st_492530_3;

typedef struct st_492530_0 {
    int field_0;
    unsigned int field_4;
} st_492530_0;

typedef struct st_492530_1 {
    int field_0;
    int field_4;
    char field_8;
} st_492530_1;


char sub_492530(st_492530_3 *idx)
{
    st_492530_0 *v5;  // edi
    unsigned int v6;  // ebx
    unsigned int v7;  // esi
    int v8;  // ebx
    unsigned int v9;  // esi
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    int v3;  // [bp-0xc]
    char v4;  // [bp-0x5]

    if (!v5)
        sub_472b94(); /* do not return */
    v3 = 0;
    v4 = 0;
    if (v5->field_0 <= NULL)
        return v4;
    v1 = v6;
    v0 = v7;
    do
    {
        v8 = idx->field_1c->field_c->field_0;
        v9 = &idx->field_1c->field_c->field_4;
        if (idx->field_1c->field_c->field_0 > NULL)
        {
            v2 = v3 * 16;
            do
            {
                if (sub_491fb3(v5->field_4 + v2, *((int *)v9), idx->field_1c))
                {
                    v4 = 1;
                    break;
                }
            } while ((v8 = (int)(v8 - 1), v9 += 4, v8 > 0));
        }
    } while ((v3 = (int)(v3 + 1), v3 < v5->field_0));
    return v4;
}
