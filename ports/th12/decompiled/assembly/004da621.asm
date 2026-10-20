
// block 004da621
004da621  push       ecx
004da622  push       eax
004da623  mov        dword ptr [0x188a0da6], eax
004da628  cmp        eax, 0x69fa1deb
004da62d  cmp        ebp, eax
004da62f  mov        esi, dword ptr [esi + 0x63]
004da632  std        
004da633  ja         0x4da656

// block 004da635
004da635  dec        ebx
004da637  leave      
004da638  push       ss
004da639  xchg       edx, eax
004da63a  iretd      

// block 004da656
004da656  test       eax, 0xeabd385a
004da65b  or         ecx, 0x32
004da65e  or         edx, esi
004da660  mov        bh, 0x7c
004da662  push       edx
004da663  jae        0x4da6c3

// block 004da665
004da665  dec        ecx

// block 004da6c3
004da6c3  xor        byte ptr [eax + 0x43], 0xf4
004da6c7  hlt        
