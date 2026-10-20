// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4710f2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4710f2_0 {
    char padding_0[8];
    unsigned int field_8;
    char field_c;
} st_4710f2_0;


int sub_4710f2(void)
{
    st_4710f2_0 *v1;  // edi
    unsigned int *v3;  // eax
    int i;  // [bp+0x4]

    if (v1->field_c & 64 && !v1->field_8)
    {
        *(v3) = *(v3) + i;
        return;
    }
    while (i > 0)
    {
        i -= 1;
        sub_471099();
        if (*(v3) == 0xffffffff)
        {
            if (*((int *)sub_46e96f()) == 42)
                sub_471099();
            else
                return;
        }
    }
    return;
}
