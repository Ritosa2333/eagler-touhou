
// block 00492c0c
00492c0c  mov        edi, edi
00492c0e  push       ebp
00492c0f  mov        ebp, esp
00492c11  push       ecx
00492c12  push       ecx
00492c13  push       esi
00492c14  mov        esi, dword ptr [ebp + 8]
00492c17  cmp        dword ptr [esi], 0x80000003
00492c1d  je         0x492cfd

// block 00492c23
00492c23  push       edi
00492c24  call       0x475467

// block 00492c29
00492c29  cmp        dword ptr [eax + 0x80], 0
00492c30  je         0x492c71

// block 00492c32
00492c32  call       0x475467

// block 00492c37
00492c37  lea        edi, [eax + 0x80]
00492c3d  call       0x4751d5

// block 00492c42
00492c42  cmp        dword ptr [edi], eax
00492c44  je         0x492c71

// block 00492c46
00492c46  cmp        dword ptr [esi], 0xe0434f4d
00492c4c  je         0x492c71

// block 00492c4e
00492c4e  push       dword ptr [ebp + 0x24]
00492c51  push       dword ptr [ebp + 0x20]
00492c54  push       dword ptr [ebp + 0x18]
00492c57  push       dword ptr [ebp + 0x14]
00492c5a  push       dword ptr [ebp + 0x10]
00492c5d  push       dword ptr [ebp + 0xc]
00492c60  push       esi
00492c61  call       0x491b69

// block 00492c66
00492c66  add        esp, 0x1c
00492c69  test       eax, eax
00492c6b  jne        0x492cfc

// block 00492c71
00492c71  mov        edi, dword ptr [ebp + 0x18]
00492c74  cmp        dword ptr [edi + 0xc], 0
00492c78  jne        0x492c7f

// block 00492c7a
00492c7a  call       0x472b94

// block 00492c7f
00492c7f  mov        esi, dword ptr [ebp + 0x1c]
00492c82  lea        eax, [ebp - 8]
00492c85  push       eax
00492c86  lea        eax, [ebp - 4]
00492c89  push       eax
00492c8a  push       esi
00492c8b  push       dword ptr [ebp + 0x20]
00492c8e  push       edi
00492c8f  call       0x491cdf

// block 00492c94
00492c94  mov        edi, eax
00492c96  mov        eax, dword ptr [ebp - 4]
00492c99  add        esp, 0x14
00492c9c  cmp        eax, dword ptr [ebp - 8]
00492c9f  jae        0x492cfc

// block 00492ca1
00492ca1  push       ebx

// block 00492ca2
00492ca2  cmp        esi, dword ptr [edi]
00492ca4  jl         0x492ced

// block 00492ca6
00492ca6  cmp        esi, dword ptr [edi + 4]
00492ca9  jg         0x492ced

// block 00492cab
00492cab  mov        eax, dword ptr [edi + 0xc]
00492cae  mov        ecx, dword ptr [edi + 0x10]
00492cb1  shl        eax, 4
00492cb4  add        eax, ecx
00492cb6  mov        ecx, dword ptr [eax - 0xc]
00492cb9  test       ecx, ecx
00492cbb  je         0x492cc3

// block 00492cbd
00492cbd  cmp        byte ptr [ecx + 8], 0
00492cc1  jne        0x492ced

// block 00492cc3
00492cc3  lea        ebx, [eax - 0x10]
00492cc6  test       byte ptr [ebx], 0x40
00492cc9  jne        0x492ced

// block 00492ccb
00492ccb  push       dword ptr [ebp + 0x24]
00492cce  mov        esi, dword ptr [ebp + 0xc]
00492cd1  push       dword ptr [ebp + 0x20]
00492cd4  push       0
00492cd6  push       dword ptr [ebp + 0x18]
00492cd9  push       dword ptr [ebp + 0x14]
00492cdc  push       dword ptr [ebp + 0x10]
00492cdf  push       dword ptr [ebp + 8]
00492ce2  call       0x492b9e

// block 00492ce7
00492ce7  mov        esi, dword ptr [ebp + 0x1c]
00492cea  add        esp, 0x1c

// block 00492ced
00492ced  inc        dword ptr [ebp - 4]
00492cf0  mov        eax, dword ptr [ebp - 4]
00492cf3  add        edi, 0x14
00492cf6  cmp        eax, dword ptr [ebp - 8]
00492cf9  jb         0x492ca2

// block 00492cfb
00492cfb  pop        ebx

// block 00492cfc
00492cfc  pop        edi

// block 00492cfd
00492cfd  pop        esi
00492cfe  leave      
00492cff  ret        
