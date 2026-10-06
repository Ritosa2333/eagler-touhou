
// block 004664f0
004664f0  push       esi
004664f1  mov        esi, eax
004664f3  mov        eax, dword ptr [esi + 4]
004664f6  xor        ecx, ecx
004664f8  cmp        eax, ecx
004664fa  je         0x466504

// block 004664fc
004664fc  cmp        dword ptr [esi + 0x30], ecx
004664ff  jne        0x46650b

// block 00466501
00466501  mov        dword ptr [esi + 0x34], ecx

// block 00466504
00466504  mov        eax, 0x800401f0
00466509  pop        esi
0046650a  ret        

// block 0046650b
0046650b  mov        dword ptr [esi + 0x30], ecx
0046650e  mov        dword ptr [esi + 0x34], 1
00466515  mov        eax, dword ptr [eax]
00466517  mov        ecx, dword ptr [eax]
00466519  mov        edx, dword ptr [ecx + 0x48]
0046651c  push       ebx
0046651d  push       edi
0046651e  push       eax
0046651f  call       edx

// block 00466521
00466521  mov        edi, dword ptr [esi + 0xc]
00466524  push       1
00466526  push       0
00466528  mov        ebx, eax
0046652a  mov        eax, dword ptr [edi + 0x8c]
00466530  push       0
00466532  push       eax
00466533  call       dword ptr [0x4980a0]

// block 00466539
00466539  mov        dword ptr [edi + 0x98], eax
0046653f  mov        esi, dword ptr [esi + 0xc]
00466542  cmp        dword ptr [esi + 0x78], 1
00466546  jne        0x46655f

// block 00466548
00466548  mov        ecx, dword ptr [esi + 0x8c]
0046654e  push       ecx
0046654f  call       dword ptr [0x4980a4]

// block 00466555
00466555  mov        dword ptr [esi + 0x8c], 0xffffffff

// block 0046655f
0046655f  pop        edi
00466560  mov        eax, ebx
00466562  pop        ebx
00466563  pop        esi
00466564  ret        
