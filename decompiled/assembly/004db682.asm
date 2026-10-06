
// block 004db682
004db682  cmp        cl, bh
004db684  inc        ebp
004db686  pop        ss
004db687  push       esi
004db689  dec        ebp
004db68a  sar        dword ptr [edi - 0x68], cl
004db68d  dec        ebp
004db68e  pushal     
004db68f  mov        ebp, 0xcb33aa42
004db694  adc        esi, dword ptr [ebp + 0x5a]
004db697  cmpsb      byte ptr [esi], byte ptr es:[edi]
004db698  inc        ecx
004db699  out        0xe2, eax
004db69b  rol        esp, 0xd9
004db69e  cmp        eax, 0xed0dd255
004db6a3  in         al, dx
004db6a4  call       0x6f2b4d6d

// block 004db6a9
004db6a9  popal      
004db6aa  stc        
004db6ab  loopne     0x4db715

// block 004db6ad
004db6ad  push       esi
004db6ae  hlt        

// block 004db6da
004db6da  cmp        al, 0x99
004db6dc  add        dh, byte ptr [edi]
004db6de  sbb        eax, 0xb5b51917
004db6e3  mov        edx, 0x30b9f0d
004db6e8  mov        al, 0x6d
004db6ea  mov        dl, 0xb4
004db6ec  mov        es, word ptr [esi - 0x6b]

// block 004db715
004db715  pop        ecx
004db716  loop       0x4db6da
