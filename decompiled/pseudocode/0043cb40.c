// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43cb40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_43cb40(void)
{
    void* idx;  // eax

    *((unsigned int *)&idx[4]) = 0;
    *((unsigned int *)&idx[8]) = 0;
    *((unsigned int *)&idx[12]) = 0;
    *((unsigned int *)&idx[20]) = 0;
    *((unsigned int *)&idx[24]) = 0;
    *((unsigned int *)&idx[28]) = 0;
    *((unsigned int *)&idx[32]) = 0;
    *((unsigned int *)idx) = 1915892084;
    *((unsigned short *)&idx[4]) = 4;
    *((unsigned int *)&idx[16]) = 0x100;
    return;
}
