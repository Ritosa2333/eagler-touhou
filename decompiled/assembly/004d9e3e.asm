
// block 004d9e3e
004d9e3e  sub        al, 0x68
004d9e40  adc        dword ptr [edi], ecx
004d9e42  and        dh, byte ptr [edi + 0x38]
004d9e45  jbe        0x4d9e54

// block 004d9e47
004d9e47  das        
004d9e48  jne        0x4d9e9c

// block 004d9e54
004d9e54  clc        
004d9e55  pop        edi
004d9e56  or         dword ptr [edx], edx
004d9e58  inc        esi
004d9e59  mov        ebx, 0xb69a39c9
004d9e5e  out        0x65, eax
004d9e60  mov        byte ptr [0x61e8dd09], al
004d9e65  popal      
004d9e66  dec        esi
004d9e67  sbb        eax, 0x60616a93
004d9e6c  sal        byte ptr [edi + 0x46], 1
004d9e6f  pop        edi
004d9e70  and        eax, 0x53a6ce31
004d9e75  mov        eax, 0xe1c505ce
004d9e7a  mov        ecx, 0x9821e3b6
004d9e7f  ret        0x17b5

// block 004d9e9c
004d9e9c  fmul       st(6), st(0)
004d9e9e  xor        byte ptr [edx + 0x5751fbc7], dh
