// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47de10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47de10_0 {
    char padding_0[4];
    char field_4;
} st_47de10_0;

extern char g_49ddbc;

st_47de10_0 * sub_47de10(st_47de10_0 *a0, unsigned int a1, char a2)
{
    *((char **)&a0->padding_0[0]) = &g_49ddbc;
    a0->field_4 = a2;
    return a0;
}
