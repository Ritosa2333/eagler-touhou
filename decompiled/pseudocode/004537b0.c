// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4537b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4537b0_0 {
    char padding_0[36];
    unsigned int field_24;
    char padding_28[4];
    unsigned short field_2c;
} st_4537b0_0;

extern char g_49b67c;
extern void* g_4ce940;

int sub_4537b0(void)
{
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    unsigned int v14;  // edx
    char *iter;  // eax
    unsigned int v7;  // edi
    char i;  // 4102
    void* idx;  // eax
    st_4537b0_0 *v10;  // ebx
    unsigned int v11;  // ebp
    unsigned int v12;  // edi
    HANDLE count;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    v2 = v4;
    v1 = v5;
    iter = "thbgm.da";
    v0 = v7;
    do
    {
        i = *(iter);
        *(idx - &g_49b67c + iter) = *(iter);
        iter += 1;
    } while (i);
    if (!(int)idx[16])
        return;
    if (!*((int *)idx))
        return;
    sub_453c30();
    v10 = (int)idx[6532];
    v11 = v10->field_2c;
    v12 = v11 * v10->field_24;
    *((HANDLE *)&idx[21104]) = CreateEventA(NULL, 0, 0, NULL);
    count = CreateThread(NULL, NULL, sub_4548a0, g_4ce940, THREAD_CREATE_RUN_IMMEDIATELY, idx + 20);
    v14 = (int)idx[21104];
    *((HANDLE *)&idx[24]) = count;
    sub_4657a0((int)idx[16], 0, 0, 0, 0, (v12 * 4 >> 4) - (v12 * 4 >> 4) % v11, v14, v10);
    return;
}
