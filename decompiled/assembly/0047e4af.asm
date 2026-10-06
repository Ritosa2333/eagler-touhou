
// block 0047e4af
0047e4af  mov        edi, edi
0047e4b1  push       ebp
0047e4b2  mov        ebp, esp
0047e4b4  push       esi
0047e4b5  mov        esi, ecx
0047e4b7  cmp        byte ptr [esi + 4], 1
0047e4bb  jg         0x47e4ea

// block 0047e4bd
0047e4bd  cmp        dword ptr [esi], 0
0047e4c0  mov        eax, dword ptr [ebp + 8]
0047e4c3  je         0x47e4e4

// block 0047e4c5
0047e4c5  cmp        eax, 2
0047e4c8  je         0x47e4e4

// block 0047e4ca
0047e4ca  cmp        eax, 3
0047e4cd  je         0x47e4e4

// block 0047e4cf
0047e4cf  test       eax, eax
0047e4d1  je         0x47e4ea

// block 0047e4d3
0047e4d3  push       eax
0047e4d4  call       0x47deeb

// block 0047e4d9
0047e4d9  pop        ecx
0047e4da  push       eax
0047e4db  mov        ecx, esi
0047e4dd  call       0x47e26b

// block 0047e4e2
0047e4e2  jmp        0x47e4ea

// block 0047e4e4
0047e4e4  push       eax
0047e4e5  call       0x47e2f7

// block 0047e4ea
0047e4ea  mov        eax, esi
0047e4ec  pop        esi
0047e4ed  pop        ebp
0047e4ee  ret        4
