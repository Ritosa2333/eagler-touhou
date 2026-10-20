// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47de46; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47de46_0 {
    char padding_0[4];
    char field_4;
} st_47de46_0;

typedef struct st_47de46_2 {
    char padding_0[4];
    struct st_47de46_0 *field_4;
} st_47de46_2;

extern char g_49ddc8;

st_47de46_2 * sub_47de46(st_47de46_2 *a0, unsigned int a1, st_47de46_0 *a2)
{
    st_47de46_0 *v0;  // ecx

    v0 = a2;
    *((char **)&a0->padding_0[0]) = &g_49ddc8;
    if (v0 && (v0->field_4 == 2 || v0->field_4 == 3))
        v0 = NULL;
    a0->field_4 = v0;
    return a0;
}
