// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454078; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454078_2 {
    char padding_0[144];
    struct st_454078_3 *field_90;
} st_454078_2;

typedef struct st_454078_1 {
    char padding_0[12];
    struct st_454078_2 *field_c;
} st_454078_1;

typedef struct st_454078_3 {
    char padding_0[28];
    unsigned int field_1c;
} st_454078_3;

typedef struct st_454078_0 {
    char padding_0[4];
    int field_4;
} st_454078_0;

typedef struct st_453670_2 {
    char field_0;
} st_453670_2;

typedef struct st_453670_1 {
    char padding_0[6532];
    struct st_453670_2 *field_1984;
} st_453670_1;

extern char g_4ceae8;
extern st_453670_1 g_4cf4e8;
extern char g_4d3654;
extern st_454078_1 *g_4d4754;

int sub_454078(unsigned int a0, unsigned int a1)
{
    st_454078_0 *idx;  // ebp
    int v2;  // ebx
    int v3;  // eax
    int v4;  // eax
    int v5;  // eax
    st_454078_1 *v6;  // ecx
    int v7;  // ecx
    int v8;  // eax
    int v9;  // eax

    if (g_4ceae8 & 16 && idx->field_4 >= v2)
    {
        v3 = (int)idx[1].padding_0;
        if (v3 == v2)
        {
            if (!sub_453ae0())
                goto LABEL_0x4543a1;
        }
        else
        {
            if (v3 == 2)
            {
                if (g_4d4754 == v2)
                    goto LABEL_0x4543a1;
                v4 = sub_4669f0();
            }
            else if (v3 == 5)
            {
                v5 = sub_4662f0();
                v7 = v6->field_c->field_90->field_1c;
                idx->field_4 = v7;
                v4 = sub_465fe0(v5, v7);
            }
            else if (v3 != 7)
            {
                if (v3 < 20)
                    goto LABEL_0x4543a1;
            }
            else
            {
                sub_466350();
            }
            if (v4 >= 0)
                goto LABEL_0x4543a1;
        }
    }
    if (g_4d4754 == v2)
        goto LABEL_0x454282;
    v8 = (int)idx[1].padding_0;
    if (v8 == v2)
    {
        return sub_4542d0();
    }
    else if (v8 == 1)
    {
        if (*((int *)&g_4d4754[7].padding_0[8]) != v2)
            goto LABEL_0x4543a4;
        sub_465de0();
    }
    else if (v8 != 2)
    {
        if (v8 == 3)
        {
            sub_4662f0();
            sub_4669f0();
            v9 = g_4d4754->field_c->field_90->field_1c;
            idx->field_4 = v9;
            return sub_454155(v9);
        }
        if (v8 != 4)
            if (v8 < 7)
                goto LABEL_0x4543a1;
        else
            sub_466350();
    }
    else
    {
        sub_453670((idx->field_4 < v2 ? (struct st_454078_0 *)&idx[1].field_4 : &(&g_4d3654)[0x100 * idx->field_4]), a1, &g_4cf4e8.padding_0[0]);
        sub_466bc0(v2);
    }
}
