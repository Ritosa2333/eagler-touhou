
// block 004d9fcb
004d9fcb  xor        dh, byte ptr [edi + 0x7a]
004d9fce  lahf       
004d9fcf  ror        cl, 0xbe
004d9fd2  scasb      al, byte ptr es:[edi]
004d9fd3  mov        word ptr [edi], fs
