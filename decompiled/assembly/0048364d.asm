
// block 0048364d
0048364d  mov        edi, edi
0048364f  push       ebp
00483650  mov        ebp, esp
00483652  sub        esp, 0x10
00483655  push       dword ptr [ebp + 8]
00483658  lea        ecx, [ebp - 0x10]
0048365b  call       0x46d3b4

// block 00483660
00483660  push       dword ptr [ebp + 0x28]
00483663  lea        ecx, [ebp - 0x10]
00483666  push       dword ptr [ebp + 0x24]
00483669  push       dword ptr [ebp + 0x20]
0048366c  push       dword ptr [ebp + 0x1c]
0048366f  push       dword ptr [ebp + 0x18]
00483672  push       dword ptr [ebp + 0x14]
00483675  push       dword ptr [ebp + 0x10]
00483678  push       dword ptr [ebp + 0xc]
0048367b  call       0x4832a8

// block 00483680
00483680  add        esp, 0x20
00483683  cmp        byte ptr [ebp - 4], 0
00483687  je         0x483690

// block 00483689
00483689  mov        ecx, dword ptr [ebp - 8]
0048368c  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 00483690
00483690  leave      
00483691  ret        
