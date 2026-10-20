
// block 0048c2b1
0048c2b1  mov        edi, edi
0048c2b3  push       ebp
0048c2b4  mov        ebp, esp
0048c2b6  sub        esp, 0x10
0048c2b9  push       dword ptr [ebp + 8]
0048c2bc  lea        ecx, [ebp - 0x10]
0048c2bf  call       0x46d3b4

// block 0048c2c4
0048c2c4  push       dword ptr [ebp + 0x18]
0048c2c7  push       dword ptr [ebp + 0x14]
0048c2ca  push       dword ptr [ebp + 0x10]
0048c2cd  push       dword ptr [ebp + 0xc]
0048c2d0  call       dword ptr [0x4981a0]

// block 0048c2d6
0048c2d6  cmp        byte ptr [ebp - 4], 0
0048c2da  je         0x48c2e3

// block 0048c2dc
0048c2dc  mov        ecx, dword ptr [ebp - 8]
0048c2df  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048c2e3
0048c2e3  leave      
0048c2e4  ret        
