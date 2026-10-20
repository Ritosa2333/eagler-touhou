// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453c30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_453c30_2 {
    char padding_0[20];
    unsigned int field_14;
    HANDLE field_18;
    char padding_1c[21072];
    struct st_453c30_1 *field_526c;
    HANDLE field_5270;
} st_453c30_2;

typedef struct st_453c30_1 {
    struct st_453c30_0 *field_0;
} st_453c30_1;

typedef struct st_453c30_0 {
    unsigned int field_0;
} st_453c30_0;

void sub_453c30(void)
{
    st_453c30_2 *idx;  // esi
    unsigned int v3;  // ebx
    unsigned int v0;  // [bp-0x8]

    if (!idx->field_526c)
        return;
    sub_466460(1);
    if (idx->field_18)
    {
        v0 = v3;
        PostThreadMessageA(idx->field_14, 18, NULL, NULL);
        if (WaitForSingleObject(idx->field_18, 0x100))
        {
            do
            {
                PostThreadMessageA(idx->field_14, 18, NULL, NULL);
            } while (WaitForSingleObject(idx->field_18, 0x100));
        }
        CloseHandle(idx->field_18);
        CloseHandle(idx->field_5270);
        idx->field_18 = NULL;
    }
    if (!idx->field_526c)
        return;
    idx->field_526c->field_0->field_0(1);
    idx->field_526c = NULL;
    return;
}
