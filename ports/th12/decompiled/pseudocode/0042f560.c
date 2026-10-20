// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f560; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4027e0_0 {
    char padding_0[120];
    unsigned int field_78;
    char padding_7c[96];
    unsigned int field_dc;
    char padding_e0[72];
    unsigned int field_128;
    char padding_12c[40];
    unsigned int field_154;
    char padding_158[72];
    unsigned int field_1a0;
    char padding_1a4[56];
    unsigned int field_1dc;
    char padding_1e0[56];
    unsigned int field_218;
    char padding_21c[72];
    unsigned int field_264;
    char padding_268[40];
    unsigned int field_290;
    char padding_294[40];
    unsigned int field_2bc;
    char padding_2c0[40];
    unsigned int field_2e8;
    char padding_2ec[236];
    unsigned int field_3d8;
    char padding_3dc[8];
    unsigned short field_3e4;
} st_4027e0_0;

extern unsigned int g_4b2ed0;
extern unsigned int g_4b4508;
extern unsigned int g_4b450c;
extern unsigned short g_4ce560;
extern unsigned short g_4ce568;
extern unsigned int g_4ceaa4;
extern unsigned int g_4ceaa8;
extern unsigned int g_4ceaac;
extern unsigned int g_4cee7c;
extern unsigned int g_4cf2a8;
extern void g_4cf4e8;
extern unsigned int g_4d4768;

unsigned int sub_42f560(void)
{
    unsigned int t;  // eax
    st_4027e0_0 *v7;  // eax
    st_4027e0_0 *v9;  // eax
    st_4027e0_0 *v11;  // eax
    int v0;  // [bp-0x4], Other Possible Types: unsigned int
    int v1;  // [bp+0x0]
    int v2;  // [bp+0x4]
    int v3;  // [bp+0x8]
    int v4;  // [bp+0xc]
    int v5;  // [bp+0x10]

    sub_42f4b0(v0);
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4cf2a8 = 0xff000000;
    sub_430be0(v0);
    t = timeGetTime();
    g_4cee7c = t;
    g_4ce568 = t;
    g_4ce560 = t;
    g_4d4768 = CreateThread(NULL, NULL, sub_453460, &g_4cf4e8, THREAD_CREATE_RUN_IMMEDIATELY, &v0);
    sub_41cad0(v0, v1, v2, v3, v4, v5);
    sub_45eb30();
    sub_44e4d0();
    v7 = sub_46c9ea(1204);
    g_4ceaa4 = (!v7 ? 0 : (unsigned int)sub_4027e0(v7));
    v9 = sub_46c9ea(1204);
    g_4ceaa8 = (!v9 ? 0 : (unsigned int)sub_4027e0(v9));
    v11 = sub_46c9ea(1204);
    g_4ceaac = (!v11 ? 0 : (unsigned int)sub_4027e0(v11));
    g_4b450c = 0;
    g_4b4508 = 0;
    return 0;
}
