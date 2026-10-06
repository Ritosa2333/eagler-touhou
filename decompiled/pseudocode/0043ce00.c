// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43ce00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43ce00_0 {
    unsigned short field_0;
    unsigned short field_2;
    char padding_4[4];
    unsigned int field_8;
    char padding_c[4];
    int field_10;
    char field_14;
    char field_15;
    unsigned int field_16;
    unsigned int field_1a;
    char field_1e;
    char padding_1f[1];
    unsigned int field_20;
    unsigned int field_24;
    char padding_28[1732];
    int field_6ec;
    unsigned int field_6f0;
} st_43ce00_0;

extern char g_4af208;

void sub_43ce00(void)
{
    st_43ce00_0 *index;  // esi
    int v2;  // edi
    int v3;  // ebx
    st_43ce00_0 *iter;  // eax
    unsigned int v5;  // edi
    unsigned int i;  // edi
    int k;  // ecx
    int j;  // eax
    st_43ce00_0 *node;  // ecx

    sub_477420(index, 0, 17908, v2, v3);
    index->field_0 = 21059;
    index->field_2 = 2;
    index->field_8 = 17908;
    iter = &index->field_14;
    v5 = 5;
    do
    {
        i = v5;
        k = 100000;
        do
        {
            *((int *)((char *)iter - 4)) = k;
            *((char *)&iter->field_0) = 1;
            *((unsigned int *)&iter->field_2) = 0x2d2d2d2d;
            *((unsigned int *)&iter->padding_4[2]) = 0x2d2d2d2d;
            *((char *)&iter->field_8 + 2) = 0;
            *((unsigned int *)&iter->padding_c[0]) = 0;
            iter->field_10 = 0;
            *((char *)&iter->field_0 + 1) = 0;
            k -= 10000;
            iter = &iter->field_1a;
        } while (k > 0);
        v5 = i - 1;
    } while (i != 1);
    j = 0;
    node = &index->field_6ec;
    do
    {
        *((int *)((char *)node - 4)) = j;
        *((int *)&node->field_0) = (&g_4af208)[j];
        j += 1;
        node = &node->padding_28[100];
    } while (j < 113);
    return;
}
