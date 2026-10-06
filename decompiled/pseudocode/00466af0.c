// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466af0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466af0_0 {
    char padding_0[120];
    unsigned int field_78;
    char padding_7c[16];
    HANDLE field_8c;
} st_466af0_0;

int sub_466af0(void)
{
    st_466af0_0 *v1;  // esi
    int v2;  // eax
    int v3;  // eax

    if (v1->field_78 != 1)
        return v3;
    v2 = CloseHandle(v1->field_8c);
    v1->field_8c = 0xffffffff;
    return v2;
}
