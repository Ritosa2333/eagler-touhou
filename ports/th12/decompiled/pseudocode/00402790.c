// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x402790; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_402790(void)
{
    int v4;  // eax
    int v5;  // edi
    int v6;  // edi
    unsigned int *v7;  // ebx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v5 = v4 - 1;
    if (v4 - 1 < 0)
        return;
    do
    {
        v6 = v5;
        v7(v0, v1, v2);
        v5 = v6 - 1;
    } while (v6 - 1 >= 0);
    return;
}
