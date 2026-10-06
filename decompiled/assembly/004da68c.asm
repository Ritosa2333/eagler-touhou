
// block 004da68c
004da68c  pop        ds
004da68d  cld        
004da68e  or         byte ptr [esi + 0x15], cl
004da691  scasb      al, byte ptr es:[edi]
004da692  loope      0x4da667

// block 004da694
004da694  dec        ebx
004da695  mov        eax, dword ptr [0xa66715f7]
004da69a  pop        edi
004da69b  imul       ebp, dword ptr [edi], 0xeffeb31d
004da6a1  adc        ecx, dword ptr [ecx + 0x73]
004da6a4  push       edi
