
// block 0045bac0
0045bac0  sub        esp, 0x64
0045bac3  push       ebx
0045bac4  mov        ebx, dword ptr [esp + 0x70]
0045bac8  push       ebp
0045bac9  mov        ebp, dword ptr [esp + 0x70]
0045bacd  push       esi
0045bace  push       edi
0045bacf  push       ebp
0045bad0  call       0x45b930

// block 0045bad5
0045bad5  test       byte ptr [ebx + 0x47e], 1
0045badc  mov        eax, dword ptr [0x4cee34]
0045bae1  fld        dword ptr [eax + 0xfc]
0045bae7  fsub       dword ptr [eax + 0x100]
0045baed  fstp       dword ptr [esp + 0x1c]
0045baf1  je         0x45bafb

// block 0045baf3
0045baf3  mov        ebx, dword ptr [ebx + 0x3c0]
0045baf9  jmp        0x45bb01

// block 0045bafb
0045bafb  mov        ebx, dword ptr [ebx + 0x3bc]

// block 0045bb01
0045bb01  mov        dword ptr [esp + 0x18], ebx
0045bb05  mov        esi, 0x4d47fb
0045bb0a  lea        edi, [esp + 0x30]
0045bb0e  add        ebp, 0x4b5650

// block 0045bb14
0045bb14  mov        eax, dword ptr [esp + 0x78]
0045bb18  add        eax, 0x4b5140
0045bb1d  push       eax
0045bb1e  push       ebp
0045bb1f  push       edi
0045bb20  call       0x46c70a

// block 0045bb25
0045bb25  fld        dword ptr [edi]
0045bb27  fsub       dword ptr [0x4ceaec]
0045bb2d  push       ecx
0045bb2e  fstp       dword ptr [esp + 0x28]
0045bb32  fld        dword ptr [edi + 4]
0045bb35  fsub       dword ptr [0x4ceaf0]
0045bb3b  fstp       dword ptr [esp + 0x2c]
0045bb3f  fld        dword ptr [edi + 8]
0045bb42  fsub       dword ptr [0x4ceaf4]
0045bb48  fstp       dword ptr [esp + 0x30]
0045bb4c  fld        dword ptr [esp + 0x2c]
0045bb50  fld        dword ptr [esp + 0x28]
0045bb54  fld        dword ptr [esp + 0x30]
0045bb58  fld        st(1)
0045bb5a  fmulp      st(2)
0045bb5c  fld        st(2)
0045bb5e  fmulp      st(3)
0045bb60  fxch       st(1)
0045bb62  faddp      st(2)
0045bb64  fmul       st(0), st(0)
0045bb66  faddp      st(1)
0045bb68  fstp       dword ptr [esp + 0x18]
0045bb6c  fld        dword ptr [esp + 0x18]
0045bb70  fstp       dword ptr [esp]
0045bb73  call       0x408860

// block 0045bb78
0045bb78  fstp       dword ptr [esp + 0x14]
0045bb7c  fld        dword ptr [0x4cebe8]
0045bb82  fld        dword ptr [esp + 0x14]
0045bb86  fcom       st(1)
0045bb88  fnstsw     ax
0045bb8a  test       ah, 0x41
0045bb8d  jne        0x45bc87

// block 0045bb93
0045bb93  fsubp      st(1)
0045bb95  fdiv       dword ptr [esp + 0x1c]
0045bb99  fstp       dword ptr [esp + 0x14]
0045bb9d  fld1       
0045bb9f  fld        dword ptr [esp + 0x14]
0045bba3  fcom       st(1)
0045bba5  fnstsw     ax
0045bba7  fstp       st(1)
0045bba9  test       ah, 1
0045bbac  jne        0x45bbc7

// block 0045bbae
0045bbae  mov        eax, dword ptr [0x4cee34]
0045bbb3  mov        ecx, dword ptr [eax + 0x114]
0045bbb9  mov        dl, byte ptr [esp + 0x1b]
0045bbbd  mov        dword ptr [esi - 3], ecx
0045bbc0  mov        byte ptr [esi], dl
0045bbc2  jmp        0x45bc8c

