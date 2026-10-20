// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461fe0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461fe0_1 {
    char padding_0[16];
    struct st_461fe0_0 *field_10;
    struct st_461fe0_1 *field_14;
} st_461fe0_1;

typedef struct st_461fe0_0 {
    unsigned int field_0;
    char padding_4[998];
    short field_3ea;
} st_461fe0_0;

int sub_461fe0(void)
{
    st_461fe0_1 *v1;  // ecx
    st_461fe0_1 *v2;  // ecx
    st_461fe0_1 *v3;  // ecx
    unsigned int v4;  // edx
    unsigned int *v5;  // eax
    unsigned int *v6;  // eax

    v2 = &v1->field_10;
    if (v1 != 0xfffffff0)
    {
        do
        {
            v3 = v2;
            if (*((short *)(*((int *)&v3->padding_0[0]) + 1002)) == v4 || v4 == 0xffffffff)
            {
                *(v5) = *((int *)*((int *)&v3->padding_0[0]));
                return;
            }
        } while ((v2 = (st_461fe0_1 *)*((int *)&v3->padding_0[4]), *((int *)&v3->padding_0[4])));
    }
    *(v6) = 0;
    return;
}
