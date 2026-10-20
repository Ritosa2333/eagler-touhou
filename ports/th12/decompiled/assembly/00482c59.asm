
// block 00482c59
00482c59  push       0x20
00482c5b  push       0x4aaf58
00482c60  call       0x46fcec

// block 00482c65
00482c65  xor        edi, edi
00482c67  mov        dword ptr [ebp - 0x1c], edi
00482c6a  mov        dword ptr [ebp - 0x28], edi
00482c6d  mov        ebx, dword ptr [ebp + 8]
00482c70  cmp        ebx, 0xb
00482c73  jg         0x482cc1

// block 00482c75
00482c75  je         0x482c8c

// block 00482c77
00482c77  mov        eax, ebx
00482c79  push       2
00482c7b  pop        ecx
00482c7c  sub        eax, ecx
00482c7e  je         0x482ca2

// block 00482c80
00482c80  sub        eax, ecx
00482c82  je         0x482c8c

// block 00482c84
00482c84  sub        eax, ecx
00482c86  je         0x482cec

// block 00482c88
00482c88  sub        eax, ecx
00482c8a  jne        0x482cd0

// block 00482c8c
00482c8c  call       0x4753ee

// block 00482c91
00482c91  mov        edi, eax
00482c93  mov        dword ptr [ebp - 0x28], edi
00482c96  test       edi, edi
00482c98  jne        0x482cae

// block 00482c9a
00482c9a  or         eax, 0xffffffff
00482c9d  jmp        0x482e03

// block 00482ca2
00482ca2  mov        esi, 0x4b4368
00482ca7  mov        eax, dword ptr [0x4b4368]
00482cac  jmp        0x482d0e

// block 00482cae
00482cae  push       dword ptr [edi + 0x5c]
00482cb1  mov        edx, ebx
00482cb3  call       0x4829c6

// block 00482cb8
00482cb8  mov        esi, eax
00482cba  add        esi, 8
00482cbd  mov        eax, dword ptr [esi]
00482cbf  jmp        0x482d1b

// block 00482cc1
00482cc1  mov        eax, ebx
00482cc3  sub        eax, 0xf
00482cc6  je         0x482d04

// block 00482cc8
00482cc8  sub        eax, 6
00482ccb  je         0x482cf8

// block 00482ccd
00482ccd  dec        eax
00482cce  je         0x482cec

// block 00482cd0
00482cd0  call       0x46e96f

// block 00482cd5
00482cd5  mov        dword ptr [eax], 0x16
00482cdb  xor        eax, eax
00482cdd  push       eax
00482cde  push       eax
00482cdf  push       eax
00482ce0  push       eax
00482ce1  push       eax
00482ce2  call       0x470f2d

// block 00482ce7
00482ce7  add        esp, 0x14
00482cea  jmp        0x482c9a

// block 00482cec
00482cec  mov        esi, 0x4b4370
00482cf1  mov        eax, dword ptr [0x4b4370]
00482cf6  jmp        0x482d0e

// block 00482cf8
00482cf8  mov        esi, 0x4b436c
00482cfd  mov        eax, dword ptr [0x4b436c]
00482d02  jmp        0x482d0e

// block 00482d04
00482d04  mov        esi, 0x4b4374
00482d09  mov        eax, dword ptr [0x4b4374]

// block 00482d0e
00482d0e  mov        dword ptr [ebp - 0x1c], 1
00482d15  push       eax
00482d16  call       0x4751de

// block 00482d1b
00482d1b  mov        dword ptr [ebp - 0x20], eax
00482d1e  pop        ecx
00482d1f  xor        eax, eax
00482d21  cmp        dword ptr [ebp - 0x20], 1
00482d25  je         0x482e03

// block 00482d2b
00482d2b  cmp        dword ptr [ebp - 0x20], eax
00482d2e  jne        0x482d37

// block 00482d30
00482d30  push       3
00482d32  call       0x472f0b

// block 00482d37
00482d37  cmp        dword ptr [ebp - 0x1c], eax
00482d3a  je         0x482d43

// block 00482d3c
00482d3c  push       eax
00482d3d  call       0x46ec9a

// block 00482d42
00482d42  pop        ecx

// block 00482d43
00482d43  xor        eax, eax
00482d45  mov        dword ptr [ebp - 4], eax
00482d48  cmp        ebx, 8
00482d4b  je         0x482d57

// block 00482d4d
00482d4d  cmp        ebx, 0xb
00482d50  je         0x482d57

// block 00482d52
00482d52  cmp        ebx, 4
00482d55  jne        0x482d72

// block 00482d57
00482d57  mov        ecx, dword ptr [edi + 0x60]
00482d5a  mov        dword ptr [ebp - 0x2c], ecx
00482d5d  mov        dword ptr [edi + 0x60], eax
00482d60  cmp        ebx, 8
00482d63  jne        0x482da5

// block 00482d65
00482d65  mov        ecx, dword ptr [edi + 0x64]
00482d68  mov        dword ptr [ebp - 0x30], ecx
00482d6b  mov        dword ptr [edi + 0x64], 0x8c

// block 00482d72
00482d72  cmp        ebx, 8
00482d75  jne        0x482da5

// block 00482d77
00482d77  mov        ecx, dword ptr [0x4adb90]
00482d7d  mov        dword ptr [ebp - 0x24], ecx

// block 00482d80
00482d80  mov        ecx, dword ptr [0x4adb94]
00482d86  mov        edx, dword ptr [0x4adb90]
00482d8c  add        ecx, edx
00482d8e  cmp        dword ptr [ebp - 0x24], ecx
00482d91  jge        0x482dac

// block 00482d93
00482d93  mov        ecx, dword ptr [ebp - 0x24]
00482d96  imul       ecx, ecx, 0xc
00482d99  mov        edx, dword ptr [edi + 0x5c]
00482d9c  mov        dword ptr [ecx + edx + 8], eax
00482da0  inc        dword ptr [ebp - 0x24]
00482da3  jmp        0x482d80

// block 00482da5
00482da5  call       0x4751d5

// block 00482daa
00482daa  mov        dword ptr [esi], eax

// block 00482dac
00482dac  mov        dword ptr [ebp - 4], 0xfffffffe
00482db3  call       0x482dcd

// block 00482db8
00482db8  cmp        ebx, 8
00482dbb  jne        0x482ddc

// block 00482dbd
00482dbd  push       dword ptr [edi + 0x64]
00482dc0  push       ebx
00482dc1  call       dword ptr [ebp - 0x20]

// block 00482dc4
00482dc4  pop        ecx
00482dc5  jmp        0x482de0

// block 00482ddc
00482ddc  push       ebx
00482ddd  call       dword ptr [ebp - 0x20]

// block 00482de0
00482de0  pop        ecx
00482de1  cmp        ebx, 8
00482de4  je         0x482df0

// block 00482de6
00482de6  cmp        ebx, 0xb
00482de9  je         0x482df0

// block 00482deb
00482deb  cmp        ebx, 4
00482dee  jne        0x482e01

// block 00482df0
00482df0  mov        eax, dword ptr [ebp - 0x2c]
00482df3  mov        dword ptr [edi + 0x60], eax
00482df6  cmp        ebx, 8
00482df9  jne        0x482e01

// block 00482dfb
00482dfb  mov        eax, dword ptr [ebp - 0x30]
00482dfe  mov        dword ptr [edi + 0x64], eax

// block 00482e01
00482e01  xor        eax, eax

// block 00482e03
00482e03  call       0x46fd31

// block 00482e08
00482e08  ret        
