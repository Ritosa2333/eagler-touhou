// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d260; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void* sub_46d260(void* a0, char i)
{
    void* v0;  // edi

    do
    {
        if (0)
            break;
    } while (*((char *)a0));
    v0 = a0 - 1;
    do
    {
        if (0)
            break;
    } while (i != *((char *)v0));
    if ((char)v0[1] == i)
        return v0 + 1;
    return NULL;
}
