// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45402a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45402a_1 {
    char padding_0[4];
    int field_4;
} st_45402a_1;

typedef struct st_45402a_0 {
    char padding_0[4];
    int field_4;
    unsigned int field_8;
} st_45402a_0;

extern char g_4ceae8;

int sub_45402a(void)
{
    st_45402a_0 *idx;  // ebp
    unsigned int v3;  // ebx
    st_45402a_1 *v4;  // ebp
    unsigned int v1;  // [bp+0xc]

    if (!(g_4ceae8 & 16))
    {
        sub_4538e0(v4->field_4);
        v1 = 1;
    }
    else if (idx->field_8 == v3)
    {
        sub_453c30();
        sub_4538e0(idx->field_4);
        v1 = 1;
    }
    else
    {
        idx->field_8 = idx->field_8 + 1;
    }
}
