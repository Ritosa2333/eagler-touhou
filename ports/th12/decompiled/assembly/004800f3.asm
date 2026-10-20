
// block 004800f3
004800f3  mov        edi, edi
004800f5  push       ebp
004800f6  mov        ebp, esp
004800f8  mov        eax, dword ptr [0x4b4318]
004800fd  sub        esp, 0xa0
00480103  cmp        byte ptr [eax], 0x3f
00480106  jne        0x48022b

// block 0048010c
0048010c  cmp        byte ptr [eax + 1], 0x24
00480110  jne        0x48022b

// block 00480116
00480116  or         dword ptr [ebp - 0x74], 0xffffffff
0048011a  or         dword ptr [ebp - 0x48], 0xffffffff
0048011e  or         dword ptr [ebp - 0xa0], 0xffffffff
00480125  push       ebx
00480126  mov        ebx, dword ptr [0x4b4314]
0048012c  push       esi
0048012d  mov        esi, dword ptr [0x4b430c]
00480133  lea        ecx, [ebp - 0x74]
00480136  mov        dword ptr [0x4b430c], ecx
0048013c  lea        ecx, [ebp - 0x48]
0048013f  push       edi
00480140  mov        edi, dword ptr [0x4b4310]
00480146  inc        eax
00480147  mov        dword ptr [0x4b4310], ecx
0048014d  inc        eax
0048014e  lea        ecx, [ebp - 0xa0]
00480154  mov        dword ptr [0x4b4318], eax
00480159  mov        dword ptr [0x4b4314], ecx
0048015f  cmp        byte ptr [eax], 0x3f
00480162  mov        byte ptr [ebp - 1], 0
00480166  jne        0x48017f

// block 00480168
00480168  inc        eax
00480169  mov        dword ptr [0x4b4318], eax
0048016e  lea        eax, [ebp - 1]
00480171  push       eax
00480172  lea        eax, [ebp - 0x14]
00480175  push       1
00480177  push       eax
00480178  call       0x47fb56

// block 0048017d
0048017d  jmp        0x48018c

// block 0048017f
0048017f  push       1
00480181  lea        eax, [ebp - 0x14]
00480184  push       1
00480186  push       eax
00480187  call       0x48023a

// block 0048018c
0048018c  mov        ecx, dword ptr [eax]
0048018e  mov        eax, dword ptr [eax + 4]
00480191  add        esp, 0xc
00480194  mov        dword ptr [ebp - 8], eax
00480197  mov        dword ptr [ebp - 0xc], ecx
0048019a  test       ecx, ecx
0048019c  jne        0x4801a5

// block 0048019e
0048019e  mov        byte ptr [0x4b4330], 1

// block 004801a5
004801a5  cmp        byte ptr [ebp - 1], 0
004801a9  jne        0x480206

// block 004801ab
004801ab  lea        eax, [ebp - 0x14]
004801ae  push       eax
004801af  call       0x47f992

// block 004801b4
004801b4  push       eax
004801b5  lea        eax, [ebp - 0x1c]
004801b8  push       0x3c
004801ba  push       eax
004801bb  call       0x47ec02

// block 004801c0
004801c0  add        esp, 0x10
004801c3  push       eax
004801c4  lea        ecx, [ebp - 0xc]
004801c7  call       0x47e7bf

// block 004801cc
004801cc  mov        ecx, dword ptr [ebp - 0xc]
004801cf  test       ecx, ecx
004801d1  je         0x4801e6

// block 004801d3
004801d3  mov        eax, dword ptr [ecx]
004801d5  call       dword ptr [eax + 4]

// block 004801d8
004801d8  cmp        al, 0x3e
004801da  jne        0x4801e6

// block 004801dc
004801dc  push       0x20
004801de  lea        ecx, [ebp - 0xc]
004801e1  call       0x47e9f6

// block 004801e6
004801e6  push       0x3e
004801e8  lea        ecx, [ebp - 0xc]
004801eb  call       0x47e9f6

// block 004801f0
004801f0  cmp        byte ptr [ebp + 0xc], 0
004801f4  je         0x480206

// block 004801f6
004801f6  mov        eax, dword ptr [0x4b4318]
004801fb  cmp        byte ptr [eax], 0
004801fe  je         0x480206

// block 00480200
00480200  inc        dword ptr [0x4b4318]

// block 00480206
00480206  mov        eax, dword ptr [ebp + 8]
00480209  mov        ecx, dword ptr [ebp - 0xc]
0048020c  mov        dword ptr [0x4b4310], edi
00480212  mov        dword ptr [0x4b430c], esi
00480218  mov        dword ptr [0x4b4314], ebx
0048021e  pop        edi
0048021f  mov        dword ptr [eax], ecx
00480221  mov        ecx, dword ptr [ebp - 8]
00480224  pop        esi
00480225  mov        dword ptr [eax + 4], ecx
00480228  pop        ebx
00480229  leave      
0048022a  ret        

// block 0048022b
0048022b  mov        ecx, dword ptr [ebp + 8]
0048022e  push       2
00480230  call       0x47e1ce

// block 00480235
00480235  mov        eax, dword ptr [ebp + 8]
00480238  leave      
00480239  ret        
