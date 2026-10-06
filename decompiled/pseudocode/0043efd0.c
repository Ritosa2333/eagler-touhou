// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43efd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_43efd0(void)
{
    unsigned int v1;  // edi
    unsigned int v2;  // esi
    unsigned int v3;  // ebx

    sub_461970(*((int *)(v1 + v2 * 4 + 712)), v3);
    *((unsigned int *)(v1 + v2 * 4 + 712)) = 0;
    return;
}
