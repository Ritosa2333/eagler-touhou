
// block 00406930
00406930  push       -1
00406932  push       0x4975db
00406937  mov        eax, dword ptr fs:[0]
0040693d  push       eax
0040693e  push       ebx
0040693f  push       ebp
00406940  push       esi
00406941  push       edi
00406942  mov        eax, dword ptr [0x4ad138]
00406947  xor        eax, esp
00406949  push       eax
0040694a  lea        eax, [esp + 0x14]
0040694e  mov        dword ptr fs:[0], eax
00406954  mov        edi, dword ptr [esp + 0x24]
00406958  xor        ebp, ebp
0040695a  mov        dword ptr [esp + 0x1c], ebp
0040695e  cmp        dword ptr [edi + 0x3c], ebp
00406961  je         0x40698f

// block 00406963
00406963  mov        eax, dword ptr [0x4b0c94]
00406968  mov        ecx, dword ptr [0x4b0c90]
0040696e  lea        edx, [eax + ecx*2]
00406971  cmp        edx, 5
00406974  jne        0x40698f

// block 00406976
00406976  mov        eax, dword ptr [edi + 0x40]
00406979  push       eax
0040697a  call       0x461a70

// block 0040697f
0040697f  lea        ebx, [ebp + 0xa]
00406982  mov        eax, 0x4b0c40
00406987  mov        dword ptr [edi + 0x40], ebp
0040698a  call       0x422d70

// block 0040698f
0040698f  mov        ecx, dword ptr [edi + 0x40]
00406992  push       ecx
00406993  call       0x461a70

// block 00406998
00406998  mov        dword ptr [edi + 0x40], ebp
0040699b  mov        edx, dword ptr [edi + 0x44]
0040699e  push       edx
0040699f  call       0x461a70

// block 004069a4
004069a4  mov        dword ptr [edi + 0x44], ebp
004069a7  mov        eax, dword ptr [edi + 0x48]
004069aa  push       eax
004069ab  call       0x461a70

// block 004069b0
004069b0  mov        ebx, dword ptr [0x4ce89c]
004069b6  mov        dword ptr [edi + 0x48], ebp
004069b9  mov        esi, dword ptr [edi + 8]
004069bc  cmp        esi, ebp
004069be  je         0x406a03

// block 004069c0
004069c0  test       dword ptr [0x4cee78], 0x8000
004069ca  je         0x4069dd

// block 004069cc
004069cc  push       0x4cf0f8
004069d1  call       dword ptr [0x498088]

// block 004069d7
004069d7  inc        byte ptr [0x4cf218]

// block 004069dd
004069dd  mov        ecx, esi
004069df  mov        edx, ebx
004069e1  call       0x462890

// block 004069e6
004069e6  test       dword ptr [0x4cee78], 0x8000
004069f0  je         0x406a03

// block 004069f2
004069f2  push       0x4cf0f8
004069f7  call       dword ptr [0x49808c]

// block 004069fd
004069fd  dec        byte ptr [0x4cf218]

// block 00406a03
00406a03  mov        esi, dword ptr [edi + 0xc]
00406a06  mov        ebx, dword ptr [0x4ce89c]
00406a0c  cmp        esi, ebp
00406a0e  je         0x406a53

// block 00406a10
00406a10  test       dword ptr [0x4cee78], 0x8000
00406a1a  je         0x406a2d

// block 00406a1c
00406a1c  push       0x4cf0f8
00406a21  call       dword ptr [0x498088]

// block 00406a27
00406a27  inc        byte ptr [0x4cf218]

// block 00406a2d
00406a2d  mov        ecx, esi
00406a2f  mov        edx, ebx
00406a31  call       0x462890

// block 00406a36
00406a36  test       dword ptr [0x4cee78], 0x8000
00406a40  je         0x406a53

// block 00406a42
00406a42  push       0x4cf0f8
00406a47  call       dword ptr [0x49808c]

// block 00406a4d
00406a4d  dec        byte ptr [0x4cf218]

// block 00406a53
00406a53  mov        esi, dword ptr [edi + 0x520]
00406a59  mov        ebx, dword ptr [0x4ce89c]
00406a5f  cmp        esi, ebp
00406a61  je         0x406aa6

// block 00406a63
00406a63  test       dword ptr [0x4cee78], 0x8000
00406a6d  je         0x406a80

// block 00406a6f
00406a6f  push       0x4cf0f8
00406a74  call       dword ptr [0x498088]

// block 00406a7a
00406a7a  inc        byte ptr [0x4cf218]

// block 00406aa6
00406aa6  mov        esi, dword ptr [edi + 0x504]
00406aac  mov        dword ptr [0x4b43c4], ebp
00406ab2  cmp        esi, ebp
00406ab4  je         0x406ac4

// block 00406ab6
00406ab6  call       0x402870

// block 00406abb
00406abb  push       esi
00406abc  call       0x46ca4f

// block 00406ac1
00406ac1  add        esp, 4

// block 00406ac4
00406ac4  mov        eax, dword ptr [edi + 0x500]
00406aca  mov        dword ptr [edi + 0x504], ebp
00406ad0  cmp        eax, ebp
00406ad2  je         0x406ae3

// block 00406ad4
00406ad4  push       eax
00406ad5  call       0x46c941

// block 00406ada
00406ada  add        esp, 4
00406add  mov        dword ptr [edi + 0x500], ebp

// block 00406ae3
00406ae3  mov        eax, dword ptr [edi + 0x4c4]
00406ae9  cmp        eax, ebp
00406aeb  je         0x406af6

// block 00406aed
00406aed  push       eax
00406aee  call       0x46c941

// block 00406af3
00406af3  add        esp, 4

// block 00406af6
00406af6  mov        dword ptr [edi + 0x4c4], ebp
00406afc  mov        ecx, dword ptr [esp + 0x14]
00406b00  mov        dword ptr fs:[0], ecx
00406b07  pop        ecx
00406b08  pop        edi
00406b09  pop        esi
00406b0a  pop        ebp
00406b0b  pop        ebx
00406b0c  add        esp, 0xc
00406b0f  ret        4
