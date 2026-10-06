
// block 004280d0
004280d0  push       ebp
004280d1  mov        ebp, dword ptr [0x498088]
004280d7  push       esi
004280d8  mov        esi, dword ptr [ebx + 8]
004280db  push       edi
004280dc  mov        edi, dword ptr [0x4ce89c]
004280e2  test       esi, esi
004280e4  je         0x428125

// block 004280e6
004280e6  test       dword ptr [0x4cee78], 0x8000
004280f0  je         0x4280ff

// block 004280f2
004280f2  push       0x4cf0f8
004280f7  call       ebp

// block 004280f9
004280f9  inc        byte ptr [0x4cf218]

// block 004280ff
004280ff  mov        ecx, esi
00428101  mov        edx, edi
00428103  call       0x462890

// block 00428108
00428108  test       dword ptr [0x4cee78], 0x8000
00428112  je         0x428125

// block 00428114
00428114  push       0x4cf0f8
00428119  call       dword ptr [0x49808c]

// block 0042811f
0042811f  dec        byte ptr [0x4cf218]

// block 00428125
00428125  mov        esi, dword ptr [ebx + 0xc]
00428128  mov        edi, dword ptr [0x4ce89c]
0042812e  test       esi, esi
00428130  je         0x428171

// block 00428132
00428132  test       dword ptr [0x4cee78], 0x8000
0042813c  je         0x42814b

// block 0042813e
0042813e  push       0x4cf0f8
00428143  call       ebp

// block 00428145
00428145  inc        byte ptr [0x4cf218]

// block 0042814b
0042814b  mov        ecx, esi
0042814d  mov        edx, edi
0042814f  call       0x462890

// block 00428154
00428154  test       dword ptr [0x4cee78], 0x8000
0042815e  je         0x428171

// block 00428160
00428160  push       0x4cf0f8
00428165  call       dword ptr [0x49808c]

// block 0042816b
0042816b  dec        byte ptr [0x4cf218]

// block 00428171
00428171  mov        esi, dword ptr [ebx + 0x18]
00428174  test       esi, esi
00428176  je         0x4281a9

// block 00428178
00428178  mov        eax, dword ptr [esi]
0042817a  mov        edx, dword ptr [eax + 0x10]
0042817d  mov        edi, dword ptr [esi + 8]
00428180  mov        ecx, esi
00428182  call       edx

// block 00428184
00428184  mov        eax, dword ptr [esi + 4]
00428187  mov        ecx, dword ptr [esi + 8]
0042818a  mov        dword ptr [eax + 8], ecx
0042818d  mov        eax, dword ptr [esi + 8]
00428190  test       eax, eax
00428192  je         0x42819a

// block 00428194
00428194  mov        edx, dword ptr [esi + 4]
00428197  mov        dword ptr [eax + 4], edx

// block 0042819a
0042819a  push       esi
0042819b  call       0x46ca4f

// block 004281a0
004281a0  add        esp, 4
004281a3  mov        esi, edi
004281a5  test       edi, edi
004281a7  jne        0x428178

// block 004281a9
004281a9  pop        edi
004281aa  pop        esi
004281ab  mov        dword ptr [0x4b44f4], 0
004281b5  pop        ebp
004281b6  ret        
