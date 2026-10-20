
// block 0047e2aa
0047e2aa  mov        edi, edi
0047e2ac  push       ebp
0047e2ad  mov        ebp, esp
0047e2af  push       esi
0047e2b0  mov        esi, ecx
0047e2b2  and        dword ptr [esi], 0
0047e2b5  mov        byte ptr [esi + 4], 0
0047e2b9  and        dword ptr [esi + 4], 0xffff00ff
0047e2c0  cmp        dword ptr [ebp + 8], 0
0047e2c4  je         0x47e2ec

// block 0047e2c6
0047e2c6  push       0
0047e2c8  push       8
0047e2ca  mov        ecx, 0x4b42f8
0047e2cf  call       0x47dc29

// block 0047e2d4
0047e2d4  test       eax, eax
0047e2d6  je         0x47e2e4

// block 0047e2d8
0047e2d8  push       dword ptr [ebp + 8]
0047e2db  mov        ecx, eax
0047e2dd  call       0x47de46

// block 0047e2e2
0047e2e2  jmp        0x47e2e6

// block 0047e2e4
0047e2e4  xor        eax, eax

// block 0047e2e6
0047e2e6  mov        dword ptr [esi], eax
0047e2e8  test       eax, eax
0047e2ea  jne        0x47e2f0

// block 0047e2ec
0047e2ec  mov        byte ptr [esi + 4], 3

// block 0047e2f0
0047e2f0  mov        eax, esi
0047e2f2  pop        esi
0047e2f3  pop        ebp
0047e2f4  ret        4
