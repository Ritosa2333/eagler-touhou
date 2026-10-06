// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491d54; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int * sub_491d54(unsigned int *a0, unsigned int a1)
{
    *(a0) = a1;
    a0[1] = *((int *)(sub_475467() + 152));
    *((unsigned int **)(sub_475467() + 152)) = a0;
    return a0;
}
