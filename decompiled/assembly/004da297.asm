
// block 004da297
004da297  mov        bl, 0x74
004da299  js         0x4da233

// block 004da29b
004da29b  push       ds
004da29c  sbb        eax, 0x53552860
004da2a1  aad        0xb3
004da2a3  adc        ch, byte ptr [edx + 0x31b57771]
004da2aa  xor        byte ptr [ecx - 0x331cb03b], dl
004da2b0  push       ecx
004da2b1  test       eax, 0xba9d1fb8
004da2b6  in         al, dx
004da2b7  rcr        byte ptr [eax], 0xa8
004da2ba  mov        dl, 0xd3
004da2bc  push       ebx
004da2bd  iretd      
