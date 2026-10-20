
// block 00421a60
00421a60  push       ebx
00421a61  push       esi
00421a62  push       edi
00421a63  mov        edi, dword ptr [0x4b43e4]
00421a69  mov        esi, dword ptr [edi + 0x6788]
00421a6f  inc        esi
00421a70  mov        eax, esi
00421a72  imul       eax, eax, 0x4b4
00421a78  lea        ebx, [eax + edi + 0x54b8]
00421a7f  mov        eax, dword ptr [ebx + 0x494]
00421a85  inc        esi
00421a86  test       eax, eax
00421a88  je         0x421a93

// block 00421a8a
00421a8a  mov        edx, 9
00421a8f  mov        ecx, ebx
00421a91  call       eax

// block 00421a93
00421a93  mov        ecx, 9
00421a98  push       ebx
00421a99  mov        word ptr [ebx + 0x3c4], cx
00421aa0  call       0x455630

// block 00421aa5
00421aa5  cmp        esi, 4
00421aa8  jl         0x421aaf

// block 00421aaa
00421aaa  mov        esi, 1

// block 00421aaf
00421aaf  mov        edx, esi
00421ab1  imul       edx, edx, 0x4b4
00421ab7  mov        eax, dword ptr [edx + edi + 0x594c]
00421abe  lea        ebx, [edx + edi + 0x54b8]
00421ac5  inc        esi
00421ac6  test       eax, eax
00421ac8  je         0x421ad3

// block 00421aca
00421aca  mov        edx, 7
00421acf  mov        ecx, ebx
00421ad1  call       eax

// block 00421ad3
00421ad3  mov        eax, 7
00421ad8  push       ebx
00421ad9  mov        word ptr [ebx + 0x3c4], ax
00421ae0  call       0x455630

// block 00421ae5
00421ae5  cmp        esi, 4
00421ae8  jl         0x421aef

// block 00421aea
00421aea  mov        esi, 1

// block 00421aef
00421aef  imul       esi, esi, 0x4b4
00421af5  mov        eax, dword ptr [esi + edi + 0x594c]
00421afc  lea        esi, [esi + edi + 0x54b8]
00421b03  test       eax, eax
00421b05  je         0x421b10

// block 00421b07
00421b07  mov        edx, 8
00421b0c  mov        ecx, esi
00421b0e  call       eax

// block 00421b10
00421b10  mov        ecx, 8
00421b15  push       esi
00421b16  mov        word ptr [esi + 0x3c4], cx
00421b1d  call       0x455630

// block 00421b22
00421b22  mov        eax, dword ptr [edi + 0x6788]
00421b28  inc        eax
00421b29  cdq        
00421b2a  mov        ecx, 3
00421b2f  idiv       ecx
00421b31  mov        dword ptr [edi + 0x6788], edx
00421b37  pop        edi
00421b38  pop        esi
00421b39  pop        ebx
00421b3a  ret        
