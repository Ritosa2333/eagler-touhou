
// block 0047e26b
0047e26b  mov        edi, edi
0047e26d  push       ebp
0047e26e  mov        ebp, esp
0047e270  cmp        dword ptr [ebp + 8], 0
0047e274  push       esi
0047e275  mov        esi, ecx
0047e277  je         0x47e2a1

// block 0047e279
0047e279  push       0
0047e27b  push       0x10
0047e27d  mov        ecx, 0x4b42f8
0047e282  call       0x47dc29

// block 0047e287
0047e287  test       eax, eax
0047e289  je         0x47e299

// block 0047e28b
0047e28b  push       dword ptr [ebp + 8]
0047e28e  mov        ecx, eax
0047e290  push       dword ptr [esi]
0047e292  call       0x47df71

// block 0047e297
0047e297  jmp        0x47e29b

// block 0047e299
0047e299  xor        eax, eax

// block 0047e29b
0047e29b  mov        dword ptr [esi], eax
0047e29d  test       eax, eax
0047e29f  jne        0x47e2a5

// block 0047e2a1
0047e2a1  mov        byte ptr [esi + 4], 3

// block 0047e2a5
0047e2a5  pop        esi
0047e2a6  pop        ebp
0047e2a7  ret        4
