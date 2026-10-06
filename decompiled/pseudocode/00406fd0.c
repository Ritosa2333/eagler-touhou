// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406fd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406fd0_0 {
    char padding_0[40];
    int field_28;
    char padding_2c[80];
    unsigned int field_7c;
    unsigned int field_80;
} st_406fd0_0;

typedef struct st_406fd0_1 {
    char padding_0[60];
    unsigned int field_3c;
} st_406fd0_1;

extern st_406fd0_1 *g_4b43c4;
extern st_406fd0_0 *g_4b43cc;

unsigned int sub_406fd0(void)
{
    unsigned int v1;  // eax

    v1 = g_4b43cc->field_7c;
    if (!((char)v1 & 1))
    {
        return v1;
    }
    else if (g_4b43cc->field_28 >= 60)
    {
        g_4b43cc->field_80 = 0;
        g_4b43cc->field_7c = v1 & 0xffffffdd;
        return v1 & 0xffffffdd;
    }
    else
    {
        if (!g_4b43c4->field_3c)
            return v1 | 32;
        g_4b43cc->field_7c = v1 | 32;
        return v1 | 32;
    }
}
