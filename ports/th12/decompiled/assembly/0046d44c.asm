
// block 0046d44c
0046d44c  mov        edi, edi
0046d44e  push       ebp
0046d44f  mov        ebp, esp
0046d451  sub        esp, 0x28
0046d454  push       ebx
0046d455  push       esi
0046d456  push       dword ptr [ebp + 0xc]
0046d459  lea        ecx, [ebp - 0x10]
0046d45c  call       0x46d3b4

// block 0046d461
0046d461  mov        esi, dword ptr [ebp + 8]
0046d464  xor        ebx, ebx
0046d466  cmp        esi, ebx
0046d468  jne        0x46d492

// block 0046d46a
0046d46a  call       0x46e96f

// block 0046d46f
0046d46f  push       ebx
0046d470  push       ebx
0046d471  push       ebx
0046d472  push       ebx
0046d473  push       ebx
0046d474  mov        dword ptr [eax], 0x16
0046d47a  call       0x470f2d

// block 0046d47f
0046d47f  add        esp, 0x14
0046d482  cmp        byte ptr [ebp - 4], bl
0046d485  je         0x46d48e

// block 0046d487
0046d487  mov        eax, dword ptr [ebp - 8]
0046d48a  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0046d48e
0046d48e  fldz       
0046d490  jmp        0x46d4f3

// block 0046d492
0046d492  mov        eax, dword ptr [ebp - 0x10]
0046d495  cmp        dword ptr [eax + 0xac], 1
0046d49c  jle        0x46d4b2

// block 0046d49e
0046d49e  lea        eax, [ebp - 0x10]
0046d4a1  push       eax
0046d4a2  movzx      eax, byte ptr [esi]
0046d4a5  push       8
0046d4a7  push       eax
0046d4a8  call       0x4758eb

// block 0046d4ad
0046d4ad  add        esp, 0xc
0046d4b0  jmp        0x46d4c2

// block 0046d4b2
0046d4b2  movzx      ecx, byte ptr [esi]
0046d4b5  mov        eax, dword ptr [eax + 0xc8]
0046d4bb  movzx      eax, word ptr [eax + ecx*2]
0046d4bf  and        eax, 8

// block 0046d4c2
0046d4c2  cmp        eax, ebx
0046d4c4  je         0x46d4c9

// block 0046d4c6
0046d4c6  inc        esi
0046d4c7  jmp        0x46d492

// block 0046d4c9
0046d4c9  lea        eax, [ebp - 0x10]
0046d4cc  push       eax
0046d4cd  push       ebx
0046d4ce  push       ebx
0046d4cf  push       esi
0046d4d0  call       0x475860

// block 0046d4d5
0046d4d5  pop        ecx
0046d4d6  push       eax
0046d4d7  lea        eax, [ebp - 0x28]
0046d4da  push       esi
0046d4db  push       eax
0046d4dc  call       0x4757b7

// block 0046d4e1
0046d4e1  fld        qword ptr [eax + 0x10]
0046d4e4  add        esp, 0x18
0046d4e7  cmp        byte ptr [ebp - 4], bl
0046d4ea  je         0x46d4f3

// block 0046d4ec
0046d4ec  mov        eax, dword ptr [ebp - 8]
0046d4ef  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0046d4f3
0046d4f3  pop        esi
0046d4f4  pop        ebx
0046d4f5  leave      
0046d4f6  ret        
