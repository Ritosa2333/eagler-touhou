// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491fb3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_491fb3_2 {
    char field_0;
    char padding_1[3];
    unsigned int field_4;
} st_491fb3_2;

typedef struct st_491fb3_1 {
    char field_0;
    char padding_1[3];
    struct st_491fb3_0 *field_4;
} st_491fb3_1;

typedef struct st_491fb3_0 {
    char padding_0[8];
    char field_8;
} st_491fb3_0;

unsigned int sub_491fb3(st_491fb3_1 *a0, st_491fb3_2 *a1, unsigned int *a2)
{
    unsigned int v1;  // edi
    st_491fb3_0 *v2;  // eax
    unsigned int v3;  // edx
    unsigned int v0;  // [bp-0xc]

    v0 = v1;
    v2 = a0->field_4;
    if (v2)
    {
        v3 = &v2->field_8;
        if (*((char *)v3) && (v2 != a1->field_4 && sub_472ac0(v3, a1->field_4 + 8) || a1->field_0 & 2 && !(a0->field_0 & 8) || (char)*(a2) & 1 && !(a0->field_0 & 1) || (char)*(a2) & 2 && !(a0->field_0 & 2)))
            return 0;
    }
    return 1;
}
