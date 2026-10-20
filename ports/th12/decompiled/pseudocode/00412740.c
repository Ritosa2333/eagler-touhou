// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412740; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412740_0 {
    char padding_0[27960];
    int field_6d38;
    unsigned int field_6d3c;
} st_412740_0;

extern st_412740_0 *g_4b43e4;

unsigned int * sub_412740(int a0, unsigned int a1, unsigned int a2)
{
    int v0;  // edx
    unsigned int v1;  // ecx

    v0 = 99;
    if (a0 <= 99)
        v0 = a0;
    g_4b43e4->field_6d38 = v0;
    v1 = 99;
    if (a0 <= 99)
        v1 = a2;
    g_4b43e4->field_6d3c = v1;
    return g_4b43e4;
}
