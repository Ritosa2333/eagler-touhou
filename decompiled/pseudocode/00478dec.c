// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x478dec; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_478dec(unsigned int a0)
{
    unsigned int *v2;  // esi
    unsigned int v3;  // ebx
    int v0;  // [bp-0x8]
    char v1;  // [bp+0x0]

    while (1)
    {
        *(v2) = *(v2) + 1;
        v3 = sub_478dc3(v0, &v1);
        if (v3 == 0xffffffff)
        {
            return v3;
        }
        else if (!sub_48bb76((char)v3))
        {
            return v3;
        }
    }
}
