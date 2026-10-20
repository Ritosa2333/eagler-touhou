// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4509c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4cf408;
extern unsigned int g_4cf40c;
extern char g_4cf410;
extern unsigned int g_4cf414;
extern unsigned long long g_4cf448;

char sub_4509c0(void)
{
    unsigned short v3;  // ftop
    unsigned short v4;  // ftop
    unsigned short v11;  // ftop
    unsigned short v5;  // ftop
    unsigned short v6;  // ftop
    unsigned short v7;  // ftop
    unsigned short v8;  // ftop
    unsigned short v9;  // ftop
    unsigned short v10;  // ftop
    long long t;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    if (g_4cf408 || g_4cf40c)
    {
        QueryPerformanceCounter(&t);
        t = (unsigned int)t - *((int *)&g_4cf410);
        v1 = *((unsigned int *)((void*)&t + 4)) - g_4cf414 - ((unsigned int)t < *((int *)&g_4cf410));
        v4 = v3 - 1;
        if (/* unsupported instruction */)
        {
            v5 = v4 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v5 = v4 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v6 = v5 - 1;
        if (/* unsupported instruction */)
        {
            v7 = v6 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v7 = v6 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v8 = v7 + 1;
    }
    else
    {
        t = timeGetTime();
        v9 = v3 - 1;
        if (/* unsupported instruction */)
        {
            v8 = v9 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v8 = v9 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (t < 0)
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
    v10 = v8 - 1;
    if (/* unsupported instruction */)
    {
        v11 = v10 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v11 = v10 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (!((char)(((v11 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        g_4cf448 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return ((char)v11 & 7) * 0;
}
