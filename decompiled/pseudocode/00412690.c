// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412690; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412690_0 {
    char padding_0[50180];
    unsigned int field_c404;
    char padding_c408[12];
    char field_c414;
} st_412690_0;

extern st_412690_0 *g_4b4514;

unsigned int sub_412690(void)
{
    if (g_4b4514->field_c404)
    {
        return 1;
    }
    else if (!(g_4b4514->field_c414 & 2))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
