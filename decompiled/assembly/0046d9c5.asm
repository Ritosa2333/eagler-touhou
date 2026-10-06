
// block 0046d9c5
0046d9c5  mov        edi, edi
0046d9c7  push       esi
0046d9c8  push       4
0046d9ca  push       0x20
0046d9cc  call       0x477813

// block 0046d9d1
0046d9d1  mov        esi, eax
0046d9d3  push       esi
0046d9d4  call       0x475163

// block 0046d9d9
0046d9d9  add        esp, 0xc
0046d9dc  mov        dword ptr [0x4d6430], eax
0046d9e1  mov        dword ptr [0x4d642c], eax
0046d9e6  test       esi, esi
0046d9e8  jne        0x46d9ef

// block 0046d9ea
0046d9ea  push       0x18
0046d9ec  pop        eax
0046d9ed  pop        esi
0046d9ee  ret        

// block 0046d9ef
0046d9ef  and        dword ptr [esi], 0
0046d9f2  xor        eax, eax
0046d9f4  pop        esi
0046d9f5  ret        
