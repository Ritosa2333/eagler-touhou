// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4741f7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4741f7_0 {
    char padding_0[112];
    unsigned int field_70;
} st_4741f7_0;

extern unsigned int g_4ad9d4;

unsigned int sub_4741f7(unsigned int a0)
{
    int v3;  // edi
    st_4741f7_0 *idx;  // eax
    unsigned int v5;  // ecx
    unsigned int v6;  // ecx
    unsigned int v0;  // [bp-0x10]
    int v1;  // [bp-0x8]
    char v2;  // [bp+0x0]

    idx = sub_475467(v3, v1, &v2);
    v5 = idx->field_70;
    v0 = 0;
    if (a0 == 0xffffffff)
    {
        g_4ad9d4 = 0xffffffff;
    }
    else if (a0)
    {
        if (a0 != 1)
        {
            if (a0 != 2)
            {
                *((unsigned int *)sub_46e96f()) = 22;
                sub_470f2d(0, 0, 0, 0, 0);
                return 0xffffffff;
            }
            v6 = v5 & 0xfffffffd;
        }
        else
        {
            v6 = v5 | 2;
        }
        idx->field_70 = v6;
    }
    return (unsigned int)((!((char)v5 & 2)) + 1);
}
