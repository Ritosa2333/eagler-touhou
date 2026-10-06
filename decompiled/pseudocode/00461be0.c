// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461be0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461be0_1 {
    struct st_461be0_0 *field_0;
    struct st_461be0_1 *field_4;
} st_461be0_1;

typedef struct st_461be0_2 {
    char padding_0[1148];
    unsigned int field_47c;
} st_461be0_2;

typedef struct st_461be0_0 {
    char padding_0[1016];
    unsigned int field_3f8;
    char padding_3fc[128];
    unsigned int field_47c;
} st_461be0_0;

void sub_461be0(unsigned int a0, unsigned int a1)
{
    unsigned int idx;  // esi
    st_461be0_1 *v2;  // eax
    st_461be0_1 *v3;  // ecx
    st_461be0_0 *index;  // eax
    st_461be0_1 *v5;  // eax
    st_461be0_1 *v6;  // ecx
    st_461be0_2 *idx1;  // eax

    v2 = *((int *)(idx + 8935096));
    if (v2)
    {
        do
        {
            v3 = v2->field_4;
            index = v2->field_0;
            if (index->field_3f8 == a1)
                index->field_47c = index->field_47c | 0x10000000;
        } while ((v2 = v3, v2));
    }
    v5 = *((int *)(idx + 8935104));
    if (!v5)
        return;
    do
    {
        v6 = v5->field_4;
        idx1 = v5->field_0;
        if (*((int *)&idx1->padding_0[1016]) == a1)
            idx1->field_47c = idx1->field_47c | 0x10000000;
    } while ((v5 = v6, v5));
    return;
}
