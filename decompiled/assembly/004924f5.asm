
// block 004924f5
004924f5  push       8
004924f7  push       0x4ab478
004924fc  call       0x46fcec

// block 00492501
00492501  and        dword ptr [ebp - 4], 0
00492505  push       dword ptr [ebp + 0x18]
00492508  push       dword ptr [ebp + 0x14]
0049250b  push       dword ptr [ebp + 0x10]
0049250e  push       dword ptr [ebp + 0xc]
00492511  call       dword ptr [ebp + 8]

// block 00492514
00492514  jmp        0x492523

// block 00492523
00492523  mov        dword ptr [ebp - 4], 0xfffffffe
0049252a  call       0x46fd31

// block 0049252f
0049252f  ret        
