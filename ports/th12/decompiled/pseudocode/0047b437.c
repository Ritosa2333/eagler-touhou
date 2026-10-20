// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b437; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_47b437(void)
{
    void* v1;  // ebp
    unsigned int v2;  // eax

    v2 = *((int *)*((int *)*((int *)((char *)v1 - 20))));
    *((unsigned int *)((char *)v1 - 32)) = v2;
    return v2 == 0xc0000017;
}
