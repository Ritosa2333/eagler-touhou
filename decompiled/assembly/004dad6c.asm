
// block 004dad65
004dad65  mov        dh, 0xd7
004dad67  xlatb      
004dad68  cdq        
004dad69  dec        edx
004dad6a  inc        esi
004dad6b  int3       

// block 004dad6c
004dad6c  or         eax, esp
004dad6e  jae        0x4dad65

// block 004dad70
004dad70  xor        eax, 0xda2c0f84
004dad75  fadd       dword ptr [ecx + ebp*8 + 0x2b4fc912]
004dad7c  jo         0x4dad94

// block 004dad7e
004dad7e  nop        
004dad7f  or         esp, dword ptr [edx + 0x30]
004dad82  in         al, 3
004dad84  inc        ebp
004dad85  sbb        dword ptr [eax], ecx
004dad87  push       ebp
004dad88  fcomp      st(7)
004dad8a  salc       
004dad8b  test       dword ptr [eax - 0x2b574e60], edi
004dad91  or         dh, ah
004dad93  add        dword ptr [ecx], 0x620f4b31
004dad9a  inc        ebp
004dad9b  mov        ecx, 0xa07e2c26
004dada0  sub        cl, byte ptr [ecx + 0xb807c5a]
004dada6  xor        dword ptr [esp], 0x94d4b524
004dadad  push       eax
004dadae  lahf       
004dadaf  je         0x4dad39

// block 004dadb1
004dadb1  pop        edi
004dadb2  movsb      byte ptr es:[edi], byte ptr [esi]
004dadb3  xchg       esp, eax
004dadb4  cwde       
