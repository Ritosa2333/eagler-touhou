// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41ad00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41ad00_1 {
    struct st_41ad00_0 *field_0;
    struct st_41ad00_1 *field_4;
} st_41ad00_1;

typedef struct st_41ad00_0 {
    char padding_0[9800];
    unsigned int field_2648;
    char padding_264c[8];
    unsigned int field_2654;
    unsigned int field_2658;
    char field_265c;
    char padding_265d[155];
    unsigned int field_26f8;
} st_41ad00_0;

typedef struct st_41ad00_2 {
    char padding_0[104];
    struct st_41ad00_1 *field_68;
} st_41ad00_2;

extern st_41ad00_2 *g_4b43dc;

unsigned int sub_41ad00(void)
{
    st_41ad00_1 *v4;  // eax
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v7;  // edi
    st_41ad00_1 *v8;  // ebp
    unsigned int idx;  // ecx
    unsigned int v10;  // esi
    int v11;  // edi
    int v12;  // edx
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    v4 = g_4b43dc->field_68;
    if (!v4)
        return 0;
    v2 = v5;
    v1 = v6;
    v0 = v7;
    do
    {
        v8 = v4->field_4;
        idx = &v4->field_0->padding_0[4160];
        if (*((int *)(idx + 5816)) & 0x400)
        {
            if (*((char *)(idx + 5660)) & 1)
            {
                v10 = *((int *)(idx + 5656));
                *((unsigned int *)(idx + 5652)) = *((int *)(idx + 5652)) + 0xfffe7961;
                v11 = *((int *)(idx + 5652)) - v10 * 7;
                v12 = (int)((unsigned int)(2454267027 * v11 >> 32) + v11) >> 2;
                *((unsigned int *)(idx + 5640)) = ((unsigned int)(v12) >> 31) + v12 + v10;
            }
            else
            {
                *((unsigned int *)(idx + 5640)) = *((int *)(idx + 5640)) + 0xfffe7961;
            }
        }
    } while ((v4 = v8, v4));
    return 0;
}
