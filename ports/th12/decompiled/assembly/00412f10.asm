
// block 00412f10
00412f10  push       ecx
00412f11  push       ebx
00412f12  push       ebp
00412f13  push       esi
00412f14  push       edi
00412f15  mov        edi, eax
00412f17  push       edi
00412f18  call       0x40e940

// block 00412f1d
00412f1d  mov        esi, dword ptr [edi + 8]
00412f20  mov        ebx, dword ptr [0x4ce89c]
00412f26  mov        ebp, dword ptr [0x498088]
00412f2c  test       esi, esi
00412f2e  je         0x412f6f

// block 00412f30
00412f30  test       dword ptr [0x4cee78], 0x8000
00412f3a  je         0x412f49

// block 00412f3c
00412f3c  push       0x4cf0f8
00412f41  call       ebp

// block 00412f43
00412f43  inc        byte ptr [0x4cf218]

// block 00412f49
00412f49  mov        ecx, esi
00412f4b  mov        edx, ebx
00412f4d  call       0x462890

// block 00412f52
00412f52  test       dword ptr [0x4cee78], 0x8000
00412f5c  je         0x412f6f

// block 00412f5e
00412f5e  push       0x4cf0f8
00412f63  call       dword ptr [0x49808c]

// block 00412f69
00412f69  dec        byte ptr [0x4cf218]

// block 00412f6f
00412f6f  mov        esi, dword ptr [edi + 0xc]
00412f72  mov        ebx, dword ptr [0x4ce89c]
00412f78  test       esi, esi
00412f7a  je         0x412fbb

// block 00412f7c
00412f7c  test       dword ptr [0x4cee78], 0x8000
00412f86  je         0x412f95

// block 00412f88
00412f88  push       0x4cf0f8
00412f8d  call       ebp

// block 00412f8f
00412f8f  inc        byte ptr [0x4cf218]

// block 00412f95
00412f95  mov        ecx, esi
00412f97  mov        edx, ebx
00412f99  call       0x462890

// block 00412f9e
00412f9e  test       dword ptr [0x4cee78], 0x8000
00412fa8  je         0x412fbb

// block 00412faa
00412faa  push       0x4cf0f8
00412faf  call       dword ptr [0x49808c]

// block 00412fb5
00412fb5  dec        byte ptr [0x4cf218]

// block 00412fbb
00412fbb  mov        esi, 0xc

// block 00412fc0
00412fc0  mov        eax, dword ptr [edi + 0x64]
00412fc3  mov        eax, dword ptr [esi + eax]
00412fc6  test       eax, eax
00412fc8  je         0x412fd3

// block 00412fca
00412fca  push       eax
00412fcb  call       0x46c941

// block 00412fd0
00412fd0  add        esp, 4

// block 00412fd3
00412fd3  add        esi, 4
00412fd6  cmp        esi, 0x8c
00412fdc  jl         0x412fc0

// block 00412fde
00412fde  mov        esi, dword ptr [edi + 0x64]
00412fe1  test       esi, esi
00412fe3  je         0x413011

// block 00412fe5
00412fe5  mov        eax, dword ptr [esi + 0x8c]
00412feb  mov        dword ptr [esi], 0x49fc28
00412ff1  test       eax, eax
00412ff3  je         0x413008

// block 00412ff5
00412ff5  push       eax
00412ff6  call       0x46c941

// block 00412ffb
00412ffb  add        esp, 4
00412ffe  mov        dword ptr [esi + 0x8c], 0

// block 00413008
00413008  push       esi
00413009  call       0x46ca4f

// block 0041300e
0041300e  add        esp, 4

// block 00413011
00413011  mov        dword ptr [edi + 0x64], 0
00413018  mov        ebp, 8
0041301d  mov        ebx, 0x4b50e0
00413022  mov        dword ptr [esp + 0x10], 4
0041302a  lea        ebx, [ebx]

// block 00413030
00413030  test       ebp, ebp
00413032  jl         0x41305f

// block 00413034
00413034  cmp        ebp, 0x20
00413037  jae        0x41305f

// block 00413039
00413039  mov        ecx, dword ptr [0x4ce8cc]
0041303f  mov        edi, dword ptr [ebx + ecx]
00413042  lea        esi, [ebx + ecx]
00413045  test       edi, edi
00413047  je         0x41305f

// block 00413049
00413049  call       0x4604e0

// block 0041304e
0041304e  mov        edx, dword ptr [esi]
00413050  push       edx
00413051  call       0x46ca4f

// block 00413056
00413056  add        esp, 4
00413059  mov        dword ptr [esi], 0

// block 0041305f
0041305f  add        ebx, 4
00413062  inc        ebp
00413063  sub        dword ptr [esp + 0x10], 1
00413068  jne        0x413030

// block 0041306a
0041306a  pop        edi
0041306b  pop        esi
0041306c  pop        ebp
0041306d  mov        dword ptr [0x4b43dc], 0
00413077  pop        ebx
00413078  pop        ecx
00413079  ret        
