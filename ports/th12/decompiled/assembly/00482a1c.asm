
// block 00482a1c
00482a1c  push       0x10
00482a1e  push       0x4aaf38
00482a23  call       0x46fcec

// block 00482a28
00482a28  and        dword ptr [ebp - 0x20], 0
00482a2c  mov        esi, dword ptr [ebp + 0xc]
00482a2f  mov        ebx, dword ptr [ebp + 8]
00482a32  cmp        esi, 4
00482a35  je         0x482c1d

// block 00482a3b
00482a3b  cmp        esi, 3
00482a3e  je         0x482c1d

// block 00482a44
00482a44  push       2
00482a46  pop        edi
00482a47  cmp        ebx, edi
00482a49  je         0x482b0e

// block 00482a4f
00482a4f  cmp        ebx, 0x15
00482a52  je         0x482b0e

// block 00482a58
00482a58  cmp        ebx, 0x16
00482a5b  je         0x482b0e

// block 00482a61
00482a61  cmp        ebx, 6
00482a64  je         0x482b0e

// block 00482a6a
00482a6a  cmp        ebx, 0xf
00482a6d  je         0x482b0e

// block 00482a73
00482a73  cmp        ebx, 8
00482a76  je         0x482a86

// block 00482a78
00482a78  cmp        ebx, 4
00482a7b  je         0x482a86

// block 00482a7d
00482a7d  cmp        ebx, 0xb
00482a80  jne        0x482c1d

// block 00482a86
00482a86  call       0x4753ee

// block 00482a8b
00482a8b  mov        esi, eax
00482a8d  test       esi, esi
00482a8f  je         0x482c1d

// block 00482a95
00482a95  mov        edi, 0x49d5c8
00482a9a  cmp        dword ptr [esi + 0x5c], edi
00482a9d  jne        0x482ac6

// block 00482a9f
00482a9f  push       dword ptr [0x4adb98]
00482aa5  call       0x4777ce

// block 00482aaa
00482aaa  pop        ecx
00482aab  mov        dword ptr [esi + 0x5c], eax
00482aae  test       eax, eax
00482ab0  je         0x482c1d

// block 00482ab6
00482ab6  push       dword ptr [0x4adb98]
00482abc  push       edi
00482abd  push       eax
00482abe  call       0x47a330

// block 00482ac3
00482ac3  add        esp, 0xc

// block 00482ac6
00482ac6  push       dword ptr [esi + 0x5c]
00482ac9  mov        edx, ebx
00482acb  call       0x4829c6

// block 00482ad0
00482ad0  pop        ecx
00482ad1  test       eax, eax
00482ad3  je         0x482c1d

// block 00482ad9
00482ad9  mov        edx, dword ptr [eax + 8]
00482adc  mov        ecx, dword ptr [ebp + 0xc]
00482adf  cmp        ecx, 2
00482ae2  je         0x482c0d

// block 00482ae8
00482ae8  jmp        0x482b04

// block 00482aea
00482aea  mov        dword ptr [eax + 8], ecx
00482aed  add        eax, 0xc
00482af0  mov        edi, dword ptr [0x4adb9c]
00482af6  imul       edi, edi, 0xc
00482af9  add        edi, dword ptr [esi + 0x5c]
00482afc  cmp        eax, edi
00482afe  jae        0x482c0d

// block 00482b04
00482b04  cmp        dword ptr [eax + 4], ebx
00482b07  je         0x482aea

// block 00482b09
00482b09  jmp        0x482c0d

// block 00482b0e
00482b0e  push       0
00482b10  call       0x46ec9a

// block 00482b15
00482b15  pop        ecx
00482b16  and        dword ptr [ebp - 4], 0
00482b1a  cmp        ebx, edi
00482b1c  je         0x482b23

// block 00482b1e
00482b1e  cmp        ebx, 0x15
00482b21  jne        0x482b61

// block 00482b23
00482b23  cmp        dword ptr [0x4b4378], 0
00482b2a  jne        0x482b61

// block 00482b2c
00482b2c  push       1
00482b2e  push       0x48292b
00482b33  call       dword ptr [0x498064]

// block 00482b39
00482b39  xor        ecx, ecx
00482b3b  inc        ecx
00482b3c  cmp        eax, ecx
00482b3e  jne        0x482b48

// block 00482b40
00482b40  mov        dword ptr [0x4b4378], ecx
00482b46  jmp        0x482b61

