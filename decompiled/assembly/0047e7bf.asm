
// block 0047e7bf
0047e7bf  mov        edi, edi
0047e7c1  push       ebp
0047e7c2  mov        ebp, esp
0047e7c4  push       esi
0047e7c5  mov        esi, ecx
0047e7c7  cmp        byte ptr [esi + 4], 1
0047e7cb  jg         0x47e7fb

// block 0047e7cd
0047e7cd  mov        eax, dword ptr [ebp + 8]
0047e7d0  mov        ecx, dword ptr [eax]
0047e7d2  test       ecx, ecx
0047e7d4  jne        0x47e7e4

// block 0047e7d6
0047e7d6  movsx      eax, byte ptr [eax + 4]
0047e7da  push       eax
0047e7db  mov        ecx, esi
0047e7dd  call       0x47e4af

// block 0047e7e2
0047e7e2  jmp        0x47e7fb

// block 0047e7e4
0047e7e4  cmp        dword ptr [esi], 0
0047e7e7  jne        0x47e7f3

// block 0047e7e9
0047e7e9  push       eax
0047e7ea  mov        ecx, esi
0047e7ec  call       0x47dde0

// block 0047e7f1
0047e7f1  jmp        0x47e7fb

// block 0047e7f3
0047e7f3  push       ecx
0047e7f4  mov        ecx, esi
0047e7f6  call       0x47e26b

// block 0047e7fb
0047e7fb  mov        eax, esi
0047e7fd  pop        esi
0047e7fe  pop        ebp
0047e7ff  ret        4
