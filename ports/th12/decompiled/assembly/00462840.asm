
// block 00462840
00462840  test       esi, esi
00462842  je         0x462889

// block 00462844
00462844  test       dword ptr [0x4cee78], 0x8000
0046284e  je         0x462861

// block 00462850
00462850  push       0x4cf0f8
00462855  call       dword ptr [0x498088]

// block 0046285b
0046285b  inc        byte ptr [0x4cf218]

// block 00462861
00462861  mov        edx, dword ptr [esp + 4]
00462865  mov        ecx, esi
00462867  call       0x462890

// block 0046286c
0046286c  test       dword ptr [0x4cee78], 0x8000
00462876  je         0x462889

// block 00462878
00462878  push       0x4cf0f8
0046287d  call       dword ptr [0x49808c]

// block 00462883
00462883  dec        byte ptr [0x4cf218]

// block 00462889
00462889  ret        4
