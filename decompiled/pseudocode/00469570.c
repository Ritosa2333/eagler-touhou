// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x469570; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_469570_0 {
    char padding_0[4];
    struct st_469570_1 *field_4;
    char padding_8[4104];
    unsigned int field_1010;
    unsigned int field_1014;
    char padding_1018[20];
    unsigned int field_102c;
} st_469570_0;

typedef struct st_469570_1 {
    unsigned int field_0;
    unsigned int field_4;
} st_469570_1;

extern char g_49fc34;

st_469570_0 * sub_469570(int a0)
{
    int v0;  // esi
    st_469570_0 *index;  // eax
    st_469570_0 *idx;  // esi
    unsigned int v3;  // edi
    unsigned int v4;  // eax

    index = sub_46c9ea(4156, v0);
    idx = NULL;
    if (index)
    {
        *((char **)&index->padding_0[0]) = &g_49fc34;
        index->field_1010 = 0;
        index->field_1014 = 0;
        idx = index;
    }
    idx->field_102c = v3;
    v4 = sub_4694f0(a0);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_4->field_4 = v4;
    idx->field_4->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    return idx;
}
