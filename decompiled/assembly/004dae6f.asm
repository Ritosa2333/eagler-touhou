
// block 004dae20
004dae20  scasd      eax, dword ptr es:[edi]
004dae21  xor        eax, 0xf53f1b7c
004dae26  ficom      word ptr [ebx - 0x3f]
004dae29  adc        dl, byte ptr [edi + edx - 1]
004dae2d  mov        edi, ss
004dae2f  scasd      eax, dword ptr es:[edi]
004dae30  cld        
004dae31  mov        ecx, 0xfb69875e
004dae36  xor        dword ptr [ecx], esp
004dae38  in         eax, dx
004dae39  fist       word ptr [ebp + 0xcb3202b]
004dae3f  pop        ss
004dae40  test       eax, 0xfd45c44a
004dae45  push       ss
004dae46  or         eax, 0xfd6ffb65
004dae4b  inc        ebx
004dae4c  push       ss
004dae4d  inc        ecx

// block 004dae62
004dae62  jge        0x4dae13

// block 004dae69
004dae69  std        
004dae6b  xchg       ebp, eax
004dae6c  pop        es
004dae6d  push       ss

// block 004dae6f
004dae6f  xor        eax, ecx
004dae71  loop       0x4dae62

// block 004dae73
004dae73  mov        esi, 0xbfd2ac2a
004dae78  jno        0x4dae69

// block 004dae7a
004dae7a  popal      
004dae7b  push       edx
004dae7c  inc        ebp
004dae7d  cdq        
004dae7e  sbb        bh, bl
004dae80  pop        esi
004dae81  adc        ah, byte ptr [ebp + 0x5f]
004dae84  neg        byte ptr [ecx + 0x16]
004dae87  dec        esi

// block 004dae88
004dae88  sti        

// block 004dae89
004dae89  das        
004dae8a  sub        byte ptr [ecx + edi*2 + 0x76], 0x9f
004dae8f  xchg       dword ptr [edx - 0x76], edi
004dae92  push       ecx
004dae93  jnp        0x4dae20
