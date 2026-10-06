// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4927d2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4927d2_0 {
    char padding_0[136];
    unsigned int field_88;
} st_4927d2_0;

typedef struct st_4927d2_1 {
    char padding_0[140];
    unsigned int field_8c;
} st_4927d2_1;

typedef struct st_4927d2_2 {
    unsigned int field_0;
    char padding_4[12];
    unsigned int field_10;
    unsigned int field_14;
    int field_18;
} st_4927d2_2;

void sub_4927d2(void)
{
    void* v1;  // edi
    void* v2;  // ebp
    st_4927d2_0 *v3;  // eax
    st_4927d2_1 *v4;  // eax
    st_4927d2_2 *v5;  // esi
    unsigned int v6;  // eax

    *((int *)((char *)v1 - 4)) = *((int *)((char *)v2 - 36));
    sub_491da7(*((int *)((char *)v2 - 40)));
    v3 = sub_475467();
    v3->field_88 = *((int *)((char *)v2 - 44));
    v4 = sub_475467();
    v4->field_8c = *((int *)((char *)v2 - 48));
    if (v5->field_0 != 3765269347)
    {
        return;
    }
    else if (v5->field_10 == 3)
    {
        v6 = v5->field_14;
        if (v6 != 429065504 && v6 != 429065505 && v6 != 429065506)
            return;
        if (*((int *)((char *)v2 - 52)))
        {
            return;
        }
        else if (*((int *)((char *)v2 - 28)))
        {
            if (!sub_491d80(v5->field_18))
                return;
            sub_492181(v5, (int)v2[16]);
            return;
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }
}
