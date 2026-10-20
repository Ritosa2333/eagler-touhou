// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x459400; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_459400_0 {
    char padding_0[8];
    unsigned int field_8;
} st_459400_0;

int sub_459400(void)
{
    st_459400_0 *idx;  // eax
    unsigned int *idx1;  // edx
    unsigned int *index;  // ecx
    unsigned int v4;  // edx

    *((unsigned int *)&idx->padding_0[0]) = *(idx1) - *(index);
    v4 = idx1[2] - index[2];
    *((unsigned int *)&idx->padding_0[4]) = idx1[1] - index[1];
    idx->field_8 = v4;
    return;
}
