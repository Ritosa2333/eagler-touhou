// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45df30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45df30_0 {
    char padding_0[1144];
    int field_478;
} st_45df30_0;

extern int g_4ce8cc;

unsigned int sub_45df30(st_45df30_0 *a0)
{
    sub_45c800(g_4ce8cc, a0->field_478, 33);
    return 0;
}
