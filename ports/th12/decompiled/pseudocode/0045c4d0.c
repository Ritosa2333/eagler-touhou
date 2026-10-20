// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45c4d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45c4d0_1 {
    char padding_0[959];
    char field_3bf;
    char padding_3c0[52];
    struct st_45c4d0_0 *field_3f4;
    char padding_3f8[132];
    unsigned int field_47c;
} st_45c4d0_1;

typedef struct st_45c4d0_0 {
    char padding_0[8];
    unsigned int field_8;
} st_45c4d0_0;

typedef struct st_45c4d0_3 {
    unsigned int field_0;
} st_45c4d0_3;

typedef struct st_45c4d0_2 {
    char padding_0[260];
    struct st_45c4d0_3 *field_104;
} st_45c4d0_2;

typedef struct st_45c4d0_4 {
    struct st_45c4d0_2 *field_0;
} st_45c4d0_4;

extern char g_4b563c;
extern char g_4b5642;
extern st_45c4d0_4 *g_4ce8f0;

int sub_45c4d0(void)
{
    st_45c4d0_1 *v1;  // eax
    unsigned int idx;  // ecx

    if (!((char)v1->field_47c & 1))
    {
        return;
    }
    else if (!((char)v1->field_47c & 2))
    {
        return;
    }
    else if (!v1->field_3bf)
    {
        return;
    }
    else
    {
        if (*((int *)&(&g_4b563c)[idx]) != v1->field_3f4->field_8)
        {
            *((unsigned int *)&(&g_4b563c)[idx]) = v1->field_3f4->field_8;
            sub_45a3c0();
            g_4ce8f0->field_0->field_104(g_4ce8f0, 0, *((int *)*((int *)&(&g_4b563c)[idx])));
        }
        if ((&g_4b5642)[idx] != 1)
        {
            sub_45a3c0();
            (&g_4b5642)[idx] = 1;
        }
        sub_459cf0();
        sub_45a4a0();
        return;
    }
}
