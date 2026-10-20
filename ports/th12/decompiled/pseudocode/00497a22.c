// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x497a22; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_497a22_0 {
    char padding_0[20];
    char field_14;
} st_497a22_0;

extern int g_4cf2b0;

void sub_497a22(char a0, unsigned int a1)
{
    st_497a22_0 *idx;  // edx
    char v2;  // ch

    idx = _INSERT(a1, 1, 0x44);
    idx->field_14 = idx->field_14 + v2;
    sub_46cf1e(&g_4cf2b0, 16);
    return;
}
