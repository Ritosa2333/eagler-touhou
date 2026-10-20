// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4390f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b2ed0;

int sub_4390f0(void)
{
    void* v3;  // eax
    void* iter;  // esi
    int v5;  // eax
    int v6;  // edi
    void* index;  // edi
    unsigned int v8;  // edx
    unsigned int v9;  // eax
    unsigned int *idx;  // [bp+0x4]
    unsigned int v1;  // [bp+0x10]
    unsigned int v2;  // [bp+0x14]

    iter = v3 + 35208;
    v5 = 0;
    while ((char)iter[112] & 1)
    {
        iter += 116;
        if (v5 + 1 >= 128)
            return;
    }
    sub_477420(iter, 0, 116, v6);
    *((unsigned int *)&iter[112]) = (int)iter[112] | 3;
    index = iter + 24;
    sub_477420(index, 0, 52);
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
    *((unsigned int *)index) = *(idx);
    *((unsigned int *)&index[4]) = idx[1];
    v8 = idx[2];
    *((int *)iter) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&index[8]) = v8;
    *((int *)&iter[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v9 = (int)iter[92];
    if (!((char)v9 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&iter[84]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&iter[80]) = 0;
        *((unsigned int *)&iter[76]) = 0xfff0bdc1;
        *((char **)&iter[88]) = &g_4b2ed0;
        *((unsigned int *)&iter[92]) = v9 | 1;
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
    *((unsigned int *)&iter[80]) = v1;
    *((unsigned int *)&iter[76]) = v1 - 1;
    if (/* unsupported instruction */)
    {
        *((int *)&iter[84]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned int *)&iter[84]) = nan;
        /* unsupported instruction */
    }
    *((unsigned int *)&iter[96]) = v2;
    *((unsigned int *)&iter[100]) = 0;
    *((unsigned int *)&iter[104]) = 999999;
    *((unsigned int *)&iter[108]) = 4;
    return;
}
