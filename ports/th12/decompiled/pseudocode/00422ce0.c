// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422ce0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_422ce0_0 {
    char padding_0[88];
    int field_58;
} st_422ce0_0;

extern int g_4b0c98;
extern int g_4b0c9c;
extern int g_4b43e4;

int sub_422ce0(void)
{
    st_422ce0_0 *idx;  // eax
    int v2;  // esi

    idx->field_58 = idx->field_58 + 1;
    if (idx->field_58 > 8)
    {
        idx->field_58 = 8;
    }
    else
    {
        sub_453d90(v2);
        sub_420f90(0);
    }
    sub_41ce60(g_4b43e4, g_4b0c98, g_4b0c9c);
    return;
}
