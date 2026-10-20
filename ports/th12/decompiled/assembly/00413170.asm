
// block 00413170
00413170  push       esi
00413171  mov        esi, dword ptr [0x4b43dc]
00413177  test       esi, esi
00413179  je         0x41318b

// block 0041317b
0041317b  mov        eax, esi
0041317d  call       0x412f10

// block 00413182
00413182  push       esi
00413183  call       0x46ca4f

// block 00413188
00413188  add        esp, 4

// block 0041318b
0041318b  pop        esi
0041318c  ret        
