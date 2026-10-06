
// block 0049245e
0049245e  push       8
00492460  push       0x4ab418
00492465  call       0x46fcec

// block 0049246a
0049246a  and        dword ptr [ebp - 4], 0
0049246e  mov        ecx, dword ptr [ebp + 0xc]
00492471  call       dword ptr [ebp + 8]

// block 00492474
00492474  jmp        0x492483

// block 00492483
00492483  mov        dword ptr [ebp - 4], 0xfffffffe
0049248a  call       0x46fd31

// block 0049248f
0049248f  ret        
