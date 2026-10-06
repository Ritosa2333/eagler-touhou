
// block 00492a59
00492a59  mov        edi, edi
00492a5b  push       ebp
00492a5c  mov        ebp, esp
00492a5e  mov        eax, dword ptr [ebp + 8]
00492a61  sub        esp, 0x10
00492a64  test       eax, eax
00492a66  jne        0x492a6a

// block 00492a68
00492a68  leave      
00492a69  ret        

// block 00492a6a
00492a6a  push       ebx
00492a6b  mov        ebx, dword ptr [ebp + 0x10]
00492a6e  push       esi
00492a6f  mov        esi, dword ptr [eax]
00492a71  push       edi
00492a72  mov        edi, dword ptr [ebp + 0xc]
00492a75  test       edi, edi
00492a77  je         0x492a7f

// block 00492a79
00492a79  cmp        byte ptr [edi + 8], 0
00492a7d  jne        0x492a95

// block 00492a7f
00492a7f  mov        eax, dword ptr [esi]
00492a81  cmp        eax, 0xe0434f4d
00492a86  je         0x492b4d

// block 00492a8c
00492a8c  test       bl, 0x40
00492a8f  je         0x492b4d

// block 00492a95
00492a95  cmp        dword ptr [esi], 0xe06d7363
00492a9b  jne        0x492b86

// block 00492aa1
00492aa1  cmp        dword ptr [esi + 0x10], 3
00492aa5  jne        0x492b86

// block 00492aab
00492aab  mov        eax, dword ptr [esi + 0x14]
00492aae  cmp        eax, 0x19930520
00492ab3  je         0x492ac7

// block 00492ab5
00492ab5  cmp        eax, 0x19930521
00492aba  je         0x492ac7

// block 00492abc
00492abc  cmp        eax, 0x19930522
00492ac1  jne        0x492b86

// block 00492ac7
00492ac7  cmp        dword ptr [esi + 0x1c], 0
00492acb  jne        0x492aea

// block 00492acd
00492acd  call       0x475467

// block 00492ad2
00492ad2  cmp        dword ptr [eax + 0x88], 0
00492ad9  je         0x492b86

// block 00492adf
00492adf  call       0x475467

// block 00492ae4
00492ae4  mov        esi, dword ptr [eax + 0x88]

// block 00492aea
00492aea  mov        eax, dword ptr [esi + 0x1c]
00492aed  mov        eax, dword ptr [eax + 0xc]
00492af0  or         ebx, 0x80000000
00492af6  mov        dword ptr [ebp - 0xc], edi
00492af9  mov        edi, dword ptr [eax]
00492afb  mov        dword ptr [ebp - 0x10], ebx
00492afe  lea        ebx, [eax + 4]
00492b01  jmp        0x492b20

// block 00492b03
00492b03  mov        eax, dword ptr [ebx]
00492b05  push       dword ptr [esi + 0x1c]
00492b08  mov        dword ptr [ebp + 8], eax
00492b0b  push       eax
00492b0c  lea        eax, [ebp - 0x10]
00492b0f  push       eax
00492b10  call       0x491fb3

// block 00492b15
00492b15  add        esp, 0xc
00492b18  test       eax, eax
00492b1a  jne        0x492b26

// block 00492b1c
00492b1c  dec        edi
00492b1d  add        ebx, 4

// block 00492b20
00492b20  test       edi, edi
00492b22  jg         0x492b03

// block 00492b24
00492b24  jmp        0x492b86

// block 00492b26
00492b26  call       0x475467

// block 00492b2b
00492b2b  add        eax, 0x90
00492b30  inc        dword ptr [eax]
00492b32  cmp        dword ptr [ebp + 0x14], 0
00492b36  je         0x492b96

// block 00492b38
00492b38  push       dword ptr [ebp + 8]
00492b3b  lea        eax, [ebp - 0x10]
00492b3e  push       eax
00492b3f  push       dword ptr [ebp + 0x14]
00492b42  push       esi
00492b43  call       0x4929c7

// block 00492b48
00492b48  add        esp, 0x10
00492b4b  jmp        0x492b96

// block 00492b4d
00492b4d  cmp        eax, 0xe06d7363
00492b52  jne        0x492b8a

// block 00492b54
00492b54  cmp        dword ptr [esi + 0x10], 3
00492b58  jne        0x492b8a

// block 00492b5a
00492b5a  mov        eax, dword ptr [esi + 0x14]
00492b5d  cmp        eax, 0x19930520
00492b62  je         0x492b72

// block 00492b64
00492b64  cmp        eax, 0x19930521
00492b69  je         0x492b72

// block 00492b6b
00492b6b  cmp        eax, 0x19930522
00492b70  jne        0x492b8a

// block 00492b72
00492b72  cmp        dword ptr [esi + 0x1c], 0
00492b76  jne        0x492b8a

// block 00492b78
00492b78  call       0x475467

// block 00492b7d
00492b7d  cmp        dword ptr [eax + 0x88], 0
00492b84  jne        0x492b8a

// block 00492b86
00492b86  xor        eax, eax
00492b88  jmp        0x492b99

// block 00492b8a
00492b8a  call       0x475467

// block 00492b8f
00492b8f  add        eax, 0x90
00492b94  inc        dword ptr [eax]

// block 00492b96
00492b96  xor        eax, eax
00492b98  inc        eax

// block 00492b99
00492b99  pop        edi
00492b9a  pop        esi
00492b9b  pop        ebx
00492b9c  leave      
00492b9d  ret        
