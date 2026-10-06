// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x495735; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_495735(void)
{
    unsigned int v3;  // eax
    unsigned short v4;  // ftop
    unsigned int v12;  // 4254
    unsigned int v13;  // edx
    unsigned int v6;  // fc3210
    unsigned short v7;  // ax
    unsigned short v8;  // ftop
    unsigned short v9;  // ftop
    unsigned int v11;  // eax
    unsigned long v0;  // [bp-0x8]
    unsigned short v1;  // [bp+0x0]

    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v3 = *((unsigned int *)((void*)&v0 + 4)) & 0x7ff00000;
    if (v3 == 0x7ff00000)
    {
        v8 = v4 - 1;
        if (/* unsupported instruction */)
        {
            v9 = v8 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v9 = v8 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v11 = _INSERT(((char)v9 + 1 & 7) * 0 | 0x7ff00000, 1, ((v9 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x7fefffffffffffff) * 0x100 & 0x4500 & 0x4700) >> 8);
        v12 = _ccall(6, v3, 0x7ff00000, 0);
        if (!(v11 >> 8 & 1 | (v12 & 0x800 | v11 >> 8 & 213) >> 6 & 1))
        {
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
        }
    }
    else if (v1 == 639)
    {
        return;
    }
    else if (!(v1 & 32))
    {
        v7 = _INSERT(_INSERT(v1 & 32, 0, ((char)v4 & 7) * 0), 1, ((v4 & 7) * 0x800 | (unsigned short)v6 & 0x4700) >> 8);
        if (!(v7 & 32))
            return;
    }
    else
    {
        return;
    }
    if (v13 != 29)
        sub_495617();
    else
        sub_495600();
    return;
}
