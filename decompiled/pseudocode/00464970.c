// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464970; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464970_0 {
    int field_0;
    char padding_4[4];
    unsigned int field_8;
    char padding_c[196];
    unsigned int field_d0;
    int field_d4;
} st_464970_0;


int sub_464970(void)
{
    st_464970_0 *idx;  // eax
    unsigned int i;  // esi
    unsigned int v5;  // ebx
    unsigned int v6;  // edi
    int v7;  // edi
    st_464970_0 *v8;  // ebx
    unsigned int v9;  // ecx
    unsigned int v10;  // ecx
    int v11;  // ecx
    st_464970_0 *v12;  // edx
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp+0x4]

    i = idx->field_8;
    if (i <= 0)
        return;
    v1 = v5;
    v0 = v6;
    v7 = idx->field_d4;
    v8 = &idx->padding_c[132];
    while (1)
    {
        idx->field_0 = idx->field_0 + v2;
        if (idx->field_0 >= i)
        {
            v9 = idx->field_d0;
            do
            {
                if (v9)
                    idx->field_0 = idx->field_0 - i;
                else
                    idx->field_0 = i - 1;
            } while (idx->field_0 >= i);
        }
        if (idx->field_0 < 0)
        {
            v10 = idx->field_d0;
            do
            {
                if (v10)
                    idx->field_0 = idx->field_0 + i;
                else
                    idx->field_0 = 0;
            } while (idx->field_0 < 0);
        }
        v11 = 0;
        v12 = v8;
        while (1)
        {
            if (v11 >= v7)
                return;
            if (v12->field_0 == idx->field_0)
                break;
            v11 += 1;
            v12 = v12->padding_4;
        }
    }
}
