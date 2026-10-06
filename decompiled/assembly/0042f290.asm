
// block 0042f290
0042f290  sub        esp, 0x10
0042f293  cmp        dword ptr [0x4cea94], 0
0042f29a  je         0x42f314

// block 0042f29c
0042f29c  push       esi
0042f29d  mov        esi, dword ptr [0x4ce8cc]
0042f2a3  call       0x45a3c0

// block 0042f2a8
0042f2a8  mov        edx, dword ptr [0x4cea94]
0042f2ae  mov        eax, dword ptr [0x4ce8f0]
0042f2b3  mov        ecx, dword ptr [eax]
0042f2b5  push       edx
0042f2b6  push       0
0042f2b8  push       eax
0042f2b9  mov        eax, dword ptr [ecx + 0x94]
0042f2bf  call       eax

// block 0042f2c1
0042f2c1  fld1       
0042f2c3  mov        eax, dword ptr [0x4cede8]
0042f2c8  mov        edx, dword ptr [0x4cedf0]
0042f2ce  mov        ecx, dword ptr [0x4cedec]
0042f2d4  add        edx, eax
0042f2d6  mov        dword ptr [esp + 4], eax
0042f2da  mov        eax, dword ptr [0x4cedf4]
0042f2df  add        eax, ecx
0042f2e1  mov        dword ptr [esp + 0x10], eax
0042f2e5  mov        eax, dword ptr [0x4ce8f0]
0042f2ea  push       0
0042f2ec  mov        dword ptr [esp + 0x10], edx
0042f2f0  mov        edx, dword ptr [0x4cf2a8]
0042f2f6  mov        dword ptr [esp + 0xc], ecx
0042f2fa  mov        ecx, dword ptr [eax]
0042f2fc  push       ecx
0042f2fd  fstp       dword ptr [esp]
0042f300  push       edx
0042f301  push       3
0042f303  lea        edx, [esp + 0x14]
0042f307  push       edx
0042f308  push       1
0042f30a  push       eax
0042f30b  mov        eax, dword ptr [ecx + 0xac]
0042f311  call       eax

// block 0042f313
0042f313  pop        esi

// block 0042f314
0042f314  mov        eax, 1
0042f319  add        esp, 0x10
0042f31c  ret        
