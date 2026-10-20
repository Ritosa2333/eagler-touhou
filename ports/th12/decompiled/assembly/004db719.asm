
// block 004db719
004db719  cmpsb      byte ptr [esi], byte ptr es:[edi]
004db71a  push       ss
004db71b  inc        ebx
004db71c  dec        esi
004db71d  test       dword ptr [edi + ebx*8], 0xfab788bc
004db724  pop        ebx
004db725  dec        esp
004db726  movsd      dword ptr es:[edi], dword ptr [esi]
004db727  or         dword ptr [eax], edi
004db729  sub        ebp, dword ptr [ecx - 0x44]
004db72c  dec        eax
004db72d  push       edi
004db72e  scasd      eax, dword ptr es:[edi]
004db72f  dec        eax
004db730  jmp        0x4db78a

// block 004db78a
004db78a  pop        ss
004db78b  fidivr     word ptr [edx]
004db78d  jns        0x4db7f6

// block 004db78f
004db78f  cmp        esp, 0x5e
004db792  jbe        0x4db7ab

// block 004db794
004db794  cmpsb      byte ptr [esi], byte ptr es:[edi]
004db795  mov        ebp, 0xbdb00897
004db79a  adc        cl, cl
004db79c  movsb      byte ptr es:[edi], byte ptr [esi]
004db79d  loopne     0x4db75e

// block 004db79f
004db79f  popal      
004db7a0  pop        eax
004db7a1  sub        edx, ebp
004db7a3  push       ebx

// block 004db7ab
004db7ab  push       ecx
004db7ac  mov        edx, 0x44dcf1ae
004db7b1  mov        dh, 6

// block 004db7ed
004db7ed  out        dx, eax
004db7ee  or         eax, 0x872bac41
004db7f3  inc        ebp

// block 004db7f6
004db7f6  mov        edx, 0x47185ecb
004db7fb  leave      
004db7fc  push       ebp
004db7fd  test       al, 0x41
004db7ff  push       cs
004db800  cmp        eax, dword ptr [edi - 0x64]
004db803  cmp        byte ptr [0xd798a9b0], 0x4f
004db80a  cmp        cl, cl
004db80c  or         byte ptr [edi - 0x50515412], dl
004db812  cmpsb      byte ptr [esi], byte ptr es:[edi]
004db813  sub        eax, 0x231450af
004db818  pop        esp
004db819  mov        byte ptr [0x159ffe34], al
004db81e  sub        al, 0xe
004db820  sub        al, 0x6e
004db822  jecxz      0x4db83d

// block 004db824
004db824  xchg       ebx, eax
004db825  mov        dword ptr [edx + 0x5e22773f], edx
004db82b  inc        esp
004db82c  jnp        0x4db7ed

// block 004db82e
004db82e  or         bl, bl
004db830  adc        dl, dl
004db832  mov        al, 0x68
004db834  mov        dl, 0x3d
004db836  cdq        
004db837  stosd      dword ptr es:[edi], eax

// block 004db83d
004db83d  mov        ds, word ptr [ecx + 0x7c0d4ea1]
