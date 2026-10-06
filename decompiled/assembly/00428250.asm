
// block 00428250
00428250  push       ebx
00428251  mov        ebx, dword ptr [0x4b44f4]
00428257  test       ebx, ebx
00428259  je         0x428269

// block 0042825b
0042825b  call       0x4280d0

// block 00428260
00428260  push       ebx
00428261  call       0x46ca4f

// block 00428266
00428266  add        esp, 4

// block 00428269
00428269  pop        ebx
0042826a  ret        
