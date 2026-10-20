// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bf40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bf40_0 {
    char padding_0[4];
    HANDLE field_4;
} st_44bf40_0;

unsigned int sub_44bf40(st_44bf40_0 *a0)
{
    if (a0->field_4 != 0xffffffff)
        return SetFilePointer(a0->field_4, 0, NULL, FILE_CURRENT);
    return 0;
}
