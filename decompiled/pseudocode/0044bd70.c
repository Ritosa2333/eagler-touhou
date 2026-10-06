// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bd70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bd70_0 {
    char padding_0[4];
    HANDLE field_4;
    unsigned int field_8;
} st_44bd70_0;

extern char g_4a22dc;
extern char g_4a2304;

void sub_44bd70(st_44bd70_0 *idx)
{
    HANDLE v1;  // eax

    v1 = idx->field_4;
    *((char **)&idx->padding_0[0]) = &g_4a22dc;
    if (v1 != 0xffffffff)
    {
        CloseHandle(v1);
        idx->field_4 = 0xffffffff;
        idx->field_8 = 0;
    }
    *((char **)&idx->padding_0[0]) = &g_4a2304;
    return;
}
