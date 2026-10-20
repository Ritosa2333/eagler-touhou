// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x431580; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_431580_0 {
    char padding_0[1020];
    unsigned int field_3fc;
    char padding_400[120];
    void* field_478;
    unsigned int field_47c;
} st_431580_0;

extern unsigned int g_4ce8cc;
extern int g_4cee70;

st_431580_0 ** sub_431580(void)
{
    st_431580_0 **v1;  // ebx
    int v2;  // esi
    st_431580_0 *idx;  // esi
    int v4;  // edi
    void* v5;  // eax
    unsigned int v6;  // ecx
    unsigned int v7;  // ecx
    void* iter;  // eax
    unsigned int i;  // ecx
    unsigned int v10;  // ecx

    sub_4615a0(g_4cee70, v1, 82, 0, v2);
    idx = sub_461920(*(v1), g_4ce8cc, *(v1));
    if (!idx)
        *(v1) = idx;
    v5 = sub_46d04a(v4 * 56);
    v6 = idx->field_47c & 0xf67fffff | 0x6000000;
    idx->field_478 = v5;
    idx->field_47c = v6;
    idx->field_3fc = v4;
    if (v4 <= 2)
    {
        idx->field_47c = v6 & 0xf07fffff;
        return v1;
    }
    v7 = v4 * 2;
    if (v7 <= 0)
        return v1;
    /* unsupported instruction */
    /* unsupported instruction */
    iter = v5 + 12;
    /* unsupported instruction */
    /* unsupported instruction */
    do
    {
        i = v7;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&iter[4]) = 0xffffffff;
        *((int *)((char *)iter - 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        iter += 28;
        v10 = i - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)iter - 28)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v7 = v10;
    } while (i != 1);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return v1;
}
