// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e869; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e4f1_1 {
    struct st_47e4f1_0 *field_0;
    char field_4;
} st_47e4f1_1;

typedef struct st_47e4f1_0 {
    char padding_0[4];
    char field_4;
} st_47e4f1_0;

st_47e4f1_1 * sub_47e869(st_47e4f1_1 *idx, int a1, char a2)
{
    idx->field_4 = 0;
    *((unsigned int *)&idx->field_4) = *((int *)&idx->field_4) & 0xffff00ff;
    idx->field_0 = 0;
    if (a2)
        sub_47e4f1(idx, a1, &a2, 0x1);
    return idx;
}
