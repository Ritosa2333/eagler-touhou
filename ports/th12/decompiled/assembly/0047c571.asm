
// block 0047c571
0047c571  mov        edi, edi
0047c573  push       ebp
0047c574  mov        ebp, esp
0047c576  push       ecx
0047c577  or         dword ptr [ebp - 4], 0xffffffff
0047c57b  push       0
0047c57d  push       dword ptr [ebp + 0xc]
0047c580  call       0x484851

// block 0047c585
0047c585  push       eax
0047c586  push       dword ptr [ebp + 8]
0047c589  lea        eax, [ebp - 4]
0047c58c  push       eax
0047c58d  call       0x47c397

// block 0047c592
0047c592  add        esp, 0x14
0047c595  test       eax, eax
0047c597  jne        0x47c59e

// block 0047c599
0047c599  mov        eax, dword ptr [ebp - 4]
0047c59c  leave      
0047c59d  ret        

// block 0047c59e
0047c59e  or         eax, 0xffffffff
0047c5a1  leave      
0047c5a2  ret        
