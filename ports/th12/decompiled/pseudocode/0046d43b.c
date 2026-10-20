// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d43b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d43b_2 {
    char padding_0[8];
    struct st_46d43b_0 *field_8;
    char field_c;
} st_46d43b_2;

typedef struct st_46d43b_0 {
    char padding_0[112];
    unsigned int field_70;
} st_46d43b_0;


st_46d43b_0 * sub_46d43b(st_46d43b_2 *a0)
{
    st_46d43b_0 *idx;  // eax

    if (a0->field_c)
    {
        idx = a0->field_8;
        idx->field_70 = idx->field_70 & 0xfffffffd;
    }
    return idx;
}
