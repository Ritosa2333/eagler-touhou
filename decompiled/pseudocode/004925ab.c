// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4925ab; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_4925ab(unsigned int a0)
{
    void* v0;  // ebp

    sub_491e8c(4);
    if (!*((int *)(sub_475467() + 148)))
    {
        *((unsigned int *)((char *)v0 - 4)) = 0;
        sub_472b81(); /* do not return */
    }
    sub_472b94(); /* do not return */
}
