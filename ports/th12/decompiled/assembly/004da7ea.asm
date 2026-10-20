
// block 004da7b4
004da7b4  int3       

// block 004da7c9
004da7c9  cmp        byte ptr [edi - 0x237f6a5b], dl
004da7cf  fnstcw     word ptr [ebp - 0x4c]
004da7d2  jle        0x4da845

// block 004da7d9
004da7d9  inc        edi
004da7db  pop        edi

// block 004da7ea
004da7ea  mov        edx, 0x8f1bcefe
004da7ef  push       ebx
004da7f0  xchg       esi, eax
004da7f1  and        edi, dword ptr [edi - 0x54]
004da7f4  mov        dword ptr [0x4994803], eax
004da7f9  adc        dl, byte ptr [eax]
004da7fb  xchg       byte ptr [eax - 0x3b], bh
004da7fe  jnp        0x4da7c9

// block 004da800
004da800  jnp        0x4da7b4

// block 004da802
004da802  test       byte ptr [0x3761955], bl

// block 004da808
004da808  mov        dh, 0x73
004da80a  fist       dword ptr [ecx]
004da80c  aad        0x6c
004da80e  mov        esp, 0xcddd8832
004da813  cdq        
004da814  sal        byte ptr [edi], 9
004da817  jl         0x4da7d9

// block 004da819
004da819  jb         0x4da85d

// block 004da81b
004da81b  xor        ebp, dword ptr [ebp + 0x637ebe7c]

// block 004da821
004da821  aam        0xfa
004da823  leave      
004da824  xor        esi, dword ptr [eax]
004da826  and        eax, 0xf8f7e8ea
004da82b  or         dword ptr [ecx], esi
004da82d  lea        edx, [esi]
004da82f  jns        0x4da808

// block 004da85d
004da85d  or         al, 0xf5
004da85f  or         byte ptr [ecx + 0x5b97f053], ah
