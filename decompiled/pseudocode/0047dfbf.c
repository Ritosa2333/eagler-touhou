// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47dfbf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47dfbf_4 {
    struct st_47dfbf_0 *field_0;
} st_47dfbf_4;

typedef struct st_47dfbf_0 {
    char padding_0[4];
    unsigned int field_4;
} st_47dfbf_0;

typedef struct st_47dfbf_3 {
    char padding_0[4];
    struct st_47dfbf_4 *field_4;
    struct st_47dfbf_4 *field_8;
} st_47dfbf_3;


unsigned int sub_47dfbf(st_47dfbf_3 *a0)
{
    unsigned int v2;  // eax
    unsigned int v0;  // [bp-0x4]

    v2 = a0->field_8->field_0->field_4(v0);
    if ((char)v2)
        return v2;
    goto *((void *)(a0->field_4->field_0->field_4));
}
