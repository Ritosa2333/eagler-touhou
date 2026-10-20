
// block 0044f130
0044f130  or         eax, 0xffffffff
0044f133  xor        edx, edx
0044f135  div        ecx
0044f137  sub        esp, 0x10
0044f13a  cmp        eax, 1
0044f13d  jae        0x44f16b

// block 0044f13f
0044f13f  lea        eax, [esp]
0044f142  push       eax
0044f143  lea        ecx, [esp + 8]
0044f147  mov        dword ptr [esp + 4], 0
0044f14f  call       0x46df5e

// block 0044f154
0044f154  push       0x4ab5d0
0044f159  lea        ecx, [esp + 8]
0044f15d  push       ecx
0044f15e  mov        dword ptr [esp + 0xc], 0x49cd00
0044f166  call       0x46ff8f

// block 0044f16b
0044f16b  push       ecx
0044f16c  call       0x46c9ea

// block 0044f171
0044f171  add        esp, 4
0044f174  add        esp, 0x10
0044f177  ret        
