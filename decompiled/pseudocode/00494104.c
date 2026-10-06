// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x494104; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_494104(unsigned int a0, unsigned int a1)
{
    unsigned int v0;  // [bp-0xe]
    unsigned int v1;  // [bp-0xa]
    unsigned short v2;  // [bp-0x6]

    if ((*((unsigned short *)((void*)&a1 + 2)) >> 4 & 0x7ff) == 0x7ff)
    {
        v2 = *((unsigned short *)((void*)&a1 + 2)) | 0x7fff;
        v1 = CONCAT(a1, a0) * 0x800 >> 32;
        v0 = a0;
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        return;
    }
    else if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
}
