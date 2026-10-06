
// block 00428780
00428780  mov        ecx, dword ptr [eax + 0x18]
00428783  push       edi
00428784  xor        edi, edi
00428786  test       ecx, ecx
00428788  je         0x4287b2

// block 0042878a
0042878a  push       esi
0042878b  jmp        0x428790

// block 00428790
00428790  cmp        dword ptr [ecx + 0xc], 1
00428794  mov        esi, dword ptr [ecx + 8]
00428797  je         0x4287ab

// block 00428799
00428799  mov        edx, dword ptr [ecx]
0042879b  fld        dword ptr [esp + 0xc]
0042879f  mov        eax, dword ptr [edx + 0x24]
004287a2  push       ecx
004287a3  fstp       dword ptr [esp]
004287a6  push       ebx
004287a7  call       eax

// block 004287a9
004287a9  add        edi, eax

// block 004287ab
004287ab  mov        ecx, esi
004287ad  test       esi, esi
004287af  jne        0x428790

// block 004287b1
004287b1  pop        esi

// block 004287b2
004287b2  mov        eax, edi
004287b4  pop        edi
004287b5  ret        4
