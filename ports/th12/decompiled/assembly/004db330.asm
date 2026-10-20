
// block 004db330
004db330  push       0x40
004db332  stosd      dword ptr es:[edi], eax
004db333  aaa        
004db334  cmpsd      dword ptr [esi], dword ptr es:[edi]
004db335  lodsd      eax, dword ptr [esi]
004db336  daa        
004db337  xchg       dword ptr [esi + 0x5153a7d0], ebp
004db33d  jo         0x4db2e7

// block 004db33f
004db33f  and        dword ptr [esi + 0x31], ebp
004db342  inc        ecx
