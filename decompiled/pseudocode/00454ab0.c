// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454ab0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_454ab0(void)
{
    unsigned int v1;  // edi
    unsigned int v2;  // esi
    int v3;  // eax
    int v4;  // eax

    v3 = *((int *)(v1 + v2 * 4 + 6336));
    if (!v3)
        return v3;
    v4 = sub_46c941(v3);
    *((unsigned int *)(v1 + v2 * 4 + 6336)) = 0;
    return v4;
}
