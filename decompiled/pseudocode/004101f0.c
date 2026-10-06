// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4101f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4101f0_0 {
    unsigned int field_0;
    int field_4;
    char padding_8[4];
    unsigned int field_c;
    struct st_4101f0_1 *field_10;
} st_4101f0_0;

typedef struct st_4101f0_1 {
    unsigned int field_0;
} st_4101f0_1;

typedef struct st_4101f0_2 {
    unsigned int field_0;
    char field_4;
    char padding_5[23];
    unsigned int field_1c;
    char field_20;
    char padding_21[23];
    char field_38;
} st_4101f0_2;

void sub_4101f0(unsigned int a0)
{
    st_4101f0_0 *idx;  // ebx
    unsigned int *v4;  // edx
    unsigned int v13;  // eax
    unsigned int *k;  // esi
    unsigned int v15;  // ecx
    unsigned int iter;  // edi
    unsigned int v5;  // esi
    unsigned int index;  // esi
    int v7;  // ecx
    st_4101f0_2 *v8;  // eax
    int i;  // ebp
    unsigned int v10;  // ecx
    unsigned int *j;  // esi
    st_4101f0_2 *node;  // edi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = a0;
    if (!idx->field_0)
        return;
    v4 = &idx->field_10->field_0;
    v0 = v5;
    index = 0;
    v1 = 0;
    if (idx->field_0 - 1 <= 0)
        return;
    v7 = idx->field_4;
    do
    {
        v8 = *((int *)(*((int *)(idx->field_c + index * 4)) + 1144));
        i = 0;
        if (v7 > 0)
        {
            do
            {
                v10 = 7;
                j = v4;
                for (node = v8; v10; j += 1)
                {
                    v10 -= 1;
                    node->field_0 = *(j);
                    node = &node->field_4;
                }
                v13 = &v8->field_1c;
                k = &v4[7 * idx->field_4];
                v15 = 7;
                for (iter = v13; v15; k += 1)
                {
                    v15 -= 1;
                    *((unsigned int *)iter) = *(k);
                    iter += 4;
                }
                v7 = idx->field_4;
                i += 1;
                v8 = v13 + 28;
                v4 += 7;
            } while (i < idx->field_4);
            index = v1;
        }
    } while ((index += 1, v1 = index, index < idx->field_0 - 1));
    return;
}
