// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461c90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461c90_1 {
    struct st_461c90_0 *field_0;
    struct st_461c90_1 *field_4;
} st_461c90_1;

typedef struct st_461c90_0 {
    char padding_0[1148];
    unsigned int field_47c;
} st_461c90_0;

typedef struct st_461c90_2 {
    char padding_0[20];
    struct st_461c90_1 *field_14;
    unsigned int field_18;
    char padding_1c[1120];
    unsigned int field_47c;
} st_461c90_2;

extern unsigned int g_4ce8cc;

int sub_461c90(void)
{
    unsigned int *v1;  // eax
    st_461c90_2 *idx;  // eax
    unsigned int v3;  // 4105
    st_461c90_1 *v4;  // eax
    st_461c90_1 *v5;  // eax

    idx = sub_461920(*(v1), g_4ce8cc, *(v1));
    if (!idx)
        return;
    v3 = idx->field_18;
    idx->field_47c = idx->field_47c & 0xfffffffd | 0x40000;
    if (v3)
        return;
    v4 = idx->field_14;
    if (!idx->field_14)
        return;
    do
    {
        v5 = v4;
        v5->field_0->field_47c = v5->field_0->field_47c & 0xfffffffd;
        v5->field_0->field_47c = v5->field_0->field_47c | 0x40000;
        v4 = v5->field_4;
    } while (v5->field_4);
    return;
}
