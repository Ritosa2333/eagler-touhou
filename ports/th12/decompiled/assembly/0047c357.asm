
// block 0047c357
0047c357  mov        edi, edi
0047c359  push       ebp
0047c35a  mov        ebp, esp
0047c35c  mov        ecx, dword ptr [0x4ad138]
0047c362  mov        edx, dword ptr [ebp + 8]
0047c365  or         ecx, 1
0047c368  xor        eax, eax
0047c36a  cmp        dword ptr [0x4b42f4], ecx
0047c370  sete       al
0047c373  neg        edx
0047c375  sbb        edx, edx
0047c377  and        edx, ecx
0047c379  mov        dword ptr [0x4b42f4], edx
0047c37f  pop        ebp
0047c380  ret        
