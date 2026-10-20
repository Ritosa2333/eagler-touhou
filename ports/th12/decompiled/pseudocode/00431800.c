// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x431800; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_431800_0 {
    char padding_0[1424];
    unsigned int field_590;
} st_431800_0;

void sub_431800(void)
{
    st_431800_0 *v1;  // edi
    unsigned int v2;  // esi

    if (v1->field_590 & 0x8000)
    {
        LeaveCriticalSection(&v1[1].padding_0[636 + 24 * v2]);
        v1[1].padding_0[924 + v2] = v1[1].padding_0[924 + v2] - 1;
    }
    return;
}
