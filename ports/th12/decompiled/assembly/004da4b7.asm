
// block 004da4b7
004da4b7  jbe        0x4da45f

// block 004da4b9
004da4b9  xor        bh, dh
004da4bb  cmp        byte ptr [ebp + 0xc6ce610], ah
004da4c1  lahf       
004da4c2  fsub       st(2), st(0)
004da4c4  movsb      byte ptr es:[edi], byte ptr [esi]
004da4c5  shl        esp, cl
004da4c7  and        ah, byte ptr [ebp - 0x14ed9bb2]
