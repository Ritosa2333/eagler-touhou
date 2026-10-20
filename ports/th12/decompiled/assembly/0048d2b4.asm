
// block 0048d2b4
0048d2b4  push       0xc
0048d2b6  push       0x4ab148
0048d2bb  call       0x46fcec

// block 0048d2c0
0048d2c0  xor        esi, esi
0048d2c2  cmp        dword ptr [ebp + 8], esi
0048d2c5  jne        0x48d2d0

// block 0048d2c7
0048d2c7  push       esi
0048d2c8  call       0x48d1da

// block 0048d2cd
0048d2cd  pop        ecx
0048d2ce  jmp        0x48d2f7

// block 0048d2d0
0048d2d0  push       dword ptr [ebp + 8]
0048d2d3  call       0x47c0fc

// block 0048d2d8
0048d2d8  pop        ecx
0048d2d9  mov        dword ptr [ebp - 4], esi
0048d2dc  push       dword ptr [ebp + 8]
0048d2df  call       0x48d192

// block 0048d2e4
0048d2e4  pop        ecx
0048d2e5  mov        dword ptr [ebp - 0x1c], eax
0048d2e8  mov        dword ptr [ebp - 4], 0xfffffffe
0048d2ef  call       0x48d2fd

// block 0048d2f4
0048d2f4  mov        eax, dword ptr [ebp - 0x1c]

// block 0048d2f7
0048d2f7  call       0x46fd31

// block 0048d2fc
0048d2fc  ret        
