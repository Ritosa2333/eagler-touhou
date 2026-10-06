// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466460; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466460_0 {
    char padding_0[120];
    unsigned int field_78;
    char padding_7c[16];
    HANDLE field_8c;
} st_466460_0;

typedef struct st_466460_1 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[4];
    struct st_466460_0 *field_c;
    unsigned int field_10;
    char padding_14[8];
    unsigned int field_1c;
    char padding_20[16];
    unsigned int field_30;
    unsigned int field_34;
} st_466460_1;

int sub_466460(void)
{
    st_466460_1 *idx;  // eax
    unsigned int v4;  // ebx
    unsigned int v5;  // edi
    unsigned int v6;  // edi
    st_466460_0 *v7;  // esi
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp+0x4]

    if (!idx->field_4)
        return;
    v1 = v4;
    v0 = v5;
    v6 = 0;
    idx->field_30 = 0;
    idx->field_34 = 0;
    if (idx->field_10 > 0)
    {
        do
        {
            (*((int *)(*((int *)*((int *)(idx->field_4 + v6 * 4))) + 72)))(*((int *)(idx->field_4 + v6 * 4)));
        } while (((*((int *)(*((int *)*((int *)(idx->field_4 + v6 * 4))) + 52)))(*((int *)(idx->field_4 + v6 * 4)), 0), v6 += 1, v6 < idx->field_10));
    }
    idx->field_1c = 0;
    if (!v2)
        return;
    v7 = idx->field_c;
    if (v7->field_78 != 1)
        return;
    CloseHandle(v7->field_8c);
    v7->field_8c = 0xffffffff;
    return;
}
