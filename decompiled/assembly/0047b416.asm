
// block 0047b416
0047b416  push       0x10
0047b418  push       0x4aae58
0047b41d  call       0x46fcec

// block 0047b422
0047b422  and        dword ptr [ebp - 4], 0
0047b426  push       dword ptr [ebp + 0xc]
0047b429  push       dword ptr [ebp + 8]
0047b42c  call       dword ptr [0x498074]

// block 0047b432
0047b432  mov        dword ptr [ebp - 0x1c], eax
0047b435  jmp        0x47b466

// block 0047b466
0047b466  mov        dword ptr [ebp - 4], 0xfffffffe
0047b46d  mov        eax, dword ptr [ebp - 0x1c]
0047b470  call       0x46fd31

// block 0047b475
0047b475  ret        
