
// block 004931fc
004931fc  cmp        dword ptr [0x4d52dc], 0
00493203  je         0x493216

// block 00493205
00493205  sub        esp, 4
00493208  fnstcw     word ptr [esp]
0049320b  pop        eax
0049320c  and        ax, 0x7f
00493210  cmp        ax, 0x7f
00493214  je         0x4931e9
