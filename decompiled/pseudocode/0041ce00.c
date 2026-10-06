// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41ce00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41ce00_0 {
    char padding_0[27876];
    unsigned int field_6ce4;
} st_41ce00_0;

extern st_41ce00_0 *g_4b43e4;

int sub_41ce00(void)
{
    return g_4b43e4->field_6ce4;
}
