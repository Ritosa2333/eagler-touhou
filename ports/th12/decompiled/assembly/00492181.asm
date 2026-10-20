
// block 00492181
00492181  push       8
00492183  push       0x4ab3f8
00492188  call       0x46fcec

// block 0049218d
0049218d  mov        ecx, dword ptr [ebp + 8]
00492190  test       ecx, ecx
00492192  je         0x4921be

// block 00492194
00492194  cmp        dword ptr [ecx], 0xe06d7363
0049219a  jne        0x4921be

// block 0049219c
0049219c  mov        eax, dword ptr [ecx + 0x1c]
0049219f  test       eax, eax
004921a1  je         0x4921be

// block 004921a3
004921a3  mov        eax, dword ptr [eax + 4]
004921a6  test       eax, eax
004921a8  je         0x4921be

// block 004921aa
004921aa  and        dword ptr [ebp - 4], 0
004921ae  push       eax
004921af  push       dword ptr [ecx + 0x18]
004921b2  call       0x491a0c

// block 004921b7
004921b7  mov        dword ptr [ebp - 4], 0xfffffffe

// block 004921be
004921be  call       0x46fd31

// block 004921c3
004921c3  ret        
