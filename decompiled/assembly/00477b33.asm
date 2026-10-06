
// block 00477b33
00477b33  mov        edi, edi
00477b35  push       ebp
00477b36  mov        ebp, esp
00477b38  push       ecx
00477b39  push       ecx
00477b3a  push       esi
00477b3b  call       0x4753ee

// block 00477b40
00477b40  mov        esi, eax
00477b42  test       esi, esi
00477b44  je         0x477c90

// block 00477b4a
00477b4a  mov        edx, dword ptr [esi + 0x5c]
00477b4d  mov        eax, dword ptr [0x4adb9c]
00477b52  push       edi
00477b53  mov        edi, dword ptr [ebp + 8]
00477b56  mov        ecx, edx
00477b58  push       ebx

// block 00477b59
00477b59  cmp        dword ptr [ecx], edi
00477b5b  je         0x477b6b

// block 00477b5d
00477b5d  mov        ebx, eax
00477b5f  imul       ebx, ebx, 0xc
00477b62  add        ecx, 0xc
00477b65  add        ebx, edx
00477b67  cmp        ecx, ebx
00477b69  jb         0x477b59

// block 00477b6b
00477b6b  imul       eax, eax, 0xc
00477b6e  add        eax, edx
00477b70  cmp        ecx, eax
00477b72  jae        0x477b7c

// block 00477b74
00477b74  cmp        dword ptr [ecx], edi
00477b76  jne        0x477b7c

// block 00477b78
00477b78  mov        eax, ecx
00477b7a  jmp        0x477b7e

// block 00477b7c
00477b7c  xor        eax, eax

// block 00477b7e
00477b7e  test       eax, eax
00477b80  je         0x477b8c

// block 00477b82
00477b82  mov        ebx, dword ptr [eax + 8]
00477b85  mov        dword ptr [ebp - 4], ebx
00477b88  test       ebx, ebx
00477b8a  jne        0x477b93

// block 00477b8c
00477b8c  xor        eax, eax
00477b8e  jmp        0x477c8e

// block 00477b93
00477b93  cmp        ebx, 5
00477b96  jne        0x477ba4

// block 00477b98
00477b98  and        dword ptr [eax + 8], 0
00477b9c  xor        eax, eax
00477b9e  inc        eax
00477b9f  jmp        0x477c8e

// block 00477ba4
00477ba4  cmp        ebx, 1
00477ba7  je         0x477c8b

// block 00477bad
00477bad  mov        ecx, dword ptr [esi + 0x60]
00477bb0  mov        dword ptr [ebp - 8], ecx
00477bb3  mov        ecx, dword ptr [ebp + 0xc]
00477bb6  mov        dword ptr [esi + 0x60], ecx
00477bb9  mov        ecx, dword ptr [eax + 4]
00477bbc  cmp        ecx, 8
00477bbf  jne        0x477c7d

// block 00477bc5
00477bc5  mov        ecx, dword ptr [0x4adb90]
00477bcb  mov        edi, dword ptr [0x4adb94]
00477bd1  mov        edx, ecx
00477bd3  add        edi, ecx
00477bd5  cmp        edx, edi
00477bd7  jge        0x477bfd

// block 00477bd9
00477bd9  imul       ecx, ecx, 0xc

// block 00477bdc
00477bdc  mov        edi, dword ptr [esi + 0x5c]
00477bdf  and        dword ptr [ecx + edi + 8], 0
00477be4  mov        edi, dword ptr [0x4adb90]
00477bea  mov        ebx, dword ptr [0x4adb94]
00477bf0  inc        edx
00477bf1  add        ebx, edi
00477bf3  add        ecx, 0xc
00477bf6  cmp        edx, ebx
00477bf8  jl         0x477bdc

// block 00477bfa
00477bfa  mov        ebx, dword ptr [ebp - 4]

// block 00477bfd
00477bfd  mov        eax, dword ptr [eax]
00477bff  mov        edi, dword ptr [esi + 0x64]
00477c02  cmp        eax, 0xc000008e
00477c07  jne        0x477c12

// block 00477c09
00477c09  mov        dword ptr [esi + 0x64], 0x83
00477c10  jmp        0x477c70

// block 00477c12
00477c12  cmp        eax, 0xc0000090
00477c17  jne        0x477c22

// block 00477c19
00477c19  mov        dword ptr [esi + 0x64], 0x81
00477c20  jmp        0x477c70

// block 00477c22
00477c22  cmp        eax, 0xc0000091
00477c27  jne        0x477c32

// block 00477c29
00477c29  mov        dword ptr [esi + 0x64], 0x84
00477c30  jmp        0x477c70

// block 00477c32
00477c32  cmp        eax, 0xc0000093
00477c37  jne        0x477c42

// block 00477c39
00477c39  mov        dword ptr [esi + 0x64], 0x85
00477c40  jmp        0x477c70

// block 00477c42
00477c42  cmp        eax, 0xc000008d
00477c47  jne        0x477c52

// block 00477c49
00477c49  mov        dword ptr [esi + 0x64], 0x82
00477c50  jmp        0x477c70

// block 00477c52
00477c52  cmp        eax, 0xc000008f
00477c57  jne        0x477c62

// block 00477c59
00477c59  mov        dword ptr [esi + 0x64], 0x86
00477c60  jmp        0x477c70

// block 00477c62
00477c62  cmp        eax, 0xc0000092
00477c67  jne        0x477c70

// block 00477c69
00477c69  mov        dword ptr [esi + 0x64], 0x8a

// block 00477c70
00477c70  push       dword ptr [esi + 0x64]
00477c73  push       8
00477c75  call       ebx

// block 00477c77
00477c77  pop        ecx
00477c78  mov        dword ptr [esi + 0x64], edi
00477c7b  jmp        0x477c84

// block 00477c7d
00477c7d  and        dword ptr [eax + 8], 0
00477c81  push       ecx
00477c82  call       ebx

// block 00477c84
00477c84  mov        eax, dword ptr [ebp - 8]
00477c87  pop        ecx
00477c88  mov        dword ptr [esi + 0x60], eax

// block 00477c8b
00477c8b  or         eax, 0xffffffff

// block 00477c8e
00477c8e  pop        ebx
00477c8f  pop        edi

// block 00477c90
00477c90  pop        esi
00477c91  leave      
00477c92  ret        
