// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464c40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464c40_0 {
    char padding_0[4];
    HANDLE field_4;
    char padding_8[4];
    unsigned int field_c;
    unsigned int field_10;
    char padding_14[4];
    unsigned int field_18;
} st_464c40_0;

void sub_464c40(void)
{
    st_464c40_0 *idx;  // esi
    HANDLE v3;  // eax
    unsigned int v4;  // edi
    unsigned int v0;  // [bp-0x8]

    v3 = idx->field_4;
    if (!idx->field_4)
        return;
    v0 = v4;
    idx->field_c = 1;
    idx->field_10 = 0;
    if (WaitForSingleObject(v3, 200) == WAIT_TIMEOUT)
    {
        do
        {
            idx->field_c = 1;
            idx->field_10 = 0;
            Sleep(1);
        } while (WaitForSingleObject(idx->field_4, 200) == WAIT_TIMEOUT);
    }
    CloseHandle(idx->field_4);
    idx->field_4 = NULL;
    idx->field_18 = 0;
    return;
}
