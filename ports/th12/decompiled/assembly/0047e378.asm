
// block 0047e378
0047e378  mov        edi, edi
0047e37a  push       ebp
0047e37b  mov        ebp, esp
0047e37d  mov        eax, dword ptr [ebp + 0xc]
0047e380  cmp        eax, 9
0047e383  ja         0x47e3a7

// block 0047e385
0047e385  mov        edx, dword ptr [ecx]
0047e387  cmp        edx, -1
0047e38a  je         0x47e3a3

// block 0047e38c
0047e38c  cmp        eax, edx
0047e38e  jg         0x47e3a3

// block 0047e390
0047e390  mov        ecx, dword ptr [ecx + eax*4 + 4]
0047e394  mov        edx, dword ptr [ecx]
0047e396  mov        eax, dword ptr [ebp + 8]
0047e399  mov        dword ptr [eax], edx
0047e39b  mov        ecx, dword ptr [ecx + 4]
0047e39e  mov        dword ptr [eax + 4], ecx
0047e3a1  jmp        0x47e3b4

// block 0047e3a3
0047e3a3  push       2
0047e3a5  jmp        0x47e3a9

// block 0047e3a7
0047e3a7  push       3

// block 0047e3a9
0047e3a9  mov        ecx, dword ptr [ebp + 8]
0047e3ac  call       0x47e1ce

// block 0047e3b1
0047e3b1  mov        eax, dword ptr [ebp + 8]

// block 0047e3b4
0047e3b4  pop        ebp
0047e3b5  ret        8
