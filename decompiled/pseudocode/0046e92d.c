// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e92d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad140[4];
extern unsigned int g_4ad144[4];

unsigned int * sub_46e92d(unsigned int *a0)
{
    unsigned int v1;  // ecx
    unsigned int *v0;  // [bp-0x8]

    v1 = 0;
    do
    {
        if (a0 == g_4ad140[2 * v1])
            return g_4ad144[2 * v1];
    } while ((v1 += 1, v1 < 45));
    if ((char *)a0 - 19 > 0x11)
    {
        v0 = 14;
        return (-(v0 < (char *)a0 - 188) & v0) + 8;
    }
    v0 = 13;
    return v0;
}
