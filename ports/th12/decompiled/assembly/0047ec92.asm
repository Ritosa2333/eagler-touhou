
// block 0047ec92
0047ec92  mov        edi, edi
0047ec94  push       ebp
0047ec95  mov        ebp, esp
0047ec97  mov        eax, dword ptr [ecx]
0047ec99  push       esi
0047ec9a  mov        esi, dword ptr [ebp + 8]
0047ec9d  push       dword ptr [ebp + 0xc]
0047eca0  mov        dword ptr [esi], eax
0047eca2  mov        eax, dword ptr [ecx + 4]
0047eca5  mov        ecx, esi
0047eca7  mov        dword ptr [esi + 4], eax
0047ecaa  call       0x47ea48

// block 0047ecaf
0047ecaf  mov        eax, esi
0047ecb1  pop        esi
0047ecb2  pop        ebp
0047ecb3  ret        8
