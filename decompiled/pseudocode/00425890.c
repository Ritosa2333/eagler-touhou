// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x425890; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b44f0;

unsigned int * sub_425890(void)
{
    unsigned int *v1;  // esi

    sub_46ce49(v1 + 5, 2520, 2664, sub_4258d0, sub_425900);
    sub_477420(v1, 0, 6713316);
    *(v1) = *(v1) | 2;
    g_4b44f0 = v1;
    return v1;
}
