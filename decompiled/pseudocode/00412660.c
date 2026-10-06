// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412660; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412660_0 {
    char padding_0[35200];
    unsigned int field_8980;
    char field_8984;
} st_412660_0;

extern st_412660_0 *g_4b4514;

void sub_412660(unsigned int a0)
{
    if (!g_4b4514->field_8984)
        g_4b4514->field_8980 = a0;
    return;
}
