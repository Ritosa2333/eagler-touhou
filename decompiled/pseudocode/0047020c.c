// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47020c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_47020c(void* *a0)
{
    *(a0) = *(a0) + 4;
    return _INSERT(*(a0), 0, *((short *)((char *)*(a0) - 4)));
}
