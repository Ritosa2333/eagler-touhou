
// block 004da2be
004da2be  add        byte ptr [edx - 0x6bf68e47], cl
004da2c4  and        ebx, dword ptr [ebx + edi*4 + 0x44]
004da2c8  jecxz      0x4da2b0

// block 004da2ca
004da2ca  sub        dword ptr [edx - 0x55], esp
004da2cd  mov        cl, 0x26
004da2cf  in         al, 0xf
004da2d1  popfd      
004da2d2  cdq        
004da2d3  cmp        al, 0xba
