// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493f8a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4110;

unsigned short sub_493f8a(void)
{
    void* v6;  // ebp
    unsigned short v13;  // ftop
    unsigned int v15;  // eax
    unsigned int v16;  // 4254
    unsigned short v17;  // ftop
    unsigned short v18;  // ftop
    unsigned short v19;  // ftop
    unsigned short v21;  // ftop
    unsigned int v23;  // eax
    unsigned int v24;  // 4254
    unsigned short v25;  // ax
    unsigned int v27;  // fc3210
    unsigned short v28;  // ax
    unsigned int v29;  // esi
    unsigned int v30;  // edi
    char v7;  // al
    unsigned int v31;  // ebx
    void* v32;  // esi
    void* v33;  // edi
    void* v34;  // esi
    void* v35;  // edi
    unsigned short v36;  // ax
    unsigned short v37;  // ax
    unsigned short v8;  // ax
    unsigned int v9;  // cc_dep1
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned int v0;  // [bp-0x14]
    void* v1;  // [bp-0x10]
    void* v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    if (g_4b4110)
        return v37;
    if (/* unsupported instruction */)
        *((long long *)((char *)v6 - 720)) = /* unsupported instruction */;
    else
        *((unsigned long long *)((char *)v6 - 720)) = nan;
    v7 = *((char *)v6 - 144);
    if (!*((char *)v6 - 144))
    {
LABEL_493fbd:
        v25 = *((short *)((char *)v6 - 164)) & 32;
        if (*((short *)((char *)v6 - 164)) & 32)
            return v25;
        v28 = _INSERT(_INSERT(v25, 0, ((char)v17 & 7) * 0), 1, ((v17 & 7) * 0x800 | (unsigned short)v27 & 0x4700) >> 8);
        if (!(v28 & 32))
            return v28 & 32;
        *((unsigned int *)((char *)v6 - 142)) = 8;
    }
    else if (v7 == 0xff)
    {
        v8 = *((short *)((char *)v6 - 714)) & 0x7ff0;
        v9 = v8;
        if (v8 != 0x7ff0)
            goto LABEL_493fbd;
LABEL_494043:
        *((unsigned int *)((char *)v6 - 142)) = 3;
        v18 = v17 - 1;
        if (/* unsupported instruction */)
        {
            v19 = v18 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v19 = v18 - 1;
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
        v21 = v19 + 1;
        v23 = _INSERT(_INSERT(CONCAT(v8, 0), 0, ((char)v21 & 7) * 0), 1, ((v21 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x7fefffffffffffff) * 0x100 & 0x4500 & 0x4700) >> 8);
        v24 = _ccall(5, v9, 0x7ff0, 0);
        if (!(v23 >> 8 & 1 | (v24 & 0x800 | v23 >> 8 & 213) >> 6 & 1))
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
    else if (v7 == 254)
    {
        v8 = *((short *)((char *)v6 - 714)) & 0x7ff0;
        if (v8)
        {
            v9 = v8;
            if (v8 == 0x7ff0)
                goto LABEL_494043;
            goto LABEL_493fbd;
        }
        else
        {
            *((unsigned int *)((char *)v6 - 142)) = 4;
            v10 = v17 - 1;
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
            v13 = v11 + 1;
            v15 = _INSERT(_INSERT(CONCAT(v8, 0), 0, ((char)v13 & 7) * 0), 1, ((v13 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x10000000000000) * 0x100 & 0x4500 & 0x4700) >> 8);
            v16 = _ccall(14, v8, 0, 0);
            if (v15 >> 8 & 1)
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
    }
    else if (v7)
    {
        *((int *)((char *)v6 - 142)) = v7;
    }
    else
    {
        return _INSERT(CONCAT(v7, 0), 0, v7);
    }
    v4 = v29;
    v3 = v30;
    v31 = *((int *)((char *)v6 - 148)) + 1;
    *((unsigned int *)((char *)v6 - 138)) = v31;
    if (!(*((char *)v6 - 712) & 1))
    {
        v32 = v6 + 8;
        v33 = v6 - 134;
        *((int *)v33) = *((int *)v32);
        *((int *)&v33[4]) = (int)v32[4];
        if (*((char *)(v31 + 12)) != 1)
        {
            v34 = v6 + 16;
            v35 = v6 - 126;
            *((int *)v35) = *((int *)v34);
            *((int *)&v35[4]) = (int)v34[4];
        }
    }
    if (/* unsupported instruction */)
    {
        *((long long *)((char *)v6 - 118)) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned long long *)((char *)v6 - 118)) = nan;
        /* unsupported instruction */
    }
    v2 = v6 - 164;
    v1 = v6 - 142;
    v0 = *((char *)(*((int *)((char *)v6 - 148)) + 14));
    v36 = sub_496bb6();
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
    return v36;
}
