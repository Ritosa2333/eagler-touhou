// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464d30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464d30_0 {
    char padding_0[4];
    HANDLE field_4;
} st_464d30_0;

int sub_464d30(void)
{
    st_464d30_0 *v1;  // eax

    if (v1->field_4)
        ResumeThread(v1->field_4);
    return;
}
