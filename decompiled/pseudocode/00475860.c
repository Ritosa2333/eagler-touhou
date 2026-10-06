// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x475860; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void* sub_475860(void* a0)
{
    void* v0;  // ecx
    void* v1;  // ecx
    void* v2;  // ecx
    unsigned int v3;  // eax

    v0 = a0;
    if (!(v0 & 0x3))
    {
        while (1)
        {
LABEL_475890:
            v2 = v0;
            v1 = v2 + 4;
            v0 = v1;
            if (!((*((int *)v2) ^ 0xffffffff ^ 0x7efefeff + *((int *)v2)) & 0x81010100))
                continue;
            v3 = *((int *)((char *)v1 - 4));
            if (!(char)v3)
                return v1 - 4 - a0;
            if (!*((char *)((void*)&v3 + 1)))
                return v1 - 3 - a0;
            if (!(v3 & 0xff0000))
                return v1 - 2 - a0;
            v0 = v1;
            if (!(v3 & 0xff000000))
                break;
        }
    }
    while (1)
    {
        v1 = v0 + 1;
        if (!*((char *)v0))
            break;
        v0 = v1;
        if (!(v0 & 0x3))
        {
            v0 = v1;
            goto LABEL_475890;
        }
    }
    return v1 - 1 - a0;
}
