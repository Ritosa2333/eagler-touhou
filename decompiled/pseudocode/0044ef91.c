// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ef91; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_44ef91(void)
{
    void* idx;  // ebp
    unsigned int v3;  // eax
    char v0;  // [bp+0x0]

    v3 = (int)idx[8];
    *((unsigned int *)((char *)idx - 24)) = v3;
    *((char **)((char *)idx - 16)) = &v0;
    *((char *)idx - 4) = 2;
    *((unsigned int *)&idx[8]) = sub_44f0a0(v3 + 1);
    *((unsigned int *)((char *)idx - 4)) = 1;
    return sub_44efb8;
}
