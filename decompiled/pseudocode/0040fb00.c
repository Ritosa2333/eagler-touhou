// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40fb00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b43d4;

unsigned int * sub_40fb00(void)
{
    int v1;  // esi
    unsigned int *ptr;  // eax

    ptr = sub_46c9ea(36, v1);
    if (ptr)
    {
        memset(ptr, 0, 36);
        *(ptr) = *(ptr) | 2;
        g_4b43d4 = ptr;
    }
    else
    {
        ptr = NULL;
    }
    if (!sub_40f940(ptr))
        return ptr;
    if (!ptr)
        return NULL;
    sub_40fa30();
    sub_46ca4f(ptr);
    return NULL;
}
