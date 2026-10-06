// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b44e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_47b44e(void)
{
    void* v2;  // ebp
    enum WIN32_ERROR v0;  // [bp+0x0]

    if (*((int *)((char *)v2 - 32)) == 0xc0000017)
    {
        *((unsigned int *)(*((int *)((char *)v2 - 24)) - 4)) = 8;
        SetLastError(v0);
    }
    *((unsigned int *)((char *)v2 - 28)) = 0;
}
