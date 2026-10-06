
// block 004dbacf
004dbacf  mov        bh, 0x86
004dbad1  jle        0x4dba94

// block 004dbad3
004dbad3  sti        

// block 004dbad4
004dbad4  jecxz      0x4dbab9

// block 004dbad6
004dbad6  jecxz      0x4dbab9

// block 004dbad8
004dbad8  mov        byte ptr [esi], dh
004dbada  sbb        byte ptr [eax + 0xa1dfe7e], dl
004dbae0  xor        al, 0x49
004dbae2  push       ss
004dbae3  test       al, 0x5c
004dbae5  adc        dword ptr [ecx + 5], esi
004dbae8  aad        0x15
004dbaea  push       0x2f6a85ac
004dbaef  sbb        byte ptr [esi + 0x712c8605], ch
004dbaf5  cmp        ah, byte ptr [esi + 0x2d]
004dbaf8  nop        
