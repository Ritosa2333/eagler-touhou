
// block 004db1e9
004db1e9  leave      
004db1ea  jnp        0x4db1b5

// block 004db1ec
004db1ec  in         al, dx
004db1ed  mov        eax, dword ptr [0x268510c7]
004db1f2  stosd      dword ptr es:[edi], eax
004db1f3  call       0x803e1bf7

// block 004db1f8
004db1f8  stosd      dword ptr es:[edi], eax
004db1f9  mov        edi, 0x9112e45f
004db1fe  mov        eax, dword ptr [0xf2fd0b3b]
004db203  adc        al, byte ptr [eax]
004db205  xchg       edi, eax
004db206  div        byte ptr [esi]
004db208  clc        
004db209  add        eax, ecx
004db20b  ret        0xebd1
