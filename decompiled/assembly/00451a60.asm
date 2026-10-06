
// block 00451a60
00451a60  push       esi
00451a61  mov        esi, eax
00451a63  test       byte ptr [esi + 0x200], 8
00451a6a  mov        eax, dword ptr [0x4cf3f8]
00451a6f  je         0x451a76

// block 00451a71
00451a71  or         eax, 0xffffffff
00451a74  pop        esi
00451a75  ret        

// block 00451a76
00451a76  push       ebx
00451a77  push       0
00451a79  lea        ebx, [esi + 0xc]
00451a7c  push       ebx
00451a7d  push       0x4993cc
00451a82  push       0x800
00451a87  push       eax
00451a88  call       0x46ac64

// block 00451a8d
00451a8d  test       eax, eax
00451a8f  jge        0x451aaf

// block 00451a91
00451a91  push       0x4a29b4
00451a96  mov        ecx, 0x4b0ec8
00451a9b  mov        dword ptr [ebx], 0
00451aa1  call       0x464220

// block 00451aa6
00451aa6  add        esp, 4
00451aa9  pop        ebx
00451aaa  or         eax, 0xffffffff
00451aad  pop        esi
00451aae  ret        

// block 00451aaf
00451aaf  mov        eax, dword ptr [ebx]
00451ab1  mov        ecx, dword ptr [eax]
00451ab3  mov        edx, dword ptr [ecx + 0xc]
00451ab6  push       edi
00451ab7  push       0
00451ab9  lea        edi, [esi + 0x20]
00451abc  push       edi
00451abd  push       0x49953c
00451ac2  push       eax
00451ac3  call       edx

// block 00451ac5
00451ac5  test       eax, eax
00451ac7  jge        0x451ae4

// block 00451ac9
00451ac9  mov        eax, dword ptr [ebx]
00451acb  test       eax, eax
00451acd  je         0x451add

// block 00451acf
00451acf  mov        ecx, dword ptr [eax]
00451ad1  mov        edx, dword ptr [ecx + 8]
00451ad4  push       eax
00451ad5  call       edx

// block 00451ad7
00451ad7  mov        dword ptr [ebx], 0

// block 00451add
00451add  push       0x4a29b4
00451ae2  jmp        0x451b59

// block 00451ae4
00451ae4  mov        eax, dword ptr [edi]
00451ae6  mov        ecx, dword ptr [eax]
00451ae8  mov        edx, dword ptr [ecx + 0x2c]
00451aeb  push       0x49855c
00451af0  push       eax
00451af1  call       edx

// block 00451af3
00451af3  test       eax, eax
00451af5  mov        eax, dword ptr [edi]
00451af7  jge        0x451b26

// block 00451af9
00451af9  test       eax, eax
00451afb  je         0x451b0b

// block 00451afd
00451afd  mov        ecx, dword ptr [eax]
00451aff  mov        edx, dword ptr [ecx + 8]
00451b02  push       eax
00451b03  call       edx

// block 00451b05
00451b05  mov        dword ptr [edi], 0

// block 00451b0b
00451b0b  mov        eax, dword ptr [ebx]
00451b0d  test       eax, eax
00451b0f  je         0x451b1f

// block 00451b11
00451b11  mov        ecx, dword ptr [eax]
00451b13  mov        edx, dword ptr [ecx + 8]
00451b16  push       eax
00451b17  call       edx

// block 00451b19
00451b19  mov        dword ptr [ebx], 0

// block 00451b1f
00451b1f  push       0x4a29d4
00451b24  jmp        0x451b59

// block 00451b26
00451b26  mov        edx, dword ptr [0x4cf3f0]
00451b2c  mov        ecx, dword ptr [eax]
00451b2e  push       0x16
00451b30  push       edx
00451b31  push       eax
00451b32  mov        eax, dword ptr [ecx + 0x34]
00451b35  call       eax

// block 00451b37
00451b37  test       eax, eax
00451b39  jge        0x451b6d

// block 00451b3b
00451b3b  mov        eax, dword ptr [edi]
00451b3d  test       eax, eax
00451b3f  je         0x451b4f

// block 00451b41
00451b41  mov        ecx, dword ptr [eax]
00451b43  mov        edx, dword ptr [ecx + 8]
00451b46  push       eax
00451b47  call       edx

// block 00451b49
00451b49  mov        dword ptr [edi], 0

// block 00451b4f
00451b4f  call       0x4318f0

// block 00451b54
00451b54  push       0x4a2a04

// block 00451b59
00451b59  mov        ecx, 0x4b0ec8
00451b5e  call       0x464220

// block 00451b63
00451b63  add        esp, 4
00451b66  pop        edi
00451b67  pop        ebx
00451b68  or         eax, 0xffffffff
00451b6b  pop        esi
00451b6c  ret        

// block 00451b6d
00451b6d  mov        edi, dword ptr [edi]
00451b6f  mov        eax, dword ptr [edi]
00451b71  mov        ecx, dword ptr [eax + 0x1c]
00451b74  push       edi
00451b75  call       ecx

// block 00451b77
00451b77  push       0x4a2a38
00451b7c  mov        ecx, 0x4b0ec8
00451b81  call       0x464220

// block 00451b86
00451b86  mov        ebx, dword ptr [ebx]
00451b88  mov        edx, dword ptr [ebx]
00451b8a  mov        eax, dword ptr [edx + 0x10]
00451b8d  add        esp, 4
00451b90  push       1
00451b92  push       0
00451b94  push       0x451c70
00451b99  push       4
00451b9b  push       ebx
00451b9c  call       eax

// block 00451b9e
00451b9e  cmp        dword ptr [esi + 0x24], 0
00451ba2  je         0x451c12

// block 00451ba4
00451ba4  mov        eax, dword ptr [esi + 0x24]
00451ba7  mov        ecx, dword ptr [eax]
00451ba9  mov        edx, dword ptr [ecx + 0x2c]
00451bac  push       0x498764
00451bb1  push       eax
00451bb2  call       edx

// block 00451bb4
00451bb4  mov        eax, dword ptr [esi + 0x24]
00451bb7  mov        edx, dword ptr [0x4cf3f0]
00451bbd  mov        ecx, dword ptr [eax]
00451bbf  push       0xa
00451bc1  push       edx
00451bc2  push       eax
00451bc3  mov        eax, dword ptr [ecx + 0x34]
00451bc6  call       eax

// block 00451bc8
00451bc8  mov        dword ptr [0x4ce914], 0x2c
00451bd2  mov        eax, dword ptr [esi + 0x24]
00451bd5  mov        ecx, dword ptr [eax]
00451bd7  mov        edx, dword ptr [ecx + 0xc]
00451bda  push       0x4ce914
00451bdf  push       eax
00451be0  call       edx

// block 00451be2
00451be2  mov        esi, dword ptr [esi + 0x24]
00451be5  mov        eax, dword ptr [esi]
00451be7  mov        ecx, dword ptr [eax + 0x10]
00451bea  push       0
00451bec  push       0
00451bee  push       0x451cb0
00451bf3  push       esi
00451bf4  mov        dword ptr [0x4ce8c8], 0
00451bfe  call       ecx

// block 00451c00
00451c00  push       0x4a2a60
00451c05  mov        ecx, 0x4b0ec8
00451c0a  call       0x464220

// block 00451c0f
00451c0f  add        esp, 4

// block 00451c12
00451c12  pop        edi
00451c13  pop        ebx
00451c14  xor        eax, eax
00451c16  pop        esi
00451c17  ret        
