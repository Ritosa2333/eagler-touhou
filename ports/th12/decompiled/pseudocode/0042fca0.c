// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42fca0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42fca0_7 {
    unsigned short field_0;
    char field_2;
} st_42fca0_7;

typedef struct st_42fca0_1 {
    unsigned int field_0;
} st_42fca0_1;

typedef struct st_42fca0_0 {
    char padding_0[8];
    struct st_42fca0_1 *field_8;
    char padding_c[40];
    struct st_42fca0_1 *field_34;
    struct st_42fca0_1 *field_38;
} st_42fca0_0;

typedef struct st_42fca0_4 {
    char padding_0[72];
    struct st_42fca0_1 *field_48;
} st_42fca0_4;

typedef struct st_42fca0_6 {
    struct st_42fca0_4 *field_0;
} st_42fca0_6;

extern int g_4a0a8c;
extern int g_4a0acc;
extern st_42fca0_6 *g_4ce8f0;
extern unsigned int g_4ce9e4;
extern unsigned int g_4cefb8;
extern void g_4cefbc;
extern unsigned int g_4cefbe;
extern unsigned int g_4cefc0;
extern unsigned int g_4cefc4;
extern unsigned int g_4cefc6;
extern unsigned short g_4cefc8;
extern int g_4cefcc;
extern unsigned int g_4cefd0;
extern char g_4cefd4;

int sub_42fca0(void)
{
    char *v4;  // eax
    char *v5;  // esi
    unsigned int v14;  // esi
    char *node;  // eax
    char i;  // 4102
    int j;  // edi
    unsigned int v9;  // ebp
    st_42fca0_7 *iter;  // ecx
    st_42fca0_7 *v11;  // eax
    unsigned int v12;  // esi
    unsigned int k;  // esi
    st_42fca0_0 **v0;  // [bp-0x10]
    char v1;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x8]

    v5 = v4;
    if (g_4cefb8)
    {
        do
        {
            Sleep(10);
        } while (g_4cefb8);
    }
    v0 = NULL;
    sub_4319e0("SnapShot! %s\n", v5);
    g_4ce8f0->field_0->field_48(g_4ce8f0, 0, 0, 0, &v0);
    *((unsigned int *)&g_4cefbc) = 0;
    g_4cefc0 = 0;
    g_4cefc4 = 0;
    g_4cefc8 = 0;
    g_4cefc6 = 54;
    g_4cefbe = 54;
    *((unsigned short *)&g_4cefbc) = 19778;
    node = v5;
    do
    {
        i = *(node);
        *(&g_4cefd4 - v5 + node) = *(node);
        node += 1;
    } while (i);
    if (g_4ce9e4 != 22)
    {
        if (g_4ce9e4 != 23)
        {
            sub_464220("error ? .\\src\\game\\mother.cpp\r\n");
            return;
        }
        sub_464220(&g_4a0a8c);
    }
    else
    {
        g_4cefcc = sub_46d04a(44);
        if (g_4cefcc)
        {
            sub_477420(g_4cefcc, 0, 44);
            g_4cefd0 = sub_46d04a(0xe1000);
            if (!g_4cefd0)
                goto LABEL_42fdac;
            g_4cefbe = g_4cefbe + 0xe1000;
            *((unsigned short *)(g_4cefcc + 14)) = 24;
            *((unsigned int *)g_4cefcc) = 40;
            *((unsigned int *)(g_4cefcc + 4)) = 640;
            *((unsigned int *)(g_4cefcc + 8)) = 480;
            *((unsigned short *)(g_4cefcc + 12)) = 1;
            *((unsigned int *)(g_4cefcc + 16)) = 0;
            *(v0)->field_34(v0, &v1, 0, 0);
            j = 479;
            v9 = 0;
            do
            {
                iter = g_4cefd0 + v9;
                v11 = j * v1 + v2;
                v12 = 640;
                do
                {
                    k = v12;
                    iter->field_0 = v11->field_0;
                    iter->field_2 = v11->field_2;
                    v11 = (char *)&v11[1].field_0 + 1;
                    iter += 1;
                    v14 = k - 1;
                    v12 = v14;
                } while (k != 1);
                j -= 1;
                v9 += 1920;
            } while (j > -0x1);
            *(v0)->field_38(v0);
            g_4cefb8 = sub_46db4a(sub_42fbb0, v14, v14);
        }
        else
        {
LABEL_42fdac:
            sub_464220(&g_4a0acc);
        }
    }
    if (v0)
        *(v0)->field_8(v0);
    return;
}
