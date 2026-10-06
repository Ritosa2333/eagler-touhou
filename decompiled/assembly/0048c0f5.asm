
// block 0048c0f5
0048c0f5  push       0xc
0048c0f7  push       0x4ab038
0048c0fc  call       0x46fcec

// block 0048c101
0048c101  xor        eax, eax
0048c103  xor        esi, esi
0048c105  cmp        dword ptr [ebp + 0xc], esi
0048c108  setne      al
0048c10b  cmp        eax, esi
0048c10d  jne        0x48c12c

// block 0048c10f
0048c10f  call       0x46e96f

// block 0048c114
0048c114  mov        dword ptr [eax], 0x16
0048c11a  push       esi
0048c11b  push       esi
0048c11c  push       esi
0048c11d  push       esi
0048c11e  push       esi
0048c11f  call       0x470f2d

// block 0048c124
0048c124  add        esp, 0x14
0048c127  or         eax, 0xffffffff
0048c12a  jmp        0x48c157

// block 0048c12c
0048c12c  push       dword ptr [ebp + 0xc]
0048c12f  call       0x47c0fc

// block 0048c134
0048c134  pop        ecx
0048c135  mov        dword ptr [ebp - 4], esi
0048c138  push       dword ptr [ebp + 0xc]
0048c13b  push       dword ptr [ebp + 8]
0048c13e  call       0x48c004

// block 0048c143
0048c143  pop        ecx
0048c144  pop        ecx
0048c145  mov        dword ptr [ebp - 0x1c], eax
0048c148  mov        dword ptr [ebp - 4], 0xfffffffe
0048c14f  call       0x48c15d

// block 0048c154
0048c154  mov        eax, dword ptr [ebp - 0x1c]

// block 0048c157
0048c157  call       0x46fd31

// block 0048c15c
0048c15c  ret        
