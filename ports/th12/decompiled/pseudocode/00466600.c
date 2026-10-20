// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466600; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_466600(unsigned int a0)
{
    unsigned int v2;  // ebx
    unsigned int *v3;  // edi
    unsigned int v4;  // esi
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    v2 = 0;
    if (!v3[1])
        return 0;
    v4 = 0;
    if (v3[4] <= 0)
        return 0;
    do
    {
        if (*((int *)(v3[1] + v4 * 4)))
        {
            v0 = 0;
            (*((int *)(*((int *)*((int *)(v3[1] + v4 * 4))) + 36)))(*((int *)(v3[1] + v4 * 4)), &v0);
            v2 |= v0 & 1;
        }
    } while ((v4 += 1, v4 < v3[4]));
    return v2;
}
