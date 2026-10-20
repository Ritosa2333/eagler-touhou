
// block 0048af42
0048af42  mov        edi, edi
0048af44  push       ebp
0048af45  mov        ebp, esp
0048af47  cmp        dword ptr [0x4d6428], 0
0048af4e  push       esi
0048af4f  mov        esi, dword ptr [0x4b3d80]
0048af55  jne        0x48af5b

// block 0048af57
0048af57  xor        eax, eax
0048af59  jmp        0x48afbe

// block 0048af5b
0048af5b  push       edi
0048af5c  test       esi, esi
0048af5e  jne        0x48af7b

// block 0048af60
0048af60  cmp        dword ptr [0x4b3d88], esi
0048af66  je         0x48afbb

// block 0048af68
0048af68  call       0x48e343

// block 0048af6d
0048af6d  test       eax, eax
0048af6f  jne        0x48afbb

// block 0048af71
0048af71  mov        esi, dword ptr [0x4b3d80]
0048af77  test       esi, esi
0048af79  je         0x48afbb

// block 0048af7b
0048af7b  cmp        dword ptr [ebp + 8], 0
0048af7f  je         0x48afbb

// block 0048af81
0048af81  push       dword ptr [ebp + 8]
0048af84  call       0x475860

// block 0048af89
0048af89  pop        ecx
0048af8a  mov        edi, eax
0048af8c  jmp        0x48afb5

// block 0048af8e
0048af8e  push       eax
0048af8f  call       0x475860

// block 0048af94
0048af94  pop        ecx
0048af95  cmp        eax, edi
0048af97  jbe        0x48afb2

// block 0048af99
0048af99  mov        eax, dword ptr [esi]
0048af9b  cmp        byte ptr [eax + edi], 0x3d
0048af9f  jne        0x48afb2

// block 0048afa1
0048afa1  push       edi
0048afa2  push       dword ptr [ebp + 8]
0048afa5  push       eax
0048afa6  call       0x48e329

// block 0048afab
0048afab  add        esp, 0xc
0048afae  test       eax, eax
0048afb0  je         0x48afc1

// block 0048afb2
0048afb2  add        esi, 4

// block 0048afb5
0048afb5  mov        eax, dword ptr [esi]
0048afb7  test       eax, eax
0048afb9  jne        0x48af8e

// block 0048afbb
0048afbb  xor        eax, eax

// block 0048afbd
0048afbd  pop        edi

// block 0048afbe
0048afbe  pop        esi
0048afbf  pop        ebp
0048afc0  ret        

// block 0048afc1
0048afc1  mov        eax, dword ptr [esi]
0048afc3  lea        eax, [eax + edi + 1]
0048afc7  jmp        0x48afbd
