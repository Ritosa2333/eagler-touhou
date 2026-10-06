// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46cee3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_46cee3(void)
{
    void* v1;  // ebp

    *((int *)((char *)v1 - 28)) = *((int *)((char *)v1 - 20));
    *((int *)((char *)v1 - 32)) = *((int *)*((int *)((char *)v1 - 28)));
    if (*((int *)*((int *)((char *)v1 - 32))) == 3765269347)
        sub_472b48(); /* do not return */
    *((unsigned int *)((char *)v1 - 36)) = 0;
    return *((int *)((char *)v1 - 36));
}
