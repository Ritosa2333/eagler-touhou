// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43fcb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43fcb0_0 {
    char padding_0[24];
    int field_18;
    char padding_1c[8];
    unsigned int field_24;
    unsigned int field_28;
    char padding_2c[4];
    unsigned int field_30;
    char padding_34[644];
    int field_2b8;
    char padding_2bc[408];
    int field_454;
    char padding_458[24];
    int field_470;
    char padding_474[684];
    int field_720;
} st_43fcb0_0;

extern unsigned int g_4d48c4;

int sub_43fcb0(void)
{
    unsigned int v2;  // ecx
    st_43fcb0_0 *idx;  // eax
    int v4;  // esi
    unsigned int v5;  // eax
    unsigned int v6;  // 4101
    int v7;  // ebx
    unsigned int v0;  // [bp-0x4]

    v0 = v2;
    switch (idx->field_24)
    {
    case 2:
        if (g_4d48c4 & 524547)
        {
            sub_461970(idx->field_470, v7);
            sub_43ef40(idx->field_470, v7);
            sub_453d90(v2, 14);
            sub_461970(idx->field_720);
            return;
        }
        break;
    case 4:
        if (idx->field_2b8 >= 30)
            sub_43eee0();
        break;
    case 0:
        if (!sub_461920(idx->field_454))
        {
            sub_43efa0(v4);
            sub_43efa0(v4);
            sub_4615a0(idx->field_18, &edi, 0, 0);
            idx->field_720 = 0;
        }
        sub_43ef40();
        v5 = idx->field_30;
        if (v5)
        {
            v6 = _ccall(14, 15, v5, 0, 0);
            if (v6 & 1)
            {
                idx->field_28 = v5 - 1;
                break;
            }
            else
            {
                idx->field_28 = 0;
                break;
            }
        }
        else
        {
            idx->field_28 = 0;
            break;
        }
    case 1:
        if (idx->field_2b8 > 10)
        {
            sub_43ef40();
            return;
        }
        break;
    default:
        return;
    }
}
