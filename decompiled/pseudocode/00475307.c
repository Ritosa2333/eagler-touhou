// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x475307; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_475307_0 {
    char padding_0[20];
    unsigned int field_14;
    char padding_18[80];
    int field_68;
    int field_6c;
    unsigned int field_70;
    char padding_74[84];
    char field_c8;
    char padding_c9[130];
    char field_14b;
    char padding_14c[172];
    void* field_1f8;
    void* field_1fc;
} st_475307_0;

extern WCHAR g_49d51c;
extern char g_49d5c8;
extern int g_4aad28;
extern int g_4ad4b0;
extern unsigned int g_4adab8;

void sub_475307(unsigned int a0, unsigned int a1)
{
    HMODULE v0;  // eax
    void* index;  // ebp
    st_475307_0 *idx;  // esi
    unsigned int v3;  // eax

    sub_46fcec(&g_4aad28, 12);
    v0 = GetModuleHandleW(&g_49d51c);
    if (!v0)
        v0 = sub_472bdd(&g_49d51c);
    *((HMODULE *)((char *)index - 28)) = v0;
    idx = (int)index[8];
    *((char **)&idx->padding_18[0x44]) = &g_49d5c8;
    idx->field_14 = 1;
    if (v0)
    {
        idx->field_1f8 = GetProcAddress(v0, "EncodePointer");
        idx->field_1fc = GetProcAddress(*((int *)((char *)index - 28)), "DecodePointer");
    }
    idx->field_70 = 1;
    idx->field_c8 = 67;
    idx->field_14b = 67;
    idx->field_68 = &g_4ad4b0;
    sub_46ec9a(13);
    *((unsigned int *)((char *)index - 4)) = 0;
    InterlockedIncrement(idx->field_68);
    *((unsigned int *)((char *)index - 4)) = 0xfffffffe;
    sub_4753dc(&g_4aad28);
    sub_46ec9a(12);
    *((unsigned int *)((char *)index - 4)) = 1;
    v3 = (int)index[12];
    idx->field_6c = v3;
    if (!v3)
        idx->field_6c = g_4adab8;
    sub_473ff5(idx->field_6c);
    *((unsigned int *)((char *)index - 4)) = 0xfffffffe;
    sub_4753e5();
    sub_46fd31();
    return;
}
