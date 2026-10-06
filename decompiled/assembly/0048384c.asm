
// block 0048384c
0048384c  mov        edi, edi
0048384e  push       ebp
0048384f  mov        ebp, esp
00483851  sub        esp, 0x10
00483854  push       dword ptr [ebp + 8]
00483857  lea        ecx, [ebp - 0x10]
0048385a  call       0x46d3b4

// block 0048385f
0048385f  push       dword ptr [ebp + 0x24]
00483862  lea        ecx, [ebp - 0x10]
00483865  push       dword ptr [ebp + 0x20]
00483868  push       dword ptr [ebp + 0x1c]
0048386b  push       dword ptr [ebp + 0x18]
0048386e  push       dword ptr [ebp + 0x14]
00483871  push       dword ptr [ebp + 0x10]
00483874  push       dword ptr [ebp + 0xc]
00483877  call       0x483692

// block 0048387c
0048387c  add        esp, 0x1c
0048387f  cmp        byte ptr [ebp - 4], 0
00483883  je         0x48388c

// block 00483885
00483885  mov        ecx, dword ptr [ebp - 8]
00483888  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048388c
0048388c  leave      
0048388d  ret        
