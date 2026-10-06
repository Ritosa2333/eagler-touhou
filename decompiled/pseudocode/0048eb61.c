// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48eb61; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_48eb61(unsigned short *a0)
{
    unsigned short *v0;  // eax
    unsigned short *v1;  // eax

    v0 = a0;
    do
    {
        v1 = v0;
        v0 = v1 + 1;
    } while (*(v1));
    return ((int)(v1 + 1 - a0) >> 1) - 1;
}
