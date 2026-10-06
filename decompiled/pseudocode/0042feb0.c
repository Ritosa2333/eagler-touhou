// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42feb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42feb0_0 {
    char padding_0[512];
    char field_200;
    char padding_201[891];
    unsigned int field_57c;
} st_42feb0_0;

extern int g_4a0b2c;
extern int g_4a0b60;
extern int g_4a0b94;
extern int g_4a0bb0;
extern int g_4a0bd8;
extern int g_4a0bf8;
extern int g_4a0c20;
extern int g_4a0c58;
extern int g_4a0c78;
extern int g_4a0c90;
extern int g_4a0cb4;
extern int g_4a0cd8;
extern unsigned int g_4ceab0;
extern unsigned int g_4ceab4;
extern unsigned int g_4ceab8;
extern unsigned int g_4ceabc;
extern unsigned int g_4ceac0;
extern unsigned short g_4ceac4;
extern char g_4ceaca;
extern char g_4ceacb;
extern char g_4ceacc;
extern char g_4ceacd;
extern char g_4ceace;
extern char g_4ceacf;
extern unsigned int g_4cee64;
extern unsigned int g_4d49ec;
extern unsigned int g_4d49f0;
extern unsigned int g_4d49f4;
extern unsigned int g_4d49f8;
extern unsigned short g_4d49fc;

unsigned int sub_42feb0(st_42feb0_0 *a0)
{
    int v1;  // edi
    int v2;  // esi
    int v3;  // ebp
    int v4;  // ebx
    unsigned int *i;  // eax
    unsigned int v6;  // ecx
    unsigned int *iter;  // edi
    unsigned int *v0;  // [bp-0x1c], Other Possible Types: int

    sub_423250(v1, v2, v3, v4);
    i = sub_463c10(&a0, 1);
    if (!i)
    {
        v0 = &g_4a0b2c;
        goto LABEL_42ff84;
    }
    else
    {
        v6 = 15;
        iter = &g_4ceab0;
        for (v0 = i; v6; i += 1)
        {
            v6 -= 1;
            *(iter) = *(i);
            iter += 1;
        }
        sub_46c941();
        if (g_4ceaca < 2 && g_4ceacb < 3 && g_4ceacc < 2 && g_4ceacd < 4 && g_4ceace < 3 && g_4ceacf < 3 && g_4ceab0 == 1179649 && v4 == 60)
        {
            g_4d49ec = g_4ceab4;
            g_4d49f0 = g_4ceab8;
            g_4d49f4 = g_4ceabc;
            g_4d49f8 = g_4ceac0;
            g_4d49fc = g_4ceac4;
        }
        else
        {
            v0 = &g_4a0b60;
LABEL_42ff84:
            sub_464220(v0);
            sub_423250();
        }
    }
    a0->field_57c = 0;
    if (a0->field_200 & 4)
        sub_464220(&g_4a0b94);
    if (a0->field_200 & 1)
        sub_464220(&g_4a0bb0);
    if (*((int *)&a0->padding_0[276]))
        sub_464220(&g_4a0bd8);
    if (a0->field_200 & 2)
        sub_464220(&g_4a0bf8);
    if (a0->field_200 & 8)
        sub_464220(&g_4a0c20);
    if (a0->field_200 & 16)
        sub_464220(&g_4a0c58);
    if (a0->field_200 & 32)
    {
        sub_464220(&g_4a0c78);
        g_4cee64 = 1;
    }
    if (a0->field_200 & 64)
        sub_464220(&g_4a0c90);
    if (!sub_463e80(&g_4ceab0))
        return 0;
    sub_464300(&g_4a0cb4, "th12.cfg");
    sub_464300(&g_4a0cd8);
    return 0xffffffff;
}
