
// block 004db222
004db222  out        0x6f, eax
004db224  fidivr     dword ptr [eax + ebp*8 + 0x1f]
004db228  cmc        
004db229  add        bh, byte ptr [edi + 0x6c]
004db22c  adc        dword ptr [edi], edx
