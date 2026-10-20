// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e5e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e5e0_2 {
    char padding_0[1772];
    int field_6ec;
} st_40e5e0_2;

typedef struct st_40e5e0_3 {
    char padding_0[109220];
    int field_1aaa4;
} st_40e5e0_3;

typedef struct st_40e5e0_1 {
    char padding_0[16];
    unsigned int field_10;
} st_40e5e0_1;

typedef struct st_40e5e0_0 {
    char padding_0[13756];
    unsigned int field_35bc;
} st_40e5e0_0;

extern int g_4b0c44;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern st_40e5e0_0 *g_4b43c0;
extern void* g_4b43cc;
extern st_40e5e0_1 *g_4b4518;
extern unsigned int g_4b451c;

void sub_40e5e0(void)
{
    unsigned int v4;  // edx
    st_40e5e0_2 *idx;  // eax
    st_40e5e0_3 *index;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    if (!((char)g_4b43cc[124] & 1))
        return;
    g_4b43c0->field_35bc = g_4b43c0->field_35bc | 1;
    sub_461970((int)g_4b43cc[20], v0);
    sub_461970((int)g_4b43cc[24], v1);
    sub_461970((int)g_4b43cc[28], v2);
    *((unsigned int *)&g_4b43cc[124]) = (int)g_4b43cc[124] & 0xfffffffe;
    sub_461a70((int)g_4b43cc[16]);
    *((int *)&g_4b43cc[16]) = 0;
    *((unsigned int *)&g_4b43cc[124]) = (int)g_4b43cc[124] & 0xffffffdf;
    sub_461a70((int)g_4b43cc[32]);
    *((int *)&g_4b43cc[32]) = 0;
    if (!((char)g_4b43cc[124] & 2))
    {
        sub_420f90(0);
        return;
    }
    v4 = 1717986919 * (int)g_4b43cc[128] >> 0x22;
    g_4b0c44 = g_4b0c44 + (v4 >> 31) + v4;
    if (g_4b0c44 >= 1000000000)
        g_4b0c44 = 0x3b9ac9ff;
    sub_420f90((int)g_4b43cc[128]);
    if (g_4b4518->field_10 != 1)
    {
        idx = (int)g_4b43cc[120] * 144 + (g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + 1644;
        if (*((int *)&idx->padding_0[128]) < 99999)
            *((unsigned int *)&idx->padding_0[128]) = *((int *)&idx->padding_0[128]) + 1;
        index = (int)g_4b43cc[120] * 144 + g_4b451c + 109092;
        if (*((int *)&index->padding_0[128]) < 99999)
            *((unsigned int *)&index->padding_0[128]) = *((int *)&index->padding_0[128]) + 1;
    }
    sub_453d90();
    return;
}
