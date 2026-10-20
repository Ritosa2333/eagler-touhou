// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466b10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466b10_0 {
    char padding_0[16];
    unsigned int field_10;
    char padding_14[8];
    int field_1c;
} st_466b10_0;

extern unsigned int g_4d4760;

unsigned int sub_466b10(void)
{
    unsigned int v1;  // edi
    HANDLE v2;  // eax
    void* idx;  // esi
    unsigned int v4;  // 4098
    st_466b10_0 *v5;  // ebx
    unsigned int v6;  // ecx

    if (!v1)
        return 2147942487;
    sub_466e90("Streaming File Open %s\r\n", v1);
    v2 = CreateFileA(v1, 0x80000000, FILE_SHARE_READ, NULL, OPEN_EXISTING, 134217856, NULL);
    *((HANDLE *)&idx[140]) = v2;
    if (v2 == 0xffffffff)
        return 0x80004005;
    v4 = (int)idx[124];
    *((st_466b10_0 **)&idx[144]) = v5;
    *((unsigned int *)&idx[148]) = v1;
    if (v4)
    {
        *((int *)&idx[132]) = (int)idx[128];
        if (v5->field_1c > 0)
        {
            v6 = (int)idx[8];
            *((int *)&idx[0x88]) = v5->field_1c;
            *((unsigned int *)&idx[44]) = v6;
            return 0;
        }
    }
    else if (v2)
    {
        SetFilePointer(v2, v5->field_10 + g_4d4760, NULL, FILE_BEGIN);
        *((int *)&idx[8]) = *((int *)((int)idx[144] + 28));
    }
    *((int *)&idx[44]) = (int)idx[8];
    return 0;
}
