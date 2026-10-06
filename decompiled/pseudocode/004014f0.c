// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4014f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4014f0_0 {
    char padding_0[102268];
    int field_18f7c;
    unsigned int field_18f80;
    char padding_18f84[8];
    unsigned int field_18f8c;
    char padding_18f90[4];
    unsigned int field_18f94;
    unsigned int field_18f98;
    unsigned int field_18f9c;
    unsigned int field_18fa0;
    unsigned int field_18fa4;
    unsigned int field_18fa8;
} st_4014f0_0;

int sub_4014f0(void)
{
    st_4014f0_0 *index;  // ecx
    int v3;  // edx
    char *v4;  // eax
    char *v5;  // esi
    unsigned int v6;  // edi
    unsigned int *idx;  // eax
    char *iter;  // esi
    char i;  // 4102
    unsigned int *idx1;  // ebx
    unsigned int v0;  // [bp-0x8]

    v3 = index->field_18f7c;
    v5 = v4;
    if (v3 >= 320)
        return;
    v0 = v6;
    idx = &index->padding_0[312 * v3 + 2428];
    index->field_18f7c = v3 + 1;
    iter = v5;
    do
    {
        i = *(iter);
        *(idx - v5 + iter) = *(iter);
        iter += 1;
    } while (i);
    idx[64] = *(idx1);
    idx[65] = idx1[1];
    idx[66] = idx1[2];
    idx[67] = index->field_18f80;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        idx[0x44] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx[0x44] = nan;
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        idx[69] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx[69] = nan;
        /* unsupported instruction */
    }
    idx[71] = index->field_18f8c;
    idx[72] = index->field_18f98;
    idx[73] = index->field_18f94;
    idx[74] = index->field_18f9c;
    idx[75] = index->field_18fa0;
    idx[76] = index->field_18fa4;
    idx[77] = index->field_18fa8;
    return;
}
