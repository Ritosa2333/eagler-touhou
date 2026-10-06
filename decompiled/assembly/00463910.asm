
// block 00463910
00463910  push       0x100
00463915  push       0
00463917  push       esi
00463918  call       0x477420

// block 0046391d
0046391d  add        esp, 0xc
00463920  cmp        dword ptr [0x4cf3fc], 0
00463927  je         0x463972

// block 00463929
00463929  test       dword ptr [0x4cee78], 0x400
00463933  push       esi
00463934  jne        0x463942

// block 00463936
00463936  call       dword ptr [0x498244]

// block 0046393c
0046393c  mov        eax, 2
00463941  ret        

// block 00463942
00463942  mov        eax, dword ptr [0x4ce908]
00463947  mov        ecx, dword ptr [eax]
00463949  mov        edx, dword ptr [ecx + 0x24]
0046394c  push       0x100
00463951  push       eax
00463952  call       edx

// block 00463954
00463954  cmp        eax, 0x8007001e
00463959  je         0x46395f

// block 0046395b
0046395b  test       eax, eax
0046395d  je         0x46396c

// block 0046395f
0046395f  mov        eax, dword ptr [0x4ce908]
00463964  mov        ecx, dword ptr [eax]
00463966  mov        edx, dword ptr [ecx + 0x1c]
00463969  push       eax
0046396a  call       edx

// block 0046396c
0046396c  mov        eax, 1
00463971  ret        

// block 00463972
00463972  xor        eax, eax
00463974  ret        
