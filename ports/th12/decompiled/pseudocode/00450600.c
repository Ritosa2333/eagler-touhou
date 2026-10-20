// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x450600; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_450600_6 {
    char padding_0[20];
    char field_14;
} st_450600_6;

typedef struct st_450600_1 {
    unsigned int field_0;
} st_450600_1;

typedef struct st_450600_0 {
    char padding_0[164];
    struct st_450600_1 *field_a4;
    struct st_450600_1 *field_a8;
    char padding_ac[16];
    struct st_450600_1 *field_bc;
    char padding_c0[68];
    struct st_450600_1 *field_104;
} st_450600_0;

typedef struct st_450600_5 {
    struct st_450600_0 *field_0;
} st_450600_5;

extern st_450600_5 *g_4ce8f0;
extern char g_4ceace;
extern char g_4cec04;
extern unsigned int g_4cee34;
extern unsigned int g_4cee38;
extern unsigned int g_4cf278;
extern unsigned long long g_4cf2a0;

unsigned int sub_450600(st_450600_6 *idx)
{
    unsigned int v4;  // eax
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    int v3;  // [bp-0x4]

    sub_45a3c0(v0, v1, v2, v3);
    g_4cee34 = &g_4cec04;
    sub_430910(v0, v1, v2, v3);
    g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
    g_4cee38 = 1;
    v4 = sub_4624c0(v0, v1);
    if (!v4)
    {
        sub_464c40();
        return 1;
    }
    else if (v4 == 0xffffffff)
    {
        sub_464c40();
        return 2;
    }
    else
    {
        idx->field_14 = idx->field_14 + 1;
        if (g_4ceace + 1 <= idx->field_14)
        {
            g_4ce8f0->field_0->field_a4(g_4ce8f0);
            sub_45a380(v0);
            g_4cf278 = 0xff;
            sub_430300();
            sub_462620();
            sub_45a3c0();
            g_4ce8f0->field_0->field_104(g_4ce8f0, 0, 0);
            g_4ce8f0->field_0->field_a8(g_4ce8f0);
            idx->field_14 = 0;
            sub_450720(v0);
        }
        sub_4508b0();
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (!/* unsupported instruction */)
        {
            g_4cf2a0 = nan;
            /* unsupported instruction */
            return 0;
        }
        g_4cf2a0 = /* unsupported instruction */;
        /* unsupported instruction */
        return 0;
    }
}
