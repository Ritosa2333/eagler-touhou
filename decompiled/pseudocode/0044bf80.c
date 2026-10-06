// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bf80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bf80_0 {
    char padding_0[4];
    HANDLE field_4;
} st_44bf80_0;

char sub_44bf80(st_44bf80_0 *a0, unsigned int a1, int a2, enum SET_FILE_POINTER_MOVE_METHOD a3)
{
    if (a0->field_4 != 0xffffffff)
    {
        SetFilePointer(a0->field_4, a2, NULL, a3);
        return 1;
    }
    return 0;
}
