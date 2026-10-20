// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412680; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412680_0 {
    char padding_0[35204];
    char field_8984;
} st_412680_0;

extern st_412680_0 *g_4b4514;

st_412680_0 * sub_412680(void)
{
    g_4b4514->field_8984 = 1;
    return g_4b4514;
}
