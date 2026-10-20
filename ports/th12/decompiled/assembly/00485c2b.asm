
// block 00485c2b
00485c2b  mov        edi, edi
00485c2d  push       ebp
00485c2e  mov        ebp, esp
00485c30  sub        esp, 0x7c
00485c33  mov        eax, dword ptr [0x4ad138]
00485c38  xor        eax, ebp
00485c3a  mov        dword ptr [ebp - 4], eax
00485c3d  push       esi
00485c3e  push       0x78
00485c40  lea        eax, [ebp - 0x7c]
00485c43  push       eax
00485c44  mov        eax, dword ptr [ebp + 8]
00485c47  and        eax, 0x3ff
00485c4c  push       1
00485c4e  or         eax, 0x400
00485c53  push       eax
00485c54  mov        esi, ecx
00485c56  call       dword ptr [0x4980e0]

// block 00485c5c
00485c5c  test       eax, eax
00485c5e  jne        0x485c64

// block 00485c60
00485c60  xor        eax, eax
00485c62  jmp        0x485c92

// block 00485c64
00485c64  lea        edx, [ebp - 0x7c]
00485c67  call       0x485b44

// block 00485c6c
00485c6c  cmp        dword ptr [ebp + 8], eax
00485c6f  je         0x485c8f

// block 00485c71
00485c71  cmp        dword ptr [ebp + 0xc], 0
00485c75  je         0x485c8f

// block 00485c77
00485c77  mov        esi, dword ptr [esi]
00485c79  push       edi
00485c7a  mov        edx, esi
00485c7c  call       0x485b78

// block 00485c81
00485c81  push       esi
00485c82  mov        edi, eax
00485c84  call       0x475860

// block 00485c89
00485c89  pop        ecx
00485c8a  cmp        edi, eax
00485c8c  pop        edi
00485c8d  je         0x485c60

// block 00485c8f
00485c8f  xor        eax, eax
00485c91  inc        eax

// block 00485c92
00485c92  mov        ecx, dword ptr [ebp - 4]
00485c95  xor        ecx, ebp
00485c97  pop        esi
00485c98  call       0x46c932

// block 00485c9d
00485c9d  leave      
00485c9e  ret        
