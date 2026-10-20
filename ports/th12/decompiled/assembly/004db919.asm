
// block 004db906
004db906  mov        bl, 0xdb
004db908  jns        0x4db953

// block 004db90a
004db90a  sbb        cl, ch
004db90c  push       ds
004db90d  push       ds
004db90e  test       eax, 0xd28678ff
004db913  and        esp, edi
004db915  pop        ebx
004db916  mov        ch, 0x9f

// block 004db919
004db919  js         0x4db906

// block 004db953
004db953  cld        
004db954  mov        bh, 0x10
004db956  dec        edx
004db957  mov        edx, 0xd2cc2928
004db95c  scasd      eax, dword ptr es:[edi]
004db95d  jg         0x4db8e1

// block 004db95f
004db95f  inc        edi
004db960  wait       
004db961  fcomi      st(7)
004db963  cmp        ah, bh
004db965  cmc        
004db966  or         eax, 0x41f215bd
004db96b  lodsb      al, byte ptr [esi]
004db96c  xchg       edi, eax
004db96d  sub        eax, 0x613b66fe
004db972  aaa        
004db973  mov        bh, 0xa9
004db975  loope      0x4db9f0

// block 004db977
004db977  and        cl, byte ptr [esi - 0x63]
004db97a  int        0x10

// block 004db97c
004db97c  jbe        0x4db9f5

// block 004db97e
004db97e  ret        0x9086
