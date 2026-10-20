// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473eac; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_473eac_0 {
    char padding_0[180];
    unsigned int field_b4;
} st_473eac_0;

typedef struct st_473eac_1 {
    char padding_0[72];
    unsigned int field_48;
    unsigned int field_4c;
    struct st_473eac_2 *field_50;
    struct st_473eac_2 *field_54;
    char padding_58[88];
    struct st_473eac_2 *field_b0;
    struct st_473eac_2 *field_b4;
    struct st_473eac_2 *field_b8;
    int field_bc;
    struct st_473eac_2 *field_c0;
    unsigned int field_c4;
    char padding_c8[4];
    unsigned int field_cc;
    unsigned int field_d0;
    struct st_473eac_0 *field_d4;
} st_473eac_1;

typedef struct st_473eac_2 {
    unsigned int field_0;
} st_473eac_2;

extern st_473eac_0 g_4adea8;
extern int g_4adf68;

void sub_473eac(st_473eac_1 *iter)
{
    unsigned int v1;  // edi
    unsigned int *v2;  // eax
    unsigned int *v3;  // eax
    st_473eac_1 *v4;  // edi
    st_473eac_0 *v5;  // eax
    st_473eac_1 *node;  // edi
    unsigned int *v7;  // eax
    unsigned int *v8;  // eax
    unsigned int v0;  // [bp-0x10]

    v0 = v1;
    if (iter->field_bc && iter->field_bc != &g_4adf68 && iter->field_b0 && !iter->field_b0->field_0)
    {
        v2 = &iter->field_b8->field_0;
        if (v2 && !*(v2))
        {
            sub_46c941(v2);
            sub_48415b(iter->field_bc);
        }
        v3 = &iter->field_b4->field_0;
        if (v3 && !*(v3))
        {
            sub_46c941(v3);
            sub_483f19(iter->field_bc);
        }
        sub_46c941(iter->field_b0);
        sub_46c941(iter->field_bc);
    }
    if (iter->field_c0 && !iter->field_c0->field_0)
    {
        sub_46c941(iter->field_c4 - 254);
        sub_46c941(iter->field_cc - 128);
        sub_46c941(iter->field_d0 - 128);
        sub_46c941(iter->field_c0);
    }
    v4 = &iter->field_d4;
    v5 = *((int *)&v4->padding_0[0]);
    if (v5 != &g_4adea8.padding_0[0] && !v5->field_b4)
    {
        sub_483cd8(v5);
        sub_46c941(*((int *)&v4->padding_0[0]));
    }
    node = &iter->field_50;
    iter = 6;
    do
    {
        if (*((int *)(&node->padding_0[0] - 8)) != "C")
        {
            v7 = *((int *)&node->padding_0[0]);
            if (v7 && !*(v7))
                sub_46c941(v7);
        }
        if (*((int *)(&node->padding_0[0] - 4)))
        {
            v8 = *((int *)&node->padding_0[4]);
            if (v8 && !*(v8))
                sub_46c941(v8);
        }
        node = &node->padding_0[16];
        iter -= 1;
    } while (iter != 1);
    sub_46c941(iter);
    return;
}
