// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41cf40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41cf40_1 {
    unsigned int field_0;
} st_41cf40_1;

typedef struct st_41cf40_0 {
    char padding_0[10612];
    unsigned short field_2974;
    char padding_2976[206];
    struct st_41cf40_1 *field_2a44;
} st_41cf40_0;

void sub_41cf40(st_41cf40_0 *a0, int a1, unsigned short a2)
{
    int v0;  // edi
    st_41cf40_0 *iter;  // esi
    int v2;  // ebx
    int v3;  // ebx
    st_41cf40_0 *v4;  // esi
    int v5;  // edi
    st_41cf40_0 *node;  // esi
    unsigned int v7;  // ebx
    unsigned int v8;  // ebx

    v0 = 0;
    if (a1 > 0)
    {
        iter = &a0->field_2a44;
        v2 = a1;
        v0 = a1;
        do
        {
            v3 = v2;
            if (*((int *)&iter->padding_0[0]))
                (*((int *)&iter->padding_0[0]))();
        } while ((*((unsigned short *)((char *)iter - 208)) = 2, iter += 1204, v2 = (int)(v3 - 1), v3 != 1));
        if (a1 >= 8)
            return;
    }
    v4 = &a0->padding_0[1204 * v0 + 9648];
    if (*((int *)&v4->padding_0[1172]))
        (*((int *)&v4->padding_0[1172]))();
    v5 = v0 + 1;
    *((unsigned short *)&v4->padding_0[964]) = a2 + 7;
    if (v5 >= 8)
        return;
    node = &a0->padding_0[1204 * v5 + 10820];
    v7 = 8 - v5;
    do
    {
        v8 = v7;
        if (*((int *)&node->padding_0[0]))
            (*((int *)&node->padding_0[0]))();
    } while ((*((unsigned short *)((char *)node - 208)) = 3, node += 1204, v7 = v8 - 1, v8 != 1));
    return;
}
