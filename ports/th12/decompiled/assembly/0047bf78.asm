
// block 0047bf78
0047bf78  mov        edi, edi
0047bf7a  push       ebp
0047bf7b  mov        ebp, esp
0047bf7d  inc        dword ptr [0x4b42f0]
0047bf83  push       0x1000
0047bf88  call       0x4777ce

// block 0047bf8d
0047bf8d  pop        ecx
0047bf8e  mov        ecx, dword ptr [ebp + 8]
0047bf91  mov        dword ptr [ecx + 8], eax
0047bf94  test       eax, eax
0047bf96  je         0x47bfa5

// block 0047bf98
0047bf98  or         dword ptr [ecx + 0xc], 8
0047bf9c  mov        dword ptr [ecx + 0x18], 0x1000
0047bfa3  jmp        0x47bfb6

// block 0047bfa5
0047bfa5  or         dword ptr [ecx + 0xc], 4
0047bfa9  lea        eax, [ecx + 0x14]
0047bfac  mov        dword ptr [ecx + 8], eax
0047bfaf  mov        dword ptr [ecx + 0x18], 2

// block 0047bfb6
0047bfb6  mov        eax, dword ptr [ecx + 8]
0047bfb9  and        dword ptr [ecx + 4], 0
0047bfbd  mov        dword ptr [ecx], eax
0047bfbf  pop        ebp
0047bfc0  ret        
