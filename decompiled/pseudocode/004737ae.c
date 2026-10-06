// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4737ae; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4737ae_0 {
    char padding_0[16];
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    char field_1c;
    char padding_1d[256];
    char field_11d;
} st_4737ae_0;

extern char g_4ad4b0;

int sub_4737ae(void)
{
    st_4737ae_0 *v2;  // eax
    int v3;  // edi
    unsigned int v12;  // esi
    unsigned int j;  // esi
    st_4737ae_0 *v4;  // edi
    st_4737ae_0 *v5;  // edi
    st_4737ae_0 *iter;  // eax
    unsigned int v7;  // ecx
    unsigned int v8;  // edi
    unsigned int i;  // edi
    st_4737ae_0 *node;  // eax
    int v0;  // [bp-0x4]

    sub_477420(&v2->field_1c, 0, 0x101, v3, v0);
    memset(&v2->padding_0[4], 0, 12);
    v4 = &v2->field_10;
    *((unsigned int *)&v4->padding_0[0]) = 0;
    v5 = &v4->padding_0[4];
    *((unsigned int *)&v5->padding_0[0]) = 0;
    *((unsigned int *)&v5->padding_0[4]) = 0;
    iter = &v2->field_1c;
    v7 = &g_4ad4b0 - v2;
    v8 = 0x101;
    do
    {
        i = v8;
        iter->padding_0[0] = iter->padding_0[v7];
        iter = &iter->padding_0[1];
        v8 = i - 1;
    } while (i != 1);
    node = &v2->field_11d;
    v12 = 0x100;
    do
    {
        j = v12;
        node->padding_0[0] = node->padding_0[v7];
        node = &node->padding_0[1];
        v12 = j - 1;
    } while (j != 1);
    return;
}
