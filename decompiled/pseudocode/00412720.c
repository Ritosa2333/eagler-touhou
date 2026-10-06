// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412720; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412720_0 {
    char padding_0[140];
    unsigned int field_8c;
} st_412720_0;

typedef struct st_412720_1 {
    char padding_0[27952];
    struct st_412720_0 *field_6d30;
} st_412720_1;

extern st_412720_1 *g_4b43e4;

int sub_412720(void)
{
    return g_4b43e4->field_6d30->field_8c;
}
