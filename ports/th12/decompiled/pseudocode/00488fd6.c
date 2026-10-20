// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x488fd6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int * sub_488fd6(unsigned int a0, unsigned int *a1)
{
    unsigned int *v1;  // eax
    unsigned int *v2;  // eax
    unsigned int i;  // edx
    unsigned int *v4;  // eax
    unsigned int v0;  // [bp-0x8]

    v1 = a1;
    v0 = 3;
    v2 = v1;
    do
    {
        i = v0;
        v4 = v2;
        *((unsigned int *)(a0 - (char *)v1 + v4)) = *(v4);
        v2 = v4 + 1;
        v0 = i - 1;
    } while (i != 1);
    return v4 + 1;
}
