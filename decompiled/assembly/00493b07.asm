
// block 00493b07
00493b07  mov        edi, edi
00493b09  push       ebp
00493b0a  mov        ebp, esp
00493b0c  sub        esp, 0x28
00493b0f  xor        eax, eax
00493b11  push       ebx
00493b12  mov        ebx, dword ptr [ebp + 0xc]
00493b15  push       esi
00493b16  mov        esi, dword ptr [ebp + 0x10]
00493b19  push       edi
00493b1a  mov        edi, dword ptr [ebp + 8]
00493b1d  mov        byte ptr [ebp - 8], al
00493b20  mov        byte ptr [ebp - 7], al
00493b23  mov        byte ptr [ebp - 6], al
00493b26  mov        byte ptr [ebp - 5], al
00493b29  mov        byte ptr [ebp - 4], al
00493b2c  mov        byte ptr [ebp - 3], al
00493b2f  mov        byte ptr [ebp - 2], al
00493b32  mov        byte ptr [ebp - 1], al
00493b35  cmp        dword ptr [0x4d52d0], eax
00493b3b  je         0x493b4b

// block 00493b3d
00493b3d  push       dword ptr [0x4d52d8]
00493b43  call       0x4751de

// block 00493b48
00493b48  pop        ecx
00493b49  jmp        0x493b50

// block 00493b4b
00493b4b  mov        eax, 0x49616c

// block 00493b50
00493b50  mov        ecx, dword ptr [ebp + 0x14]
00493b53  mov        edx, 0xa6
00493b58  cmp        ecx, edx
00493b5a  jg         0x493cd4

// block 00493b60
00493b60  je         0x493cc1

// block 00493b66
00493b66  cmp        ecx, 0x19
00493b69  jg         0x493c67

// block 00493b6f
00493b6f  je         0x493c5e

// block 00493b75
00493b75  mov        edx, ecx
00493b77  push       2
00493b79  pop        ecx
00493b7a  sub        edx, ecx
00493b7c  je         0x493c4f

// block 00493b82
00493b82  dec        edx
00493b83  je         0x493c46

// block 00493b89
00493b89  sub        edx, 5
00493b8c  je         0x493c37

// block 00493b92
00493b92  dec        edx
00493b93  je         0x493c1f

// block 00493b99
00493b99  sub        edx, 5
00493b9c  je         0x493c0f

// block 00493b9e
00493b9e  dec        edx
00493b9f  je         0x493be6

// block 00493ba1
00493ba1  sub        edx, 9
00493ba4  jne        0x493d7e

// block 00493baa
00493baa  mov        dword ptr [ebp - 0x28], 3

// block 00493bb1
00493bb1  mov        dword ptr [ebp - 0x24], 0x4a4578

// block 00493bb8
00493bb8  fld        qword ptr [edi]
00493bba  lea        ecx, [ebp - 0x28]
00493bbd  fstp       qword ptr [ebp - 0x20]
00493bc0  push       ecx
00493bc1  fld        qword ptr [ebx]
00493bc3  fstp       qword ptr [ebp - 0x18]
00493bc6  fld        qword ptr [esi]
00493bc8  fstp       qword ptr [ebp - 0x10]
00493bcb  call       eax

// block 00493bcd
00493bcd  pop        ecx
00493bce  test       eax, eax
00493bd0  jne        0x493d79

// block 00493bd6
00493bd6  call       0x46e96f

// block 00493bdb
00493bdb  mov        dword ptr [eax], 0x22
00493be1  jmp        0x493d79

// block 00493be6
00493be6  mov        dword ptr [ebp - 0x24], 0x4a4574

// block 00493bed
00493bed  fld        qword ptr [edi]
00493bef  lea        ecx, [ebp - 0x28]
00493bf2  fstp       qword ptr [ebp - 0x20]
00493bf5  push       ecx
00493bf6  fld        qword ptr [ebx]
00493bf8  mov        dword ptr [ebp - 0x28], 4
00493bff  fstp       qword ptr [ebp - 0x18]
00493c02  fld        qword ptr [esi]
00493c04  fstp       qword ptr [ebp - 0x10]
00493c07  call       eax

// block 00493c09
00493c09  pop        ecx
00493c0a  jmp        0x493d79

// block 00493c0f
00493c0f  mov        dword ptr [ebp - 0x28], 3
00493c16  mov        dword ptr [ebp - 0x24], 0x4a4574
00493c1d  jmp        0x493bb8

// block 00493c1f
00493c1f  mov        dword ptr [ebp - 0x24], 0x4a456c

// block 00493c26
00493c26  fld        qword ptr [edi]
00493c28  fstp       qword ptr [ebp - 0x20]
00493c2b  fld        qword ptr [ebx]
00493c2d  fstp       qword ptr [ebp - 0x18]
00493c30  fld        qword ptr [esi]
00493c32  jmp        0x493d59

// block 00493c37
00493c37  mov        dword ptr [ebp - 0x28], ecx
00493c3a  mov        dword ptr [ebp - 0x24], 0x4a456c
00493c41  jmp        0x493bb8

