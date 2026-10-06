// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db470; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4db470(void)
{
    char *v1;  // esi
    unsigned short v2;  // cx
    char *iter;  // edi
    void* v4;  // ebp

    while (1)
    {
        *((char *)v4 - 78) = *((char *)v4 - 78) | *((char *)((void*)&v2 + 1));
        *(iter) = *(v1);
        iter += 1;
        v1 += 1;
        v2 = 53105;
    }
}
