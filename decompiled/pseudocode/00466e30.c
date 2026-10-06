// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466e30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466e30_0 {
    char padding_0[120];
    unsigned int field_78;
    char padding_7c[16];
    HANDLE field_8c;
} st_466e30_0;

unsigned int sub_466e30(void)
{
    st_466e30_0 *v1;  // esi

    if (v1->field_78 == 1)
    {
        CloseHandle(v1->field_8c);
        v1->field_8c = 0xffffffff;
    }
    return 0;
}
