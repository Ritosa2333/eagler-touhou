// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x49231b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_49231b_0 {
    char padding_0[136];
    unsigned int field_88;
} st_49231b_0;

typedef struct st_49231b_2 {
    char padding_0[136];
    struct st_49231b_3 *field_88;
} st_49231b_2;

typedef struct st_49231b_1 {
    char padding_0[140];
    unsigned int field_8c;
} st_49231b_1;

typedef struct st_49231b_24 {
    char padding_0[144];
    unsigned int field_90;
} st_49231b_24;

typedef struct st_49231b_3 {
    unsigned int field_0;
} st_49231b_3;

void sub_49231b(unsigned int *index, unsigned int a1)
{
    unsigned int v2;  // ebx
    unsigned int v3;  // edi
    st_49231b_2 *v12;  // eax
    st_49231b_2 *v13;  // eax
    st_49231b_2 *v14;  // eax
    st_49231b_0 *v15;  // eax
    st_49231b_24 *idx;  // eax
    st_49231b_0 *v17;  // eax
    st_49231b_1 *v18;  // eax
    st_49231b_2 *v4;  // eax
    st_49231b_2 *v5;  // eax
    st_49231b_2 *v6;  // eax
    st_49231b_2 *v7;  // eax
    st_49231b_2 *v8;  // eax
    st_49231b_2 *v9;  // eax
    st_49231b_0 *v10;  // eax
    st_49231b_2 *v11;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    if (index[2] == 0xffffffff)
        return;
    v1 = v2;
    v0 = v3;
    sub_491da7(index);
    if (!a1)
    {
        v4 = sub_475467();
        if (v4->field_88->field_0 == 3765269347)
        {
            v5 = sub_475467();
            if (v5->field_88[4].field_0 == 3)
            {
                v6 = sub_475467();
                if (v6->field_88[5].field_0 == 429065504 || (v7 = (st_49231b_2 *)sub_475467(), *((int *)(*((int *)(sub_475467() + 0x88)) + 20)) == 429065505 || (v8 = (st_49231b_2 *)sub_475467(), *((int *)(*((int *)(sub_475467() + 0x88)) + 20)) == 429065506)))
                {
                    v9 = sub_475467();
                    if (sub_491d80(v9->field_88[6].field_0))
                    {
                        v10 = sub_475467(1);
                        sub_492181(v10->field_88);
                    }
                }
            }
        }
    }
    v11 = sub_475467();
    if (v11->field_88->field_0 == 3765269347)
    {
        v12 = sub_475467();
        if (v12->field_88[4].field_0 == 3)
        {
            v13 = sub_475467();
            if ((v13->field_88[5].field_0 == 429065504 || (v14 = (st_49231b_2 *)sub_475467(), *((int *)(*((int *)(sub_475467() + 0x88)) + 20)) == 429065505 || (v15 = (st_49231b_0 *)sub_475467(), *((int *)(*((int *)(sub_475467() + 0x88)) + 20)) == 429065506))) && a1)
            {
                idx = sub_475467();
                idx->field_90 = idx->field_90 - 1;
            }
        }
    }
    v17 = sub_475467();
    v17->field_88 = index[2];
    v18 = sub_475467();
    v18->field_8c = index[3];
    return;
}
