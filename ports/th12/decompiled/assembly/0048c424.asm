
// block 0048c424
0048c424  mov        edi, edi
0048c426  push       ebp
0048c427  mov        ebp, esp
0048c429  sub        esp, 0x10
0048c42c  push       dword ptr [ebp + 8]
0048c42f  lea        ecx, [ebp - 0x10]
0048c432  call       0x46d3b4

// block 0048c437
0048c437  push       dword ptr [ebp + 0x1c]
0048c43a  lea        eax, [ebp - 0x10]
0048c43d  push       dword ptr [ebp + 0x18]
0048c440  push       dword ptr [ebp + 0x14]
0048c443  push       dword ptr [ebp + 0x10]
0048c446  push       dword ptr [ebp + 0xc]
0048c449  push       eax
0048c44a  call       0x48c2e5

// block 0048c44f
0048c44f  add        esp, 0x18
0048c452  cmp        byte ptr [ebp - 4], 0
0048c456  je         0x48c45f

// block 0048c458
0048c458  mov        ecx, dword ptr [ebp - 8]
0048c45b  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048c45f
0048c45f  leave      
0048c460  ret        
