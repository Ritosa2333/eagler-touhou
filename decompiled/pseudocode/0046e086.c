// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e086; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46e086_0 {
    char padding_0[4];
    int field_4;
    unsigned int field_8;
} st_46e086_0;

extern char g_49cd28;

void sub_46e086(st_46e086_0 *a0)
{
    unsigned int v1;  // 4098

    v1 = a0->field_8;
    *((char **)&a0->padding_0[0]) = &g_49cd28;
    if (v1)
        sub_46c941(a0->field_4);
    return;
}
