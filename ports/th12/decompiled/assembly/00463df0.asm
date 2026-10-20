
// block 00463df0
00463df0  test       dword ptr [0x4cee78], 0x8000
00463dfa  je         0x463e0d

// block 00463dfc
00463dfc  push       0x4cf128
00463e01  call       dword ptr [0x498088]

// block 00463e07
00463e07  inc        byte ptr [0x4cf21a]

// block 00463e0d
00463e0d  mov        eax, dword ptr [esp + 4]
00463e11  push       0
00463e13  push       0x8000080
00463e18  push       3
00463e1a  push       0
00463e1c  push       1
00463e1e  push       0x80000000
00463e23  push       eax
00463e24  call       dword ptr [0x49809c]

// block 00463e2a
00463e2a  cmp        eax, -1
00463e2d  je         0x463e5b

// block 00463e2f
00463e2f  push       eax
00463e30  call       dword ptr [0x4980a4]

// block 00463e36
00463e36  test       dword ptr [0x4cee78], 0x8000
00463e40  je         0x463e53

// block 00463e42
00463e42  push       0x4cf128
00463e47  call       dword ptr [0x49808c]

// block 00463e4d
00463e4d  dec        byte ptr [0x4cf21a]

// block 00463e53
00463e53  mov        eax, 1
00463e58  ret        4

// block 00463e5b
00463e5b  test       dword ptr [0x4cee78], 0x8000
00463e65  je         0x463e78

// block 00463e67
00463e67  push       0x4cf128
00463e6c  call       dword ptr [0x49808c]

// block 00463e72
00463e72  dec        byte ptr [0x4cf21a]

// block 00463e78
00463e78  xor        eax, eax
00463e7a  ret        4
