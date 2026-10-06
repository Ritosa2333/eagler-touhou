// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x437680; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_437680_4 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_437680_4;

typedef struct st_437680_2 {
    char padding_0[2604];
    struct st_437680_0 *field_a2c;
} st_437680_2;

typedef struct st_437680_0 {
    char padding_0[2];
    unsigned short field_2;
} st_437680_0;

typedef struct st_437680_3 {
    char padding_0[36];
    struct st_437680_4 *field_24;
    struct st_437680_4 *field_28;
    struct st_437680_4 *field_2c;
    struct st_437680_4 *field_30;
    char field_34;
} st_437680_3;

extern unsigned int g_4aebd4[4];
extern unsigned int g_4aedf0[4];
extern unsigned int g_4aee10[4];
extern unsigned int g_4ce8ac[4];

unsigned int sub_437680(void)
{
    st_437680_0 *v2;  // eax
    st_437680_2 *index;  // esi
    unsigned int v4;  // edx
    unsigned int v5;  // ecx
    unsigned int v6;  // edi
    st_437680_0 *v7;  // eax
    st_437680_3 *iter;  // eax
    unsigned int v0;  // [bp-0xc]

    v2 = sub_463c10(0, 0);
    index->field_a2c = v2;
    if (!v2)
        return 0xffffffff;
    v4 = 0;
    if (0 >= v2->field_2)
        return 0;
    v5 = 616;
    v0 = v6;
    do
    {
        v7 = index->field_a2c;
        *((st_437680_0 **)&v7->padding_0[v5]) = &v7->padding_0[*((int *)&v7->padding_0[v5])];
        iter = *((int *)&index->field_a2c->padding_0[v5]);
        if (*((char *)*((int *)&index->field_a2c->padding_0[v5])) >= 0)
        {
            do
            {
                iter->field_24 = *((int *)(0x4 * iter->field_24 + (char *)&g_4aedf0[0]));
                iter->field_28 = *((int *)(0x4 * iter->field_28 + (char *)&g_4aebd4[0]));
                iter->field_2c = *((int *)(0x4 * iter->field_2c + (char *)&g_4ce8ac[0]));
                iter->field_30 = *((int *)(0x4 * iter->field_30 + (char *)&g_4aee10[0]));
                iter = &iter->field_34;
            } while (iter->padding_0[0] >= 0);
        }
    } while ((v4 += 1, v5 += 8, v4 < (unsigned int)index->field_a2c->field_2));
    return 0;
}
