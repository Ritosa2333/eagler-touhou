// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x438a79; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_438a79(void)
{
    void* v9;  // esi
    unsigned int v10;  // eax
    unsigned int v11;  // ebx
    unsigned int v12;  // edi
    void* v13;  // edx
    unsigned int v14;  // ecx
    unsigned int v15;  // eax
    int v1;  // [bp+0x10]
    unsigned int v2;  // [bp+0x14]
    char v3;  // [bp+0x24]
    unsigned int v4;  // [bp+0x4c]
    unsigned int v5;  // [bp+0x988]
    unsigned int v6;  // [bp+0x98c]
    unsigned int v7;  // [bp+0xa2c]
    unsigned int v8;  // [bp+0xc598]

    *((unsigned int *)((char *)v9 - 12)) = v10;
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
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)((char *)v9 - 8)) = sub_4931e0();
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
    /* unsupported instruction */
    /* unsupported instruction */
    v2 = v7 + (*((int *)(v4 + v11 * 4 - 4)) + v12) * 8 + 120;
    *((unsigned int *)((char *)v9 - 4)) = sub_4931e0();
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
    *((unsigned int *)v9) = sub_4931e0();
    v13 = v9 - 4;
    if (!v8)
        v13 = v9 - 12;
    v14 = v6 + (int)v13[4];
    v15 = *((int *)v13) + v5;
    *((unsigned int *)((char *)v9 - 24)) = v14;
    *((unsigned int *)((char *)v9 - 28)) = v15;
    *((unsigned int *)((char *)v9 - 16)) = v14;
    *((unsigned int *)((char *)v9 - 20)) = v15;
    sub_4615a0(v1, &v3, 12, 0);
    *((unsigned int *)&v9[64]) = v2;
}
