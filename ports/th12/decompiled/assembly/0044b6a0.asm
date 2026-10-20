
// block 0044b6a0
0044b6a0  push       esi
0044b6a1  push       edi
0044b6a2  mov        esi, ecx
0044b6a4  mov        edi, eax
0044b6a6  call       0x44b710

// block 0044b6ab
0044b6ab  push       0x10
0044b6ad  call       0x46c9ea

// block 0044b6b2
0044b6b2  add        esp, 4
0044b6b5  test       eax, eax
0044b6b7  je         0x44b6d6

// block 0044b6b9
0044b6b9  mov        dword ptr [eax + 4], 0
0044b6c0  mov        dword ptr [eax + 8], 0
0044b6c7  mov        dword ptr [eax + 0xc], 0
0044b6ce  mov        dword ptr [eax], 0x4a241c
0044b6d4  jmp        0x44b6d8

// block 0044b6d6
0044b6d6  xor        eax, eax

// block 0044b6d8
0044b6d8  mov        dword ptr [esi + 0xc], eax
0044b6db  test       eax, eax
0044b6dd  je         0x44b700

// block 0044b6df
0044b6df  mov        edx, edi
0044b6e1  call       0x44b960

// block 0044b6e6
0044b6e6  test       al, al
0044b6e8  je         0x44b6fb

// block 0044b6ea
0044b6ea  call       0x44bc20

// block 0044b6ef
0044b6ef  mov        dword ptr [esi + 8], eax
0044b6f2  test       eax, eax
0044b6f4  je         0x44b6fb

// block 0044b6f6
0044b6f6  pop        edi
0044b6f7  mov        al, 1
0044b6f9  pop        esi
0044b6fa  ret        

// block 0044b6fb
0044b6fb  call       0x44b710

// block 0044b700
0044b700  pop        edi
0044b701  xor        al, al
0044b703  pop        esi
0044b704  ret        
