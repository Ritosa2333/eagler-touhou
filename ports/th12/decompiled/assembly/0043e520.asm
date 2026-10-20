
// block 0043e520
0043e520  push       -1
0043e522  push       0x496f0b
0043e527  mov        eax, dword ptr fs:[0]
0043e52d  push       eax
0043e52e  push       ebx
0043e52f  push       ebp
0043e530  push       esi
0043e531  push       edi
0043e532  mov        eax, dword ptr [0x4ad138]
0043e537  xor        eax, esp
0043e539  push       eax
0043e53a  lea        eax, [esp + 0x14]
0043e53e  mov        dword ptr fs:[0], eax
0043e544  mov        esi, dword ptr [esp + 0x24]
0043e548  xor        ebp, ebp
0043e54a  mov        dword ptr [esp + 0x1c], ebp
0043e54e  call       0x43e380

// block 0043e553
0043e553  mov        edi, dword ptr [esi + 8]
0043e556  mov        ebx, dword ptr [0x4ce89c]
0043e55c  cmp        edi, ebp
0043e55e  je         0x43e5a3

// block 0043e560
0043e560  test       dword ptr [0x4cee78], 0x8000
0043e56a  je         0x43e57d

// block 0043e56c
0043e56c  push       0x4cf0f8
0043e571  call       dword ptr [0x498088]

// block 0043e577
0043e577  inc        byte ptr [0x4cf218]

// block 0043e57d
0043e57d  mov        ecx, edi
0043e57f  mov        edx, ebx
0043e581  call       0x462890

// block 0043e586
0043e586  test       dword ptr [0x4cee78], 0x8000
0043e590  je         0x43e5a3

// block 0043e592
0043e592  push       0x4cf0f8
0043e597  call       dword ptr [0x49808c]

// block 0043e59d
0043e59d  dec        byte ptr [0x4cf218]

// block 0043e5a3
0043e5a3  mov        edi, dword ptr [esi + 0xc]
0043e5a6  mov        ebx, dword ptr [0x4ce89c]
0043e5ac  cmp        edi, ebp
0043e5ae  je         0x43e5f3

// block 0043e5b0
0043e5b0  test       dword ptr [0x4cee78], 0x8000
0043e5ba  je         0x43e5cd

// block 0043e5bc
0043e5bc  push       0x4cf0f8
0043e5c1  call       dword ptr [0x498088]

// block 0043e5c7
0043e5c7  inc        byte ptr [0x4cf218]

// block 0043e5cd
0043e5cd  mov        ecx, edi
0043e5cf  mov        edx, ebx
0043e5d1  call       0x462890

// block 0043e5d6
0043e5d6  test       dword ptr [0x4cee78], 0x8000
0043e5e0  je         0x43e5f3

// block 0043e5e2
0043e5e2  push       0x4cf0f8
0043e5e7  call       dword ptr [0x49808c]

// block 0043e5ed
0043e5ed  dec        byte ptr [0x4cf218]

// block 0043e5f3
0043e5f3  mov        eax, dword ptr [esi + 0x2e0]
0043e5f9  cmp        eax, ebp
0043e5fb  je         0x43e60c

// block 0043e5fd
0043e5fd  push       eax
0043e5fe  call       0x46c941

// block 0043e603
0043e603  add        esp, 4
0043e606  mov        dword ptr [esi + 0x2e0], ebp

// block 0043e60c
0043e60c  mov        edi, dword ptr [esi + 0x2d8]
0043e612  cmp        edi, ebp
0043e614  je         0x43e644

// block 0043e616
0043e616  mov        eax, dword ptr [edi + 0x8c]
0043e61c  mov        dword ptr [edi], 0x49fc28
0043e622  cmp        eax, ebp
0043e624  je         0x43e635

// block 0043e626
0043e626  push       eax
0043e627  call       0x46c941

// block 0043e62c
0043e62c  add        esp, 4
0043e62f  mov        dword ptr [edi + 0x8c], ebp

// block 0043e635
0043e635  push       edi
0043e636  call       0x46ca4f

// block 0043e63b
0043e63b  add        esp, 4
0043e63e  mov        dword ptr [esi + 0x2d8], ebp

// block 0043e644
0043e644  mov        ecx, dword ptr [esi + 0x2dc]
0043e64a  cmp        ecx, ebp
0043e64c  je         0x43e65d

// block 0043e64e
0043e64e  mov        eax, dword ptr [ecx]
0043e650  mov        edx, dword ptr [eax + 0x14]
0043e653  push       1
0043e655  call       edx

// block 0043e657
0043e657  mov        dword ptr [esi + 0x2dc], ebp

// block 0043e65d
0043e65d  mov        ebx, dword ptr [0x4ce8cc]
0043e663  mov        edi, dword ptr [ebx + 0x4b50d8]
0043e669  add        ebx, 0x4b50d8
0043e66f  cmp        edi, ebp
0043e671  je         0x43e685

// block 0043e673
0043e673  call       0x4604e0

// block 0043e678
0043e678  mov        eax, dword ptr [ebx]
0043e67a  push       eax
0043e67b  call       0x46ca4f

// block 0043e680
0043e680  add        esp, 4
0043e683  mov        dword ptr [ebx], ebp

// block 0043e685
0043e685  mov        ebx, dword ptr [0x4ce8cc]
0043e68b  mov        edi, dword ptr [ebx + 0x4b50d4]
0043e691  add        ebx, 0x4b50d4
0043e697  cmp        edi, ebp
0043e699  je         0x43e6ad

// block 0043e69b
0043e69b  call       0x4604e0

// block 0043e6a0
0043e6a0  mov        ecx, dword ptr [ebx]
0043e6a2  push       ecx
0043e6a3  call       0x46ca4f

// block 0043e6a8
0043e6a8  add        esp, 4
0043e6ab  mov        dword ptr [ebx], ebp

// block 0043e6ad
0043e6ad  add        esi, 0x10
0043e6b0  mov        dword ptr [0x4b4528], ebp
0043e6b6  mov        dword ptr [esi], 0x4a3738
0043e6bc  call       0x464c40

// block 0043e6c1
0043e6c1  mov        ecx, dword ptr [esp + 0x14]
0043e6c5  mov        dword ptr fs:[0], ecx
0043e6cc  pop        ecx
0043e6cd  pop        edi
0043e6ce  pop        esi
0043e6cf  pop        ebp
0043e6d0  pop        ebx
0043e6d1  add        esp, 0xc
0043e6d4  ret        4
