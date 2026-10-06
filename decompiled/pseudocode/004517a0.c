// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4517a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4517a0_0 {
    struct st_4517a0_1 *field_0;
    char padding_4[4];
    struct st_4517a0_1 *field_8;
    struct st_4517a0_1 *field_c;
} st_4517a0_0;

typedef struct st_4517a0_4 {
    char padding_0[8];
    struct st_4517a0_1 *field_8;
    char padding_c[8];
    struct st_4517a0_1 *field_14;
} st_4517a0_4;

typedef struct st_4517a0_1 {
    unsigned int field_0;
} st_4517a0_1;

typedef struct Guid {
    unsigned int Data1;
    unsigned short Data2;
    unsigned short Data3;
    char Data4[8];
} Guid;

extern Guid g_499c6c;
extern Guid g_49b88c;
extern char g_49bcfc;

int sub_4517a0(LPArray a0)
{
    unsigned int v3;  // edi
    unsigned int v4;  // eax
    unsigned int v5;  // esi
    unsigned int v6;  // eax
    st_4517a0_0 **v0;  // [bp-0x150]
    st_4517a0_4 **v1;  // [bp-0x14c], Other Possible Types: char

    if (!v3)
        return v4;
    CoInitialize(NULL);
    if (CoCreateInstance(&g_49b88c.Data1, 0, CLSCTX_INPROC_SERVER, &g_499c6c.Data1, &v0) >= 0)
    {
        if (*(v0)->field_0(v0, &g_49bcfc, &v1) >= 0)
        {
            v5 = sub_46c9ea(-0x1);
            MultiByteToWideChar(0, 0, a0, -0x1, v5, 260);
            sub_46ca4f(v5);
            *(v1)->field_8(v1);
        }
        *(v0)->field_8(v0);
    }
    CoUninitialize();
    return v6;
}
