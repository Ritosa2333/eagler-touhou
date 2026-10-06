// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493064; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_493064_2 {
    unsigned int field_0;
    char field_4;
    char padding_5[11];
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[4];
    struct st_493064_0 *field_1c;
} st_493064_2;

typedef struct st_493064_3 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[4];
    unsigned int field_c;
    char padding_10[12];
    unsigned int field_1c;
    char field_20;
} st_493064_3;

typedef struct st_493064_1 {
    unsigned int field_0;
} st_493064_1;

typedef struct st_493064_0 {
    char padding_0[8];
    struct st_493064_1 *field_8;
} st_493064_0;


unsigned int sub_493064(st_493064_2 *a0, int a1, int a2, int a3, st_493064_3 *a4, int a5, int a6, int a7)
{
    unsigned int v1;  // edi
    unsigned int v0;  // [bp-0x10]

    v0 = v1;
    if (*((int *)(sub_475467() + 524)) || a0->field_0 == 3765269347 || a0->field_0 == 0x80000026 || (a4->field_0 & 0x1fffffff) < 429065506 || !(a4->field_20 & 1))
    {
        if (a0->field_4 & 0x66)
        {
            if (a4->field_4 && !a5)
                sub_49205b(a1, a3, a4, -0x1);
        }
        else
        {
            if (a4->field_c || (a4->field_0 & 0x1fffffff) >= 429065505 && a4->field_1c)
            {
                if (a0->field_0 == 3765269347 && a0->field_10 >= 3 && a0->field_14 > 429065506 && a0->field_1c->field_8)
                    return a0->field_1c->field_8(a0, a1, a2, a3, a4, a5, a6, (char)a7);
                sub_492d00(a0, a1, a2, a3, a4, a7, a5, a6);
            }
        }
    }
    return 1;
}
