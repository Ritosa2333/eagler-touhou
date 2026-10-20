// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4236f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4236f0_2 {
    char padding_0[28];
    struct st_4236f0_0 *field_1c;
    char padding_20[92];
    struct st_4236f0_0 *field_7c;
} st_4236f0_2;

typedef struct st_4236f0_0 {
    int field_0;
    struct st_4236f0_0 *field_4;
} st_4236f0_0;

int sub_4236f0(void)
{
    st_4236f0_2 *v1;  // eax
    st_4236f0_2 *iter;  // edi
    unsigned int v3;  // ebx
    unsigned int i;  // ebx
    st_4236f0_0 *v5;  // eax
    st_4236f0_0 *v6;  // eax
    st_4236f0_0 *v7;  // esi
    st_4236f0_0 *v8;  // eax
    st_4236f0_0 *v9;  // eax
    st_4236f0_0 *v10;  // esi

    iter = &v1->field_7c;
    v3 = 8;
    do
    {
        i = v3;
        v5 = *((int *)(&iter->padding_0[0] - 96));
        if (*((int *)(&iter->padding_0[0] - 96)))
        {
            do
            {
                v6 = v5;
                v7 = v6->field_4;
                sub_46ca4f(v6->field_0);
                v5 = v7;
            } while (v6->field_4);
        }
        v8 = *((int *)&iter->padding_0[0]);
        if (*((int *)&iter->padding_0[0]))
        {
            do
            {
                v9 = v8;
                v10 = v9->field_4;
                sub_46ca4f(v9->field_0);
                v8 = v10;
            } while (v9->field_4);
        }
        iter = &iter->padding_0[12];
        v3 = i - 1;
    } while (i != 1);
    return;
}
