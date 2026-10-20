// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d44c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_46d44c_0 {
    char padding_0[112];
    unsigned int field_70;
} st_46d44c_0;

int sub_46d44c(void)
{
    unsigned int v6;  // edx
    char *v7;  // esi
    char v0;  // [bp-0x2c]
    st_46d3b4_0 v1;  // [bp-0x14]
    st_46d44c_0 *idx;  // [bp-0xc]
    char v3;  // [bp-0x8]
    char *v4;  // [bp+0x4]
    unsigned int *v5;  // [bp+0x8]

    sub_46d3b4(&v1, v6, v5);
    v7 = v4;
    if (!v7)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        if (v3)
            idx->field_70 = idx->field_70 & 0xfffffffd;
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
    else
    {
        while (1)
        {
            if (!(*((int *)(v1 + 172)) <= 1 ? *((short *)(*((int *)(v1 + 200)) + *(v7) * 2)) & 8 : sub_4758eb(*(v7), 8, &v1)))
                break;
            v7 += 1;
        }
        sub_4757b7(&v0, v7, sub_475860(v7, 0, 0, &v1));
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
        if (!v3)
            return;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return;
    }
}
