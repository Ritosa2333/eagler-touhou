// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b446; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48b446(void)
{
    void* v1;  // ebp

    if (*((int *)*((int *)*((int *)((char *)v1 - 20)))) == 0xc0000005)
    {
        return 1;
    }
    else if (*((int *)*((int *)*((int *)((char *)v1 - 20)))) != 0xc000001d)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
