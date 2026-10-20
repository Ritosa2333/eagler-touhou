// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406530; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ce8cc;

int sub_406530(void)
{
    *((unsigned int *)(g_4ce8cc + 8973648)) = 0;
    *((unsigned int *)(g_4ce8cc + 8973644)) = 0x80808080;
    return g_4ce8cc;
}
