
// block 004db756
004db756  aas        
004db757  inc        esi
004db758  xchg       ebp, eax
004db75a  push       esi
004db75b  sub        byte ptr [ecx], ah
004db75d  jae        0x4db759

// block 004db75f
004db75f  and        ebx, dword ptr [edx - 0x18d50d5a]
004db765  pushal     
004db767  jbe        0x4db74a

// block 004db769
004db769  sub        edx, dword ptr [ecx + 0x5f6d7270]
004db76f  pop        es
004db770  xor        byte ptr [0xe6cdaad9], bh
004db776  jmp        0xdc7708f7
