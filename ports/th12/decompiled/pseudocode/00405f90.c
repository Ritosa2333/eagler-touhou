// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x405f90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_405f90(void)
{
    unsigned int v1;  // ecx
    unsigned int *i;  // esi
    unsigned int *iter;  // edi

    for (v1 = 7; v1; i += 1)
    {
        v1 -= 1;
        *(iter) = *(i);
        iter += 1;
    }
    return;
}
