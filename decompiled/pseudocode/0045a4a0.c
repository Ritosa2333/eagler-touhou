// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45a4a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45a4a0_4 {
    char padding_0[140];
    unsigned int field_8c;
} st_45a4a0_4;

typedef struct st_45a4a0_0 {
    char padding_0[28];
    unsigned int field_1c;
} st_45a4a0_0;

typedef struct st_45a4a0_1 {
    char padding_0[56];
    unsigned int field_38;
} st_45a4a0_1;

typedef struct st_45a4a0_2 {
    char padding_0[84];
    unsigned int field_54;
} st_45a4a0_2;

typedef struct st_45a4a0_3 {
    char padding_0[112];
    unsigned int field_70;
} st_45a4a0_3;

extern char g_4b56a0;

int sub_45a4a0(void)
{
    unsigned int idx;  // eax
    unsigned int *iter;  // edi
    unsigned int *m;  // ebp
    unsigned int v12;  // ecx
    unsigned int *iter1;  // esi
    st_45a4a0_2 *node;  // edi
    unsigned int v15;  // ecx
    st_45a4a0_3 *iter2;  // edi
    unsigned int v17;  // ecx
    st_45a4a0_4 *v18;  // edi
    unsigned int *n;  // esi
    unsigned int v20;  // ecx
    unsigned int v3;  // ecx
    unsigned int *v4;  // edx
    unsigned int *i;  // esi
    st_45a4a0_0 *v6;  // edi
    unsigned int *l;  // ebx
    unsigned int v8;  // ecx
    unsigned int *j;  // esi
    st_45a4a0_1 *v10;  // edi

    iter = *((int *)(idx + 8607396));
    v3 = 7;
    for (i = v4; v3; i += 1)
    {
        v3 -= 1;
        *(iter) = *(i);
        iter += 1;
    }
    v6 = *((int *)(idx + 8607396)) + 28;
    l = v4 + 7;
    v8 = 7;
    for (j = l; v8; j += 1)
    {
        v8 -= 1;
        *((unsigned int *)&v6->padding_0[0]) = *(j);
        v6 = &v6->padding_0[4];
    }
    v10 = *((int *)(idx + 8607396)) + 56;
    m = v4 + 14;
    v12 = 7;
    for (iter1 = m; v12; iter1 += 1)
    {
        v12 -= 1;
        *((unsigned int *)&v10->padding_0[0]) = *(iter1);
        v10 = &v10->padding_0[4];
    }
    node = *((int *)(idx + 8607396)) + 84;
    for (v15 = 7; v15; l += 1)
    {
        v15 -= 1;
        *((unsigned int *)&node->padding_0[0]) = *(l);
        node = &node->padding_0[4];
    }
    iter2 = *((int *)(idx + 8607396)) + 112;
    for (v17 = 7; v17; m += 1)
    {
        v17 -= 1;
        *((unsigned int *)&iter2->padding_0[0]) = *(m);
        iter2 = &iter2->padding_0[4];
    }
    v18 = *((int *)(idx + 8607396)) + 140;
    n = v4 + 21;
    for (v20 = 7; v20; n += 1)
    {
        v20 -= 1;
        *((unsigned int *)&v18->padding_0[0]) = *(n);
        v18 = &v18->padding_0[4];
    }
    *((unsigned int *)(idx + 8607396)) = *((int *)(idx + 8607396)) + 168;
    *((unsigned int *)&(&g_4b56a0)[idx]) = *((int *)&(&g_4b56a0)[idx]) + 1;
    return;
}
