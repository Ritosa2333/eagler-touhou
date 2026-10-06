
// block 0047e1ce
0047e1ce  mov        edi, edi
0047e1d0  push       ebp
0047e1d1  mov        ebp, esp
0047e1d3  push       esi
0047e1d4  mov        esi, ecx
0047e1d6  mov        ecx, dword ptr [ebp + 8]
0047e1d9  and        dword ptr [esi + 4], 0xffff00ff
0047e1e0  cmp        ecx, 2
0047e1e3  je         0x47e1ee

// block 0047e1e5
0047e1e5  cmp        ecx, 3
0047e1e8  je         0x47e1ee

// block 0047e1ea
0047e1ea  xor        eax, eax
0047e1ec  jmp        0x47e1f0

// block 0047e1ee
0047e1ee  mov        eax, ecx

// block 0047e1f0
0047e1f0  and        dword ptr [esi], 0
0047e1f3  mov        byte ptr [esi + 4], al
0047e1f6  cmp        ecx, 1
0047e1f9  jne        0x47e20c

// block 0047e1fb
0047e1fb  push       ecx
0047e1fc  call       0x47deeb

// block 0047e201
0047e201  pop        ecx
0047e202  mov        dword ptr [esi], eax
0047e204  test       eax, eax
0047e206  jne        0x47e20c

// block 0047e208
0047e208  mov        byte ptr [esi + 4], 3

// block 0047e20c
0047e20c  mov        eax, esi
0047e20e  pop        esi
0047e20f  pop        ebp
0047e210  ret        4
