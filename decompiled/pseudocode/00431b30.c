// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x431b30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_431b30_0 {
    char padding_0[27868];
    int field_6cdc;
} st_431b30_0;

extern unsigned int g_4b0c40;
extern int g_4b0c44;
extern st_431b30_0 *g_4b43e4;

void sub_431b30(void)
{
    g_4b43e4->field_6cdc = g_4b0c44;
    if (g_4b0c40 < g_4b0c44)
        g_4b0c40 = g_4b0c44;
    return;
}
