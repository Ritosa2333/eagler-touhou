// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42c1a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42c1a0_0 {
    char padding_0[12];
    unsigned int field_c;
    char padding_10[1052];
    int field_42c;
    unsigned int field_430;
    char padding_434[24];
    unsigned int field_44c;
    char padding_450[1676];
    unsigned int field_adc;
} st_42c1a0_0;

unsigned int sub_42c1a0(st_42c1a0_0 *idx)
{
    unsigned int v2;  // esi
    int v3;  // edx
    unsigned int *index;  // esi
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    unsigned int v0;  // [bp-0x4]

    if (idx->field_42c >= 18)
        return v6;
    v0 = v2;
    do
    {
        v3 = idx->field_42c;
        index = &(idx->padding_0)[2 * v3 + 1196];
        v5 = index[4];
        if (!v5 || !index[5] && idx->field_430)
            break;
        if (v5 != 0x80000000)
        {
            if (v5 == 0x200)
            {
                v5 = index[2];
                idx->field_44c = v5;
            }
            else if (v5 == 0x2000)
            {
                idx->field_c = 3;
            }
            else if (v5 == 0x10000000)
            {
                if (index[2])
                {
                    idx->field_adc = idx->field_adc & 0xffffff3f | 32;
                    idx->field_42c = idx->field_42c + 1;
                    continue;
                }
                else
                {
                    idx->field_adc = idx->field_adc & 0xffffff1f;
                    idx->field_42c = idx->field_42c + 1;
                    continue;
                }
            }
        }
        idx->field_42c = v3 + 1;
    } while (idx->field_42c < 18);
    return v5;
}
