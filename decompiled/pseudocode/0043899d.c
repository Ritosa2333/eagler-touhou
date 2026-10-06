// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43899d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43899d_0 {
    char padding_0[16];
    int field_10;
    char padding_14[2420];
    unsigned int field_988;
    unsigned int field_98c;
    char padding_990[156];
    unsigned int field_a2c;
    char padding_a30[47976];
    unsigned int field_c598;
} st_43899d_0;

int sub_43899d(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, char a7, unsigned int a8, unsigned int a9, unsigned int a10, unsigned int a11, unsigned int a12, unsigned int a13, unsigned int a14, unsigned int a15, unsigned int a16, unsigned int a17, unsigned int a18)
{
    st_43899d_0 *idx;  // ebp
    unsigned int v1;  // ebx
    unsigned int v2;  // edi
    void* v3;  // esi
    void* v4;  // edx
    unsigned int v5;  // eax
    unsigned int v6;  // ecx

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
    a4 = idx->field_a2c + (*((int *)(a18 + v1 * 4 - 4)) + v2) * 8 + 40;
    *((unsigned int *)((char *)v3 - 12)) = sub_4931e0();
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
    *((unsigned int *)((char *)v3 - 8)) = sub_4931e0();
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
    a4 = idx->field_a2c + (*((int *)(a18 + v1 * 4 - 4)) + v2) * 8 + 120;
    *((unsigned int *)((char *)v3 - 4)) = sub_4931e0();
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
    *((unsigned int *)v3) = sub_4931e0();
    v4 = v3 - 4;
    if (!idx->field_c598)
        v4 = v3 - 12;
    v5 = idx->field_988 + *((int *)v4);
    v6 = idx->field_98c + (int)v4[4];
    *((unsigned int *)((char *)v3 - 28)) = v5;
    *((unsigned int *)((char *)v3 - 24)) = v6;
    *((unsigned int *)((char *)v3 - 20)) = v5;
    *((unsigned int *)((char *)v3 - 16)) = v6;
    sub_4615a0(idx->field_10, &/* unsupported instruction */, 11, 0);
    *((unsigned int *)&v3[64]) = a3;
}
