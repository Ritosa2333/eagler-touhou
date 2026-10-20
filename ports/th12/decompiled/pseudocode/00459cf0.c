// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x459cf0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_459cf0_0 {
    char padding_0[168];
    unsigned int field_a8;
} st_459cf0_0;

typedef struct st_459cf0_2 {
    unsigned int field_0;
} st_459cf0_2;

typedef struct st_459cf0_1 {
    char padding_0[228];
    struct st_459cf0_2 *field_e4;
    char padding_e8[44];
    struct st_459cf0_2 *field_114;
} st_459cf0_1;

typedef struct st_459cf0_4 {
    struct st_459cf0_1 *field_0;
} st_459cf0_4;

extern char g_4b5640;
extern char g_4b5646;
extern st_459cf0_4 *g_4ce8f0;

int sub_459cf0(void)
{
    st_459cf0_0 *idx;  // eax
    unsigned int *v5;  // edi
    int v6;  // esi
    char v7;  // al
    char v8;  // al
    unsigned int v0;  // [bp-0x30]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x8]

    if (*((char *)(idx + &g_4b5640)) != (char)(v5[287] >> 5 & 7))
    {
        sub_45a3c0(v6);
        v7 = v5[287] >> 5;
        *((char *)(idx + &g_4b5640)) = v7 & 7;
        switch (v7 & 7)
        {
        case 0:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19, 5);
            v1 = 6;
            goto LABEL_459dae;
        case 1: case 2:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19, 5);
            v1 = 2;
            goto LABEL_459dae;
        case 3:
            v2 = 2;
            goto LABEL_459d9a;
        case 4:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19, 10);
            v1 = 6;
            goto LABEL_459dae;
        case 5:
            v2 = 9;
LABEL_459d9a:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19);
            v1 = 1;
LABEL_459dae:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 20);
        case 6:
            g_4ce8f0->field_0->field_e4(g_4ce8f0, 19, 4);
            v1 = 6;
            goto LABEL_459dae;
        }
    }
    if (*((char *)(idx + &g_4b5646)) != (char)(v5[288] >> 1 & 1))
    {
        sub_45a3c0();
        v8 = v5[288] >> 1;
        *((char *)(idx + &g_4b5646)) = v8 & 1;
        if (!(v8 & 1))
        {
            g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 5, 2);
            v0 = 2;
        }
        else
        {
            g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 5, 1);
            v0 = 1;
        }
        g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 6);
    }
    idx->field_a8 = idx->field_a8 + 1;
    return;
}
