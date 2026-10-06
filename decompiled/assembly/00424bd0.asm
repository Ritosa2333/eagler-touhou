
// block 00424bd0
00424bd0  push       esi
00424bd1  push       0x88
00424bd6  call       0x46c9ea

// block 00424bdb
00424bdb  mov        esi, eax
00424bdd  add        esp, 4
00424be0  test       esi, esi
00424be2  je         0x424c1f

// block 00424be4
00424be4  push       0x88
00424be9  push       0
00424beb  push       esi
00424bec  call       0x477420

// block 00424bf1
00424bf1  fld1       
00424bf3  mov        dword ptr [esi + 0xc], esi
00424bf6  mov        dword ptr [esi + 0x10], 0
00424bfd  mov        dword ptr [esi + 0x14], 0
00424c04  fstp       dword ptr [esi + 0x7c]
00424c07  add        esp, 0xc
00424c0a  or         eax, 0xffffffff
00424c0d  mov        dword ptr [esi + 0x84], eax
00424c13  mov        dword ptr [esi + 0x6c], 0x12c
00424c1a  mov        dword ptr [esi + 0x70], eax
00424c1d  jmp        0x424c21

// block 00424c1f
00424c1f  xor        esi, esi

// block 00424c21
00424c21  mov        eax, dword ptr [0x4b0cb0]
00424c26  mov        edx, dword ptr [esp + 8]
00424c2a  lea        eax, [eax + eax*2 + 0x1e]
00424c2e  cmp        dword ptr [edx + eax*4 + 4], 0
00424c33  lea        eax, [edx + eax*4]
00424c36  lea        ecx, [esi + 0xc]
00424c39  je         0x424c49

// block 00424c3b
00424c3b  jmp        0x424c40

// block 00424c40
00424c40  mov        eax, dword ptr [eax + 4]
00424c43  cmp        dword ptr [eax + 4], 0
00424c47  jne        0x424c40

// block 00424c49
00424c49  mov        edx, dword ptr [eax + 4]
00424c4c  test       edx, edx
00424c4e  je         0x424c59

// block 00424c50
00424c50  mov        dword ptr [ecx + 4], edx
00424c53  mov        edx, dword ptr [eax + 4]
00424c56  mov        dword ptr [edx + 8], ecx

// block 00424c59
00424c59  mov        dword ptr [eax + 4], ecx
00424c5c  mov        dword ptr [ecx + 8], eax
00424c5f  mov        dword ptr [esi + 0x70], 0x1e
00424c66  mov        eax, dword ptr [edi]
00424c68  mov        dword ptr [esi], eax
00424c6a  mov        ecx, dword ptr [edi + 4]
00424c6d  mov        eax, dword ptr [esp + 0xc]
00424c71  mov        dword ptr [esi + 4], ecx
00424c74  mov        edx, dword ptr [edi + 8]
00424c77  push       0x40
00424c79  push       eax
00424c7a  lea        ecx, [esi + 0x1c]
00424c7d  push       ecx
00424c7e  mov        dword ptr [esi + 8], edx
00424c81  call       0x46d290

// block 00424c86
00424c86  mov        dword ptr [esi + 0x64], 0
00424c8d  mov        edx, dword ptr [0x4b0cb8]
00424c93  mov        dword ptr [esi + 0x68], edx
00424c96  mov        eax, dword ptr [0x4b0cc0]
00424c9b  add        esp, 0xc
00424c9e  cmp        dword ptr [0x4b0cb8], 0
00424ca5  jne        0x424cac

// block 00424ca7
00424ca7  mov        eax, dword ptr [0x4b0cbc]

// block 00424cac
00424cac  mov        dword ptr [esi + 0x60], eax
00424caf  mov        eax, esi
00424cb1  pop        esi
00424cb2  ret        8
