// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421800; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421800_0 {
    char padding_0[10];
    char field_a;
} st_421800_0;

typedef struct st_421800_1 {
    char padding_0[28];
    struct st_421800_0 *field_1c;
} st_421800_1;

extern st_421800_1 *g_4b4518;

int sub_421800(void)
{
    return g_4b4518->field_1c->field_a & 1;
}
