// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461a70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461a70_1 {
    struct st_461a70_0 *field_0;
    struct st_461a70_1 *field_4;
} st_461a70_1;

typedef struct st_461a70_0 {
    char padding_0[1148];
    unsigned int field_47c;
} st_461a70_0;

typedef struct st_461a70_2 {
    char padding_0[20];
    struct st_461a70_1 *field_14;
    unsigned int field_18;
    char padding_1c[1120];
    unsigned int field_47c;
} st_461a70_2;

extern unsigned int g_4ce8cc;

void sub_461a70(unsigned int a0, unsigned int a1, unsigned int a2)
{
    st_461a70_2 *idx;  // eax
    st_461a70_1 *v1;  // eax
    st_461a70_1 *v2;  // eax

    idx = sub_461920(a0, g_4ce8cc, a2);
    if (!idx)
        return;
    idx->field_47c = idx->field_47c | 0x10000000;
    if (idx->field_18)
        return;
    v1 = idx->field_14;
    if (!idx->field_14)
        return;
    do
    {
        v2 = v1;
        v2->field_0->field_47c = v2->field_0->field_47c | 0x10000000;
        v1 = v2->field_4;
    } while (v2->field_4);
    return;
}
