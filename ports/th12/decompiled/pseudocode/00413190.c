// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x413190; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_413190_0 {
    char padding_0[20];
    struct st_413190_1 *field_14;
} st_413190_0;

typedef struct st_413190_1 {
    unsigned int field_0;
} st_413190_1;

typedef struct st_413190_3 {
    struct st_413190_2 *field_0;
    struct st_413190_3 *field_4;
} st_413190_3;

typedef struct st_413190_2 {
    struct st_413190_0 *field_0;
    char padding_4[4156];
    char field_1040;
    char padding_1041[5815];
    unsigned int field_26f8;
    char field_26fb;
} st_413190_2;

typedef struct st_413190_4 {
    char padding_0[104];
    struct st_413190_3 *field_68;
} st_413190_4;

unsigned int sub_413190(st_413190_4 *a0)
{
    unsigned int v3;  // esi
    st_413190_3 *v4;  // esi
    unsigned int v5;  // edi
    st_413190_3 *v6;  // edi
    int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v2 = v3;
    v4 = a0->field_68;
    if (v4)
    {
        v1 = v5;
        do
        {
            v6 = v4->field_4;
            if (!(*((char *)&v4->field_0->field_26f8 + 3) & 1) && !sub_413840(&v4->field_0->field_1040))
            {
                v4->field_0->field_26f8 = v4->field_0->field_26f8 & 0xfffdffff;
            }
            else if (v4->field_0)
            {
                v4->field_0->field_0->field_14(1);
            }
        } while ((v4 = v6, v4));
    }
    sub_464a80(v0);
    return 1;
}
