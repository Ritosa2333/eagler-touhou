// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da2be; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_4da2be(unsigned int a0, void* a1)
{
    char v0;  // [bp+0x0]

    *((char *)a1 - 1811320391) = *((char *)a1 - 1811320391) + (char)a0;
    if (!a0)
        goto LABEL_0x4da2b0;
    *((char **)((char *)a1 - 0x55)) = *((int *)((char *)a1 - 0x55)) - &v0;
}
