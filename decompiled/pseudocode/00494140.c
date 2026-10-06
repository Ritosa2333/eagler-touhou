// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x494140; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_494140_0 {
    char padding_0[14];
    char field_e;
} st_494140_0;

extern char g_4b35dc;

void sub_494140(unsigned int a0, st_494140_0 *a1)
{
    void* v1;  // ebp
    unsigned int v3;  // 4155
    unsigned short v4;  // ftop
    unsigned int v5;  // eax

    *((unsigned short *)((char *)v1 - 162)) = _INSERT(*((short *)((char *)v1 - 164)), 0, 63);
    v3 = _ccall(/* unsupported instruction */, (long long)/* unsupported instruction */);
    *((st_494140_0 **)((char *)v1 - 148)) = a1;
    *((unsigned short *)((char *)v1 - 160)) = (v4 & 7) * 0x800 | (unsigned short)v3 & 0x4700;
    *((char *)v1 - 144) = 0;
    v5 = (&g_4b35dc)[__ROL__(*((char *)v1 - 159) * 2 >> 1, 1) & 15];
    goto *((void *)(*((int *)&a1[1].padding_0[1 + v5])));
}
