// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4314d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4314d0_0 {
    char padding_0[1020];
    unsigned int field_3fc;
    char padding_400[120];
    void* field_478;
    unsigned int field_47c;
} st_4314d0_0;

extern int g_4cee70;

st_4314d0_0 ** sub_4314d0(void)
{
    st_4314d0_0 **v1;  // ebx
    int v2;  // esi
    st_4314d0_0 *idx;  // esi
    int v4;  // edi
    void* v5;  // eax
    unsigned int v6;  // ecx
    void* iter;  // eax
    unsigned int i;  // ecx
    unsigned int v9;  // ecx

    sub_4615a0(g_4cee70, v1, 81, 0, v2);
    idx = sub_461920(*(v1));
    if (!idx)
        *(v1) = idx;
    v5 = sub_46d04a(v4 * 56);
    idx->field_478 = v5;
    if (v4 <= 2)
    {
        idx->field_47c = idx->field_47c & 0xf07fffff;
        return v1;
    }
    idx->field_47c = idx->field_47c & 0xf67fffff | 0x6000000;
    v6 = v4 * 2;
    idx->field_3fc = v4;
    if (v6 <= 0)
        return v1;
    /* unsupported instruction */
    /* unsupported instruction */
    iter = v5 + 12;
    /* unsupported instruction */
    /* unsupported instruction */
    do
    {
        i = v6;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&iter[4]) = 0xffffffff;
        *((int *)((char *)iter - 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        iter += 28;
        v9 = i - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)iter - 28)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v6 = v9;
    } while (i != 1);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return v1;
}
