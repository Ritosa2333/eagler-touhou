// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f470; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42f470_0 {
    char padding_0[956];
    unsigned int field_3bc;
} st_42f470_0;

extern int g_4ce8cc;
extern unsigned int g_4cea94;
extern st_42f470_0 *g_4ceaac;

unsigned int sub_42f470(void)
{
    int v1;  // esi

    if (g_4cea94)
    {
        sub_45c900(g_4ce8cc, v1);
        g_4ceaac->field_3bc = 0xffffffff;
        sub_45a3c0(g_4ce8cc, v1);
    }
    return 1;
}
