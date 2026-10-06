// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411060; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b43d8;

unsigned int * sub_411060(void)
{
    unsigned int *ptr;  // eax
    int v0;  // [bp-0x4]

    ptr = sub_46c9ea(40, v0);
    if (ptr)
    {
        memset(ptr, 0, 40);
        *(ptr) = *(ptr) | 2;
        g_4b43d8 = ptr;
    }
    else
    {
        ptr = NULL;
    }
    if (!sub_410c60(ptr))
        return ptr;
    if (!ptr)
        return NULL;
    sub_410ea0();
    sub_46ca4f(ptr);
    return NULL;
}
