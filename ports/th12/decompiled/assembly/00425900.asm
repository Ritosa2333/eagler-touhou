
// block 00425900
00425900  push       esi
00425901  mov        esi, ecx
00425903  mov        eax, dword ptr [esi + 0x92c]
00425909  test       eax, eax
0042590b  je         0x425916

// block 0042590d
0042590d  push       eax
0042590e  call       0x46c941

// block 00425913
00425913  add        esp, 4

// block 00425916
00425916  mov        dword ptr [esi + 0x92c], 0
00425920  mov        eax, dword ptr [esi + 0x478]
00425926  test       eax, eax
00425928  je         0x425933

// block 0042592a
0042592a  push       eax
0042592b  call       0x46c941

// block 00425930
00425930  add        esp, 4

// block 00425933
00425933  mov        dword ptr [esi + 0x478], 0
0042593d  pop        esi
0042593e  ret        
