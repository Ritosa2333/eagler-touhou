
// block 0048eb7b
0048eb7b  mov        edi, edi
0048eb7d  push       ebp
0048eb7e  mov        ebp, esp
0048eb80  push       ebx
0048eb81  push       esi
0048eb82  mov        esi, dword ptr [ebp + 8]
0048eb85  push       edi
0048eb86  xor        edi, edi
0048eb88  or         ebx, 0xffffffff
0048eb8b  cmp        esi, edi
0048eb8d  jne        0x48ebab

// block 0048eb8f
0048eb8f  call       0x46e96f

// block 0048eb94
0048eb94  push       edi
0048eb95  push       edi
0048eb96  push       edi
0048eb97  push       edi
0048eb98  push       edi
0048eb99  mov        dword ptr [eax], 0x16
0048eb9f  call       0x470f2d

// block 0048eba4
0048eba4  add        esp, 0x14
0048eba7  or         eax, ebx
0048eba9  jmp        0x48ebed

// block 0048ebab
0048ebab  test       byte ptr [esi + 0xc], 0x83
0048ebaf  je         0x48ebe8

// block 0048ebb1
0048ebb1  push       esi
0048ebb2  call       0x48d12a

// block 0048ebb7
0048ebb7  push       esi
0048ebb8  mov        ebx, eax
0048ebba  call       0x49112f

// block 0048ebbf
0048ebbf  push       esi
0048ebc0  call       0x47c1da

// block 0048ebc5
0048ebc5  push       eax
0048ebc6  call       0x491062

// block 0048ebcb
0048ebcb  add        esp, 0x10
0048ebce  test       eax, eax
0048ebd0  jge        0x48ebd7

// block 0048ebd2
0048ebd2  or         ebx, 0xffffffff
0048ebd5  jmp        0x48ebe8

// block 0048ebd7
0048ebd7  mov        eax, dword ptr [esi + 0x1c]
0048ebda  cmp        eax, edi
0048ebdc  je         0x48ebe8

// block 0048ebde
0048ebde  push       eax
0048ebdf  call       0x46c941

// block 0048ebe4
0048ebe4  pop        ecx
0048ebe5  mov        dword ptr [esi + 0x1c], edi

// block 0048ebe8
0048ebe8  mov        dword ptr [esi + 0xc], edi
0048ebeb  mov        eax, ebx

// block 0048ebed
0048ebed  pop        edi
0048ebee  pop        esi
0048ebef  pop        ebx
0048ebf0  pop        ebp
0048ebf1  ret        
