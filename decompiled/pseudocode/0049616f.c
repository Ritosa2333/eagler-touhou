// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x49616f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_49616f(void* idx, unsigned int *a1, unsigned int a2, unsigned int a3, unsigned int a4, void* a5, unsigned int a6)
{
    char v1;  // cl
    unsigned int v2;  // edi
    void* v11;  // ecx
    unsigned int v12;  // eax
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v16;  // eax
    unsigned int v17;  // eax
    unsigned int v18;  // eax
    char v3;  // al
    unsigned int v4;  // eax
    void* v5;  // eax
    unsigned int v6;  // ecx
    unsigned int v7;  // eax
    void* v8;  // eax
    unsigned int v9;  // ecx
    void* index;  // eax
    unsigned int v0;  // [bp-0x10]

    v1 = a2;
    v0 = v2;
    *((unsigned int *)&idx[4]) = 0;
    *((unsigned int *)&idx[8]) = 0;
    *((unsigned int *)&idx[12]) = 0;
    if ((char)a2 & 16)
    {
        *((unsigned int *)&idx[4]) = (int)idx[4] | 1;
        a2 = 0xc000008f;
    }
    if (v1 & 2)
    {
        *((unsigned int *)&idx[4]) = (int)idx[4] | 2;
        a2 = 0xc0000093;
    }
    if (v1 & 1)
    {
        *((unsigned int *)&idx[4]) = (int)idx[4] | 4;
        a2 = 0xc0000091;
    }
    if (v1 & 4)
    {
        *((unsigned int *)&idx[4]) = (int)idx[4] | 8;
        a2 = 0xc000008e;
    }
    if (v1 & 8)
    {
        *((unsigned int *)&idx[4]) = (int)idx[4] | 16;
        a2 = 0xc0000090;
    }
    *((unsigned int *)&idx[8]) = (int)idx[8] ^ (~(*(a1) * 16) ^ (int)idx[8]) & 16;
    *((unsigned int *)&idx[8]) = (int)idx[8] ^ (~(*(a1) * 2) ^ (int)idx[8]) & 8;
    *((unsigned int *)&idx[8]) = (int)idx[8] ^ (~(*(a1) >> 1) ^ (int)idx[8]) & 4;
    *((unsigned int *)&idx[8]) = (int)idx[8] ^ (~(*(a1) >> 3) ^ (int)idx[8]) & 2;
    *((unsigned int *)&idx[8]) = (int)idx[8] ^ (~(*(a1) >> 5) ^ (int)idx[8]) & 1;
    v3 = sub_491160();
    if (v3 & 1)
        *((unsigned int *)&idx[12]) = (int)idx[12] | 16;
    if (v3 & 4)
        *((unsigned int *)&idx[12]) = (int)idx[12] | 8;
    if (v3 & 8)
        *((unsigned int *)&idx[12]) = (int)idx[12] | 4;
    if (v3 & 16)
        *((unsigned int *)&idx[12]) = (int)idx[12] | 2;
    if (v3 & 32)
        *((unsigned int *)&idx[12]) = (int)idx[12] | 1;
    v4 = *(a1) & 0xc00;
    if (!(*(a1) & 0xc00))
    {
        *((unsigned int *)idx) = *((int *)idx) & 0xfffffffc;
    }
    else if (v4 == 0x400)
    {
        v5 = idx;
        v6 = *((int *)v5) & 0xfffffffd | 1;
        goto LABEL_4962b8;
    }
    else if (v4 == 0x800)
    {
        v5 = idx;
        v6 = *((int *)v5) & 0xfffffffe | 2;
LABEL_4962b8:
        *((unsigned int *)v5) = v6;
    }
    else if (v4 == 0xc00)
    {
        *((unsigned int *)idx) = *((int *)idx) | 3;
    }
    v7 = *(a1) & 0x300;
    if (!(*(a1) & 0x300))
    {
        v8 = idx;
        v9 = *((int *)v8) & 0xffffffeb | 8;
        goto LABEL_496304;
    }
    else if (v7 == 0x200)
    {
        v8 = idx;
        v9 = *((int *)v8) & 0xffffffe7 | 4;
LABEL_496304:
        *((unsigned int *)v8) = v9;
    }
    else if (v7 == 0x300)
    {
        *((unsigned int *)idx) = *((int *)idx) & 0xffffffe3;
    }
    *((unsigned int *)idx) = *((int *)idx) ^ (a3 * 32 ^ *((int *)idx)) & 0x1ffe0;
    *((unsigned int *)&idx[32]) = (int)idx[32] | 1;
    index = idx;
    if (a6)
    {
        *((unsigned int *)&index[32]) = (int)index[32] & 0xffffffe1;
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx[16]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&idx[96]) = (int)idx[96] | 1;
        *((unsigned int *)&idx[96]) = (int)idx[96] & 0xffffffe1;
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx[80]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    else
    {
        *((unsigned int *)&index[32]) = (int)index[32] & 0xffffffe3 | 2;
        /* unsupported instruction */
        /* unsupported instruction */
        *((long long *)&idx[16]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&idx[96]) = (int)idx[96] | 1;
        *((unsigned int *)&idx[96]) = (int)idx[96] & 0xffffffe3 | 2;
        /* unsupported instruction */
        /* unsupported instruction */
        *((long long *)&idx[80]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    sub_491170();
    RaiseException(a2, 0, 1, &idx);
    v11 = idx;
    if ((char)v11[8] & 16)
        *(a1) = *(a1) & 0xfffffffe;
    if ((char)v11[8] & 8)
        *(a1) = *(a1) & 0xfffffffb;
    if ((char)v11[8] & 4)
        *(a1) = *(a1) & 0xfffffff7;
    if ((char)v11[8] & 2)
        *(a1) = *(a1) & 0xffffffef;
    if ((char)v11[8] & 1)
        *(a1) = *(a1) & 0xffffffdf;
    v12 = *((int *)v11) & 3;
    v13 = v12 - 0;
    if (v12)
    {
        v14 = v13 - 1;
        if (v13 == 1)
        {
            v15 = *(a1) & 0xfffff7ff | 0x400;
            goto LABEL_4963f5;
        }
        else if (v14 == 1)
        {
            v15 = *(a1) & 0xfffffbff | 0x800;
LABEL_4963f5:
            *(a1) = v15;
        }
        else if (v14 == 2)
        {
            *(a1) = *(a1) | 0xc00;
        }
    }
    else
    {
        *(a1) = *(a1) & 0xfffff3ff;
    }
    v16 = (unsigned int)(*((int *)v11)) >> 2 & 7;
    v17 = v16 - 0;
    if (!v16)
    {
        v18 = *(a1) & 0xfffff3ff | 0x300;
        goto LABEL_496433;
    }
    else if (v17 == 1)
    {
        v18 = *(a1) & 0xfffff3ff | 0x200;
LABEL_496433:
        *(a1) = v18;
    }
    else if (v17 == 2)
    {
        *(a1) = *(a1) & 0xfffff3ff;
    }
    if (a6)
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
        *((int *)a5) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        return;
    }
    else
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
        *((long long *)a5) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        return;
    }
}
