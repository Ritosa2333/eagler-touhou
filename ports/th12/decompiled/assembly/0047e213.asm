
// block 0047e213
0047e213  mov        edi, edi
0047e215  push       ebp
0047e216  mov        ebp, esp
0047e218  push       ecx
0047e219  mov        dword ptr [ebp - 4], ecx
0047e21c  mov        ecx, dword ptr [ecx]
0047e21e  push       esi
0047e21f  mov        esi, dword ptr [ebp + 8]
0047e222  push       edi
0047e223  test       ecx, ecx
0047e225  je         0x47e25c

// block 0047e227
0047e227  test       esi, esi
0047e229  jne        0x47e246

// block 0047e22b
0047e22b  mov        eax, dword ptr [ecx]
0047e22d  call       dword ptr [eax]

// block 0047e22f
0047e22f  lea        edi, [eax + 1]
0047e232  push       esi
0047e233  push       edi
0047e234  mov        ecx, 0x4b42f8
0047e239  call       0x47dc29

// block 0047e23e
0047e23e  mov        esi, eax
0047e240  test       esi, esi
0047e242  je         0x47e263

// block 0047e244
0047e244  jmp        0x47e249

// block 0047e246
0047e246  mov        edi, dword ptr [ebp + 0xc]

// block 0047e249
0047e249  mov        ecx, dword ptr [ebp - 4]
0047e24c  lea        eax, [esi + edi - 1]
0047e250  push       eax
0047e251  push       esi
0047e252  call       0x47dda9

// block 0047e257
0047e257  mov        byte ptr [eax], 0
0047e25a  jmp        0x47e263

// block 0047e25c
0047e25c  test       esi, esi
0047e25e  je         0x47e263

// block 0047e260
0047e260  mov        byte ptr [esi], 0

// block 0047e263
0047e263  pop        edi
0047e264  mov        eax, esi
0047e266  pop        esi
0047e267  leave      
0047e268  ret        8