// block 0045bbc7
0045bbc7  movzx      eax, bl
0045bbca  mov        dword ptr [esp + 0x14], eax
0045bbce  mov        dl, bl
0045bbd0  fild       dword ptr [esp + 0x14]
0045bbd4  fnstcw     word ptr [esp + 0x14]
0045bbd8  movzx      eax, word ptr [esp + 0x14]
0045bbdd  fsub       dword ptr [0x4cebf0]
0045bbe3  or         eax, 0xc00
0045bbe8  mov        dword ptr [esp + 0x20], eax
0045bbec  movzx      eax, bh
0045bbef  fmul       st(1)
0045bbf1  fldcw      word ptr [esp + 0x20]
0045bbf5  fistp      dword ptr [esp + 0x20]
0045bbf9  movzx      ecx, byte ptr [esp + 0x20]
0045bbfe  mov        dword ptr [esp + 0x20], eax
0045bc02  sub        dl, cl
0045bc04  fldcw      word ptr [esp + 0x14]
0045bc08  mov        byte ptr [esi - 3], dl
0045bc0b  mov        dl, bh
0045bc0d  fild       dword ptr [esp + 0x20]
0045bc11  fnstcw     word ptr [esp + 0x14]
0045bc15  fsub       dword ptr [0x4cebf4]
0045bc1b  movzx      eax, word ptr [esp + 0x14]
0045bc20  or         eax, 0xc00
0045bc25  mov        dword ptr [esp + 0x20], eax
0045bc29  fmul       st(1)
0045bc2b  fldcw      word ptr [esp + 0x20]
0045bc2f  fistp      dword ptr [esp + 0x20]
0045bc33  movzx      ecx, byte ptr [esp + 0x20]
0045bc38  sub        dl, cl
0045bc3a  mov        cl, byte ptr [esp + 0x1a]
0045bc3e  fldcw      word ptr [esp + 0x14]
0045bc42  movzx      eax, cl
0045bc45  mov        dword ptr [esp + 0x20], eax
0045bc49  mov        byte ptr [esi - 2], dl
0045bc4c  fild       dword ptr [esp + 0x20]
0045bc50  fnstcw     word ptr [esp + 0x14]
0045bc54  fsub       dword ptr [0x4cebf8]
0045bc5a  movzx      eax, word ptr [esp + 0x14]
0045bc5f  or         eax, 0xc00
0045bc64  mov        dword ptr [esp + 0x20], eax
0045bc68  mov        al, byte ptr [esp + 0x1b]
0045bc6c  fmulp      st(1)
0045bc6e  mov        byte ptr [esi], al
0045bc70  fldcw      word ptr [esp + 0x20]
0045bc74  fistp      dword ptr [esp + 0x20]
0045bc78  mov        dl, byte ptr [esp + 0x20]
0045bc7c  sub        cl, dl
0045bc7e  mov        byte ptr [esi - 1], cl
0045bc81  fldcw      word ptr [esp + 0x14]
0045bc85  jmp        0x45bc8e

// block 0045bc87
0045bc87  fstp       st(1)
0045bc89  mov        dword ptr [esi - 3], ebx

// block 0045bc8c
0045bc8c  fstp       st(0)

// block 0045bc8e
0045bc8e  add        esi, 0x1c
0045bc91  add        ebp, 0x14
0045bc94  add        edi, 0x10
0045bc97  cmp        esi, 0x4d486b
0045bc9d  jl         0x45bb14

// block 0045bca3
0045bca3  mov        eax, dword ptr [esp + 0x7c]
0045bca7  mov        ecx, dword ptr [esp + 0x78]
0045bcab  push       2
0045bcad  call       0x459e50

// block 0045bcb2
0045bcb2  fld1       
0045bcb4  pop        edi
0045bcb5  fst        dword ptr [0x4d4848]
0045bcbb  pop        esi
0045bcbc  fst        dword ptr [0x4d482c]
0045bcc2  pop        ebp
0045bcc3  fst        dword ptr [0x4d4810]
0045bcc9  fstp       dword ptr [0x4d47f4]
0045bccf  pop        ebx
0045bcd0  add        esp, 0x64
0045bcd3  ret        8