// block 00493c46
00493c46  mov        dword ptr [ebp - 0x24], 0x4a24bc
00493c4d  jmp        0x493c26

// block 00493c4f
00493c4f  mov        dword ptr [ebp - 0x28], ecx
00493c52  mov        dword ptr [ebp - 0x24], 0x4a24bc
00493c59  jmp        0x493bb8

// block 00493c5e
00493c5e  mov        dword ptr [ebp - 0x24], 0x4a4578
00493c65  jmp        0x493bed

// block 00493c67
00493c67  sub        ecx, 0x1a
00493c6a  je         0x493cba

// block 00493c6c
00493c6c  dec        ecx
00493c6d  je         0x493cae

// block 00493c6f
00493c6f  dec        ecx
00493c70  je         0x493ca2

// block 00493c72
00493c72  dec        ecx
00493c73  je         0x493c95

// block 00493c75
00493c75  sub        ecx, 0x1d
00493c78  je         0x493c8c

// block 00493c7a
00493c7a  sub        ecx, 3
00493c7d  jne        0x493d7e

// block 00493c83
00493c83  mov        dword ptr [ebp - 0x24], 0x4a4564
00493c8a  jmp        0x493c26

// block 00493c8c
00493c8c  mov        dword ptr [ebp - 0x24], 0x4a455c
00493c93  jmp        0x493c26

// block 00493c95
00493c95  mov        dword ptr [ebp - 0x24], 0x4a4578

// block 00493c9c
00493c9c  fld        qword ptr [edi]
00493c9e  fstp       qword ptr [esi]
00493ca0  jmp        0x493c26

// block 00493ca2
00493ca2  mov        dword ptr [ebp - 0x24], 0x4a4578
00493ca9  jmp        0x493c26

// block 00493cae
00493cae  mov        dword ptr [ebp - 0x28], 2
00493cb5  jmp        0x493bb1

// block 00493cba
00493cba  fld1       
00493cbc  jmp        0x493d7c

// block 00493cc1
00493cc1  mov        dword ptr [ebp - 0x28], 3
00493cc8  mov        dword ptr [ebp - 0x24], 0x4a4554
00493ccf  jmp        0x493bb8

// block 00493cd4
00493cd4  add        ecx, 0xfffffc18
00493cda  cmp        ecx, 0xc
00493cdd  ja         0x493d7e

// block 00493ce3
00493ce3  jmp        dword ptr [ecx*4 + 0x493d83]

// block 00493cea
00493cea  mov        dword ptr [ebp - 0x24], 0x4a24bc
00493cf1  jmp        0x493c9c

// block 00493cf3
00493cf3  mov        dword ptr [ebp - 0x24], 0x4a456c
00493cfa  jmp        0x493c9c

// block 00493cfc
00493cfc  mov        dword ptr [ebp - 0x24], 0x4a4574
00493d03  jmp        0x493c9c

// block 00493d05
00493d05  mov        dword ptr [ebp - 0x24], 0x4a454c
00493d0c  jmp        0x493c9c

// block 00493d0e
00493d0e  mov        dword ptr [ebp - 0x24], 0x4a4544
00493d15  jmp        0x493c9c

// block 00493d17
00493d17  mov        dword ptr [ebp - 0x24], 0x4a453c
00493d1e  jmp        0x493c9c

// block 00493d23
00493d23  mov        dword ptr [ebp - 0x24], 0x4a4534
00493d2a  jmp        0x493c9c

// block 00493d2f
00493d2f  mov        dword ptr [ebp - 0x24], 0x4a4530
00493d36  jmp        0x493d48

// block 00493d38
00493d38  mov        dword ptr [ebp - 0x24], 0x4a452c
00493d3f  jmp        0x493d48

// block 00493d41
00493d41  mov        dword ptr [ebp - 0x24], 0x4a4528

// block 00493d48
00493d48  fld        qword ptr [edi]
00493d4a  fmul       qword ptr [ebp - 8]
00493d4d  fst        qword ptr [esi]
00493d4f  fld        qword ptr [edi]
00493d51  fstp       qword ptr [ebp - 0x20]
00493d54  fld        qword ptr [ebx]
00493d56  fstp       qword ptr [ebp - 0x18]

// block 00493d59
00493d59  lea        ecx, [ebp - 0x28]
00493d5c  fstp       qword ptr [ebp - 0x10]
00493d5f  push       ecx
00493d60  mov        dword ptr [ebp - 0x28], 1
00493d67  call       eax

// block 00493d69
00493d69  pop        ecx
00493d6a  test       eax, eax
00493d6c  jne        0x493d79

// block 00493d6e
00493d6e  call       0x46e96f

// block 00493d73
00493d73  mov        dword ptr [eax], 0x21

// block 00493d79
00493d79  fld        qword ptr [ebp - 0x10]

// block 00493d7c
00493d7c  fstp       qword ptr [esi]

// block 00493d7e
00493d7e  pop        edi
00493d7f  pop        esi
00493d80  pop        ebx
00493d81  leave      
00493d82  ret        
