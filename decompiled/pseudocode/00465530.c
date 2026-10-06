// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465530; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465530_0 {
    unsigned int field_0;
    struct st_465530_0 *field_4;
} st_465530_0;

int sub_465530(void)
{
    st_465530_0 *v1;  // eax
    st_465530_0 *v2;  // eax
    st_465530_0 *v3;  // eax
    unsigned int v4;  // ecx

    if (v1)
    {
        do
        {
            v3 = v2;
            if (v3->field_0 == v4)
                return;
        } while ((v2 = (st_465530_0 *)v3->field_4, v3->field_4));
    }
    return;
}
