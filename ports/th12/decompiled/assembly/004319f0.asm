
// block 004319f0
004319f0  mov        ecx, dword ptr [0x4d4754]
004319f6  test       ecx, ecx
004319f8  je         0x431ada

// block 004319fe
004319fe  cmp        dword ptr [ecx + 0x1c], 1
00431a02  lea        edx, [ecx + 0x1c]
00431a05  push       esi
00431a06  mov        esi, ecx
00431a08  jne        0x431a49

// block 00431a0a
00431a0a  mov        eax, dword ptr [ecx + 0x14]
00431a0d  dec        eax
00431a0e  mov        dword ptr [ecx + 0x14], eax
00431a11  test       eax, eax
00431a13  jg         0x431a2a

// block 00431a15
00431a15  mov        eax, dword ptr [esi + 4]
00431a18  mov        dword ptr [edx], 0
00431a1e  mov        eax, dword ptr [eax]
00431a20  mov        ecx, dword ptr [eax]
00431a22  mov        edx, dword ptr [ecx + 0x48]
00431a25  push       eax
00431a26  call       edx

// block 00431a28
00431a28  jmp        0x431a43

// block 00431a2a
00431a2a  imul       eax, eax, 0x1388
00431a30  cdq        
00431a31  idiv       dword ptr [esi + 0x18]
00431a34  mov        ecx, eax
00431a36  sub        ecx, 0x1388
00431a3c  mov        eax, esi
00431a3e  call       0x4663e0

// block 00431a43
00431a43  mov        ecx, dword ptr [0x4d4754]

// block 00431a49
00431a49  cmp        dword ptr [ecx + 0x1c], 2
00431a4d  lea        edx, [ecx + 0x1c]
00431a50  mov        esi, ecx
00431a52  jne        0x431a80

// block 00431a54
00431a54  mov        eax, dword ptr [ecx + 0x14]
00431a57  dec        eax
00431a58  mov        dword ptr [ecx + 0x14], eax
00431a5b  test       eax, eax
00431a5d  jg         0x431a67

// block 00431a5f
00431a5f  mov        dword ptr [edx], 0
00431a65  jmp        0x431a7a

// block 00431a67
00431a67  imul       eax, eax, 0xffffec78
00431a6d  cdq        
00431a6e  idiv       dword ptr [esi + 0x18]
00431a71  mov        ecx, eax
00431a73  mov        eax, esi
00431a75  call       0x4663e0

// block 00431a7a
00431a7a  mov        ecx, dword ptr [0x4d4754]

// block 00431a80
00431a80  cmp        dword ptr [ecx + 0x1c], 4
00431a84  lea        edx, [ecx + 0x1c]
00431a87  mov        esi, ecx
00431a89  jne        0x431abd

// block 00431a8b
00431a8b  mov        eax, dword ptr [ecx + 0x14]
00431a8e  dec        eax
00431a8f  mov        dword ptr [ecx + 0x14], eax
00431a92  test       eax, eax
00431a94  jg         0x431a9e

// block 00431a96
00431a96  mov        dword ptr [edx], 0
00431a9c  jmp        0x431ab7

// block 00431a9e
00431a9e  imul       eax, eax, 0x3e8
00431aa4  cdq        
00431aa5  idiv       dword ptr [esi + 0x18]
00431aa8  mov        ecx, eax
00431aaa  sub        ecx, 0x3e8
00431ab0  mov        eax, esi
00431ab2  call       0x4663e0

// block 00431ab7
00431ab7  mov        ecx, dword ptr [0x4d4754]

// block 00431abd
00431abd  cmp        dword ptr [ecx + 0x1c], 3
00431ac1  lea        edx, [ecx + 0x1c]
00431ac4  mov        esi, ecx
00431ac6  jne        0x431ad9

// block 00431ac8
00431ac8  mov        eax, dword ptr [ecx + 0x14]
00431acb  dec        eax
00431acc  mov        dword ptr [ecx + 0x14], eax
00431acf  test       eax, eax
00431ad1  jg         0x431adb

// block 00431ad3
00431ad3  mov        dword ptr [edx], 0

// block 00431ad9
00431ad9  pop        esi

// block 00431ada
00431ada  ret        

// block 00431adb
00431adb  imul       eax, eax, 0xfffffc18
00431ae1  cdq        
00431ae2  idiv       dword ptr [esi + 0x18]
00431ae5  mov        ecx, eax
00431ae7  mov        eax, esi
00431ae9  pop        esi
00431aea  jmp        0x4663e0
