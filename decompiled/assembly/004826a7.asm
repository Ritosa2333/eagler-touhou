
// block 004826a7
004826a7  mov        edi, edi
004826a9  push       ebp
004826aa  mov        ebp, esp
004826ac  sub        esp, 0x18
004826af  mov        edx, dword ptr [0x4b4318]
004826b5  movsx      eax, byte ptr [edx]
004826b8  push       ebx
004826b9  xor        ebx, ebx
004826bb  mov        ecx, 0xffff0000
004826c0  and        dword ptr [ebp - 4], ecx
004826c3  sub        eax, ebx
004826c5  push       esi
004826c6  mov        dword ptr [ebp - 8], ebx
004826c9  je         0x4827c8

// block 004826cf
004826cf  sub        eax, 0x24
004826d2  je         0x482736

// block 004826d4
004826d4  sub        eax, 0x1d
004826d7  mov        esi, dword ptr [ebp + 0xc]
004826da  je         0x48270a

// block 004826dc
004826dc  dec        eax
004826dd  je         0x4826ef

// block 004826df
004826df  push       esi
004826e0  push       dword ptr [ebp + 8]
004826e3  call       0x4822f0

// block 004826e8
004826e8  pop        ecx
004826e9  pop        ecx
004826ea  jmp        0x4827d8

// block 004826ef
004826ef  push       0x49e094
004826f4  lea        ecx, [ebp - 8]
004826f7  call       0x47e896

// block 004826fc
004826fc  cmp        dword ptr [esi], ebx
004826fe  je         0x48270a

// block 00482700
00482700  push       0x20
00482702  lea        ecx, [ebp - 8]
00482705  call       0x47e9f6

// block 0048270a
0048270a  mov        eax, dword ptr [esi]
0048270c  mov        esi, dword ptr [esi + 4]
0048270f  inc        dword ptr [0x4b4318]
00482715  mov        dword ptr [ebp - 0x10], eax
00482718  lea        eax, [ebp - 0x10]
0048271b  push       eax
0048271c  lea        eax, [ebp - 8]
0048271f  push       eax
00482720  push       dword ptr [ebp + 8]
00482723  or         esi, 0x100
00482729  mov        dword ptr [ebp - 0xc], esi
0048272c  call       0x48219f

// block 00482731
00482731  jmp        0x4827d5

// block 00482736
00482736  mov        al, byte ptr [edx + 1]
00482739  cmp        al, 0x24
0048273b  je         0x482754

// block 0048273d
0048273d  cmp        al, bl
0048273f  je         0x4827c8

// block 00482745
00482745  mov        ecx, dword ptr [ebp + 8]
00482748  push       2
0048274a  call       0x47e1ce

// block 0048274f
0048274f  jmp        0x4827d8

// block 00482754
00482754  inc        edx
00482755  inc        edx
00482756  mov        dword ptr [0x4b4318], edx
0048275c  movsx      eax, byte ptr [edx]
0048275f  sub        eax, ebx
00482761  je         0x4827c8

// block 00482763
00482763  sub        eax, 0x41
00482766  je         0x4827b1

// block 00482768
00482768  dec        eax
00482769  je         0x48279b

// block 0048276b
0048276b  dec        eax
0048276c  jne        0x482745

// block 0048276e
0048276e  and        dword ptr [ebp - 4], ecx
00482771  push       ebx
00482772  lea        eax, [ebp - 8]
00482775  push       eax
00482776  push       ebx
00482777  push       dword ptr [ebp + 0xc]
0048277a  lea        eax, [ebp - 0x18]
0048277d  inc        edx
0048277e  push       eax
0048277f  mov        dword ptr [0x4b4318], edx
00482785  mov        dword ptr [ebp - 8], ebx
00482788  call       0x481a74

// block 0048278d
0048278d  push       eax
0048278e  push       dword ptr [ebp + 8]
00482791  call       0x4822f0

// block 00482796
00482796  add        esp, 0x1c
00482799  jmp        0x4827d8

// block 0048279b
0048279b  push       1
0048279d  push       dword ptr [ebp + 0xc]
004827a0  inc        edx
004827a1  push       dword ptr [ebp + 8]
004827a4  mov        dword ptr [0x4b4318], edx
004827aa  call       0x47f89d

// block 004827af
004827af  jmp        0x4827d5

// block 004827b1
004827b1  push       dword ptr [ebp + 0xc]
004827b4  inc        edx
004827b5  push       dword ptr [ebp + 8]
004827b8  mov        dword ptr [0x4b4318], edx
004827be  call       0x481720

// block 004827c3
004827c3  jmp        0x4826e8

// block 004827c8
004827c8  push       dword ptr [ebp + 0xc]
004827cb  push       1
004827cd  push       dword ptr [ebp + 8]
004827d0  call       0x47ec26

// block 004827d5
004827d5  add        esp, 0xc

// block 004827d8
004827d8  mov        eax, dword ptr [ebp + 8]
004827db  pop        esi
004827dc  pop        ebx
004827dd  leave      
004827de  ret        