// block 00482b48
00482b48  call       0x46e982

// block 00482b4d
00482b4d  mov        esi, eax
00482b4f  call       dword ptr [0x4980e4]

// block 00482b55
00482b55  mov        dword ptr [esi], eax
00482b57  mov        dword ptr [ebp - 0x20], 1
00482b5e  mov        esi, dword ptr [ebp + 0xc]

// block 00482b61
00482b61  mov        eax, ebx
00482b63  sub        eax, edi
00482b65  je         0x482bd9

// block 00482b67
00482b67  sub        eax, 4
00482b6a  je         0x482b79

// block 00482b6c
00482b6c  sub        eax, 9
00482b6f  je         0x482bb9

// block 00482b71
00482b71  sub        eax, 6
00482b74  je         0x482b99

// block 00482b76
00482b76  dec        eax
00482b77  jne        0x482bf8

// block 00482b79
00482b79  push       dword ptr [0x4b4370]
00482b7f  call       0x4751de

// block 00482b84
00482b84  pop        ecx
00482b85  mov        dword ptr [ebp - 0x1c], eax
00482b88  cmp        esi, edi
00482b8a  je         0x482bf8

// block 00482b8c
00482b8c  push       esi
00482b8d  call       0x475163

// block 00482b92
00482b92  mov        dword ptr [0x4b4370], eax
00482b97  jmp        0x482bf7

// block 00482b99
00482b99  push       dword ptr [0x4b436c]
00482b9f  call       0x4751de

// block 00482ba4
00482ba4  pop        ecx
00482ba5  mov        dword ptr [ebp - 0x1c], eax
00482ba8  cmp        esi, edi
00482baa  je         0x482bf8

// block 00482bac
00482bac  push       esi
00482bad  call       0x475163

// block 00482bb2
00482bb2  mov        dword ptr [0x4b436c], eax
00482bb7  jmp        0x482bf7

// block 00482bb9
00482bb9  push       dword ptr [0x4b4374]
00482bbf  call       0x4751de

// block 00482bc4
00482bc4  pop        ecx
00482bc5  mov        dword ptr [ebp - 0x1c], eax
00482bc8  cmp        esi, edi
00482bca  je         0x482bf8

// block 00482bcc
00482bcc  push       esi
00482bcd  call       0x475163

// block 00482bd2
00482bd2  mov        dword ptr [0x4b4374], eax
00482bd7  jmp        0x482bf7

// block 00482bd9
00482bd9  push       dword ptr [0x4b4368]
00482bdf  call       0x4751de

// block 00482be4
00482be4  pop        ecx
00482be5  mov        dword ptr [ebp - 0x1c], eax
00482be8  cmp        esi, edi
00482bea  je         0x482bf8

// block 00482bec
00482bec  push       esi
00482bed  call       0x475163

// block 00482bf2
00482bf2  mov        dword ptr [0x4b4368], eax

// block 00482bf7
00482bf7  pop        ecx

// block 00482bf8
00482bf8  mov        dword ptr [ebp - 4], 0xfffffffe
00482bff  call       0x482c14

// block 00482c04
00482c04  cmp        dword ptr [ebp - 0x20], 0
00482c08  jne        0x482c1d

// block 00482c0a
00482c0a  mov        edx, dword ptr [ebp - 0x1c]

// block 00482c0d
00482c0d  mov        eax, edx
00482c0f  jmp        0x482c53

// block 00482c1d
00482c1d  cmp        ebx, 1
00482c20  je         0x482c50

// block 00482c22
00482c22  cmp        ebx, 3
00482c25  je         0x482c50

// block 00482c27
00482c27  cmp        ebx, 0xd
00482c2a  je         0x482c50

// block 00482c2c
00482c2c  cmp        ebx, 0xf
00482c2f  jle        0x482c36

// block 00482c31
00482c31  cmp        ebx, 0x11
00482c34  jle        0x482c50

// block 00482c36
00482c36  call       0x46e96f

// block 00482c3b
00482c3b  mov        dword ptr [eax], 0x16
00482c41  xor        eax, eax
00482c43  push       eax
00482c44  push       eax
00482c45  push       eax
00482c46  push       eax
00482c47  push       eax
00482c48  call       0x470f2d

// block 00482c4d
00482c4d  add        esp, 0x14

// block 00482c50
00482c50  or         eax, 0xffffffff

// block 00482c53
00482c53  call       0x46fd31

// block 00482c58
00482c58  ret        
