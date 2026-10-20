
// block 00485b93
00485b93  mov        edi, edi
00485b95  push       ebp
00485b96  mov        ebp, esp
00485b98  sub        esp, 0x7c
00485b9b  mov        eax, dword ptr [0x4ad138]
00485ba0  xor        eax, ebp
00485ba2  mov        dword ptr [ebp - 4], eax
00485ba5  push       esi
00485ba6  push       edi
00485ba7  mov        edi, dword ptr [ebp + 8]
00485baa  call       0x475467

// block 00485baf
00485baf  mov        esi, eax
00485bb1  mov        edx, edi
00485bb3  add        esi, 0x9c
00485bb9  call       0x485b44

// block 00485bbe
00485bbe  mov        edi, eax
00485bc0  push       0x78
00485bc2  lea        eax, [ebp - 0x7c]
00485bc5  push       eax
00485bc6  mov        eax, dword ptr [esi + 0x14]
00485bc9  neg        eax
00485bcb  sbb        eax, eax
00485bcd  and        eax, 0xfffff005
00485bd2  add        eax, 0x1002
00485bd7  push       eax
00485bd8  push       edi
00485bd9  call       dword ptr [0x4980e0]

// block 00485bdf
00485bdf  test       eax, eax
00485be1  jne        0x485be9

// block 00485be3
00485be3  and        dword ptr [esi + 8], eax
00485be6  inc        eax
00485be7  jmp        0x485c1b

// block 00485be9
00485be9  lea        eax, [ebp - 0x7c]
00485bec  push       eax
00485bed  push       dword ptr [esi + 4]
00485bf0  call       0x46e3f8

// block 00485bf5
00485bf5  pop        ecx
00485bf6  pop        ecx
00485bf7  test       eax, eax
00485bf9  jne        0x485c10

// block 00485bfb
00485bfb  push       edi
00485bfc  call       0x485b20

// block 00485c01
00485c01  pop        ecx
00485c02  test       eax, eax
00485c04  je         0x485c10

// block 00485c06
00485c06  or         dword ptr [esi + 8], 4
00485c0a  mov        dword ptr [esi + 0x1c], edi
00485c0d  mov        dword ptr [esi + 0x18], edi

// block 00485c10
00485c10  mov        eax, dword ptr [esi + 8]
00485c13  shr        eax, 2
00485c16  not        eax
00485c18  and        eax, 1

// block 00485c1b
00485c1b  mov        ecx, dword ptr [ebp - 4]
00485c1e  pop        edi
00485c1f  xor        ecx, ebp
00485c21  pop        esi
00485c22  call       0x46c932

// block 00485c27
00485c27  leave      
00485c28  ret        4
