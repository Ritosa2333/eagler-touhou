// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x469110; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_469110_1 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[4096];
    unsigned int field_1008;
    unsigned int field_100c;
    unsigned int field_1010;
    struct st_469110_0 *field_1014;
    char padding_1018[4];
    char field_101c;
} st_469110_1;

typedef struct st_469110_0 {
    struct st_469110_1 *field_0;
    struct st_469110_0 *field_4;
    struct st_469110_0 *field_8;
    char padding_c[4112];
    char field_101c;
    char padding_101d[23];
    struct st_469110_0 *field_1034;
} st_469110_0;


int sub_469110(void)
{
    int v2;  // edi
    int v3;  // esi
    int v4;  // ebx
    st_469110_1 *idx;  // eax
    st_469110_0 *index;  // eax
    st_469110_0 *v7;  // eax
    st_469110_0 *idx1;  // ecx
    unsigned int v0;  // [bp+0x4]
    int v1;  // [bp+0x8]

    idx = sub_46c9ea(4132, v2, v3, v4);
    if (idx)
    {
        idx->field_1008 = 0;
        idx->field_100c = 0;
    }
    else
    {
        idx = NULL;
    }
    index = sub_46c9ea(12);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_1010 = v0;
    idx->field_1014 = v7;
    idx->field_4 = 0;
    idx->field_101c = v7->field_4->field_101c;
    idx1 = &v7->padding_101d[19];
    index->field_0 = idx;
    index->field_4 = NULL;
    index->field_8 = NULL;
    if (idx1->field_4)
    {
        index->field_4 = idx1->field_4;
        idx1->field_4->field_8 = index;
    }
    idx1->field_4 = index;
    index->field_8 = idx1;
    sub_466f00(v1);
    return;
}
