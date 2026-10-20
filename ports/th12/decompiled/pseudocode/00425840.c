// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x425840; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_425840_0 {
    char padding_0[2604];
    unsigned int field_a2c;
} st_425840_0;

extern st_425840_0 *g_4b4514;

int sub_425840(void)
{
    return g_4b4514->field_a2c;
}
