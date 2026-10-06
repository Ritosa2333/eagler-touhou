// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48cf79; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ab0c0;

int sub_48cf79(void)
{
    void* idx;  // ebp
    unsigned int v2;  // eax
    unsigned int v3;  // eax

    sub_46fcec(&g_4ab0c0, 16);
    *((unsigned int *)((char *)idx - 32)) = 0;
    if (!(int)idx[8])
    {
        *((unsigned int *)sub_46e982()) = 0;
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        *((unsigned int *)((char *)idx - 28)) = sub_48eb61((int)idx[8]);
        sub_46ec9a(3);
        *((unsigned int *)((char *)idx - 4)) = 0;
        do
        {
            v2 = *((int *)((char *)idx - 28));
            *((unsigned int *)((char *)idx - 28)) = *((int *)((char *)idx - 28)) - 1;
            if (!v2)
                goto LABEL_48cffa;
        } while ((v3 = (unsigned int)(unsigned short)*((short *)(int)idx[8]), *((unsigned int *)&idx[8]) = (int)idx[8] + 2, (unsigned short)sub_48ceb4(v3) != 0xffff));
        *((unsigned int *)((char *)idx - 32)) = 0xffffffff;
LABEL_48cffa:
        *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
        sub_48d00f();
    }
    return sub_46fd31();
}
