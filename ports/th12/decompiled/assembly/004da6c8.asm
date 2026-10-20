
// block 004da6c8
004da6c8  xchg       ecx, eax
004da6c9  out        dx, al
004da6ca  jle        0x4da66a

// block 004da6cc
004da6cc  in         eax, dx
004da6cd  cmc        
004da6ce  out        dx, al
004da6cf  mov        al, 0xff
004da6d1  adc        dword ptr [esi + 0x3b], edi
004da6d4  fidivr     word ptr [ebx - 0x4a421fbd]
004da6da  out        0x66, eax
004da6dc  push       es
004da6dd  jp         0x4da745

// block 004da6df
004da6df  sbb        eax, 0x8f2c6ff5
004da6e4  add        byte ptr [edx + ebx*2 + 0x64], al
