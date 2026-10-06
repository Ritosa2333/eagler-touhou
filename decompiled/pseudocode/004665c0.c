// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4665c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_4665c0(void)
{
    unsigned int *v1;  // edi
    unsigned int v2;  // ebx
    unsigned int i;  // esi
    unsigned int v4;  // esi

    if (!v1[1])
        return 2147746288;
    v2 = 0;
    i = 0;
    if (v1[4] <= 0)
        return 0;
    do
    {
        v4 = i + 1;
        v2 |= (*((int *)(*((int *)*((int *)(v1[1] + i * 4))) + 52)))(*((int *)(v1[1] + i * 4)), 0);
        i = v4;
    } while (i < v1[4]);
    return v2;
}
