
// block 0045b610
0045b610  sub        esp, 0x28
0045b613  push       ebx
0045b614  mov        eax, esi
0045b616  call       0x45b210

// block 0045b61b
0045b61b  test       eax, eax
0045b61d  je         0x45b627

// block 0045b61f
0045b61f  or         eax, 0xffffffff
0045b622  pop        ebx
0045b623  add        esp, 0x28
0045b626  ret        

// block 0045b627
0045b627  test       byte ptr [esi + 0x47e], 1
0045b62e  mov        eax, dword ptr [0x4cee34]
0045b633  fld        dword ptr [eax + 0xfc]
0045b639  fsub       dword ptr [eax + 0x100]
0045b63f  fstp       dword ptr [esp + 0x10]
0045b643  je         0x45b651

// block 0045b645
0045b645  mov        ecx, dword ptr [esi + 0x3c0]
0045b64b  mov        dword ptr [esp + 0xc], ecx
0045b64f  jmp        0x45b65b

// block 0045b651
0045b651  mov        edx, dword ptr [esi + 0x3bc]
0045b657  mov        dword ptr [esp + 0xc], edx

// block 0045b65b
0045b65b  fld        dword ptr [esi + 0x430]
0045b661  push       ecx
0045b662  fadd       dword ptr [esi + 0x424]
0045b668  fstp       dword ptr [esp + 0x18]
0045b66c  fld        dword ptr [esi + 0x434]
0045b672  fadd       dword ptr [esi + 0x428]
0045b678  fstp       dword ptr [esp + 0x1c]
0045b67c  fld        dword ptr [esi + 0x438]
0045b682  fadd       dword ptr [esi + 0x42c]
0045b688  fstp       dword ptr [esp + 0x20]
0045b68c  fld        dword ptr [esi + 0x43c]
0045b692  fadd       dword ptr [esp + 0x18]
0045b696  fstp       dword ptr [esp + 0x24]
0045b69a  fld        dword ptr [esi + 0x440]
0045b6a0  fadd       dword ptr [esp + 0x1c]
0045b6a4  fstp       dword ptr [esp + 0x28]
0045b6a8  fld        dword ptr [esi + 0x444]
0045b6ae  fadd       dword ptr [esp + 0x20]
0045b6b2  fstp       dword ptr [esp + 0x2c]
0045b6b6  fld        dword ptr [esp + 0x24]
0045b6ba  fsub       dword ptr [eax]
0045b6bc  fstp       dword ptr [esp + 0x18]
0045b6c0  fld        dword ptr [esp + 0x28]
0045b6c4  fsub       dword ptr [eax + 4]
0045b6c7  fstp       dword ptr [esp + 0x1c]
0045b6cb  fld        dword ptr [esp + 0x2c]
0045b6cf  fsub       dword ptr [eax + 8]
0045b6d2  fstp       dword ptr [esp + 0x20]
0045b6d6  fld        dword ptr [esp + 0x1c]
0045b6da  fld        dword ptr [esp + 0x18]
0045b6de  fld        dword ptr [esp + 0x20]
0045b6e2  fld        st(1)
0045b6e4  fmulp      st(2)
0045b6e6  fld        st(2)
0045b6e8  fmulp      st(3)
0045b6ea  fxch       st(1)
0045b6ec  faddp      st(2)
0045b6ee  fmul       st(0), st(0)
0045b6f0  faddp      st(1)
0045b6f2  fstp       dword ptr [esp + 0xc]
0045b6f6  fld        dword ptr [esp + 0xc]
0045b6fa  fstp       dword ptr [esp]
0045b6fd  call       0x408860

// block 0045b702
0045b702  cmp        dword ptr [edi + 0x88ed50], 0
0045b709  fstp       dword ptr [esp + 8]
0045b70d  je         0x45b79f

// block 0045b713
0045b713  movzx      eax, byte ptr [edi + 0x88ed4e]
0045b71a  movzx      ecx, byte ptr [esp + 0xe]
0045b71f  imul       eax, ecx
0045b722  shr        eax, 7
0045b725  cmp        eax, 0x100
0045b72a  jb         0x45b731

// block 0045b72c
0045b72c  mov        eax, 0xff

// block 0045b731
0045b731  movzx      edx, byte ptr [esp + 0xd]
0045b736  mov        byte ptr [esp + 0xe], al
0045b73a  movzx      eax, byte ptr [edi + 0x88ed4d]
0045b741  imul       eax, edx
0045b744  shr        eax, 7
0045b747  cmp        eax, 0x100
0045b74c  jb         0x45b753

// block 0045b74e
0045b74e  mov        eax, 0xff

// block 0045b753
0045b753  movzx      ecx, byte ptr [esp + 0xc]
0045b758  mov        byte ptr [esp + 0xd], al
0045b75c  movzx      eax, byte ptr [edi + 0x88ed4c]
0045b763  imul       eax, ecx
0045b766  shr        eax, 7
0045b769  cmp        eax, 0x100
0045b76e  jb         0x45b775

// block 0045b770
0045b770  mov        eax, 0xff

// block 0045b775
0045b775  movzx      edx, byte ptr [esp + 0xf]
0045b77a  mov        bl, al
0045b77c  movzx      eax, byte ptr [edi + 0x88ed4f]
0045b783  imul       eax, edx
0045b786  shr        eax, 7
0045b789  mov        byte ptr [esp + 0xc], bl
0045b78d  cmp        eax, 0x100
0045b792  jb         0x45b799

// block 0045b794
0045b794  mov        eax, 0xff

// block 0045b799
0045b799  mov        byte ptr [esp + 0xf], al
0045b79d  jmp        0x45b7a3

// block 0045b79f
0045b79f  mov        bl, byte ptr [esp + 0xc]

// block 0045b7a3
0045b7a3  fld        dword ptr [0x4cebe8]
0045b7a9  fld        dword ptr [esp + 8]
0045b7ad  fcom       st(1)
0045b7af  fnstsw     ax
0045b7b1  test       ah, 0x41
0045b7b4  jne        0x45b8fb

// block 0045b7ba
0045b7ba  fsubp      st(1)
0045b7bc  fdiv       dword ptr [esp + 0x10]
0045b7c0  fstp       dword ptr [esp + 8]
0045b7c4  fld1       
0045b7c6  fld        dword ptr [esp + 8]
0045b7ca  fcom       st(1)
0045b7cc  fnstsw     ax
0045b7ce  fstp       st(1)
0045b7d0  test       ah, 1
0045b7d3  jne        0x45b7e1

// block 0045b7d5
0045b7d5  fstp       st(0)
0045b7d7  mov        eax, 0xffffffff
0045b7dc  pop        ebx
0045b7dd  add        esp, 0x28
0045b7e0  ret        

// block 0045b7e1
0045b7e1  fld        dword ptr [0x4cebf0]
0045b7e7  call       0x4931e0

// block 0045b7ec
0045b7ec  fnstcw     word ptr [esp + 8]
0045b7f0  movzx      ecx, bl
0045b7f3  sub        ecx, eax
0045b7f5  movzx      eax, word ptr [esp + 8]
0045b7fa  mov        dword ptr [esp + 0x10], ecx
0045b7fe  or         eax, 0xc00
0045b803  fild       dword ptr [esp + 0x10]
0045b807  mov        dword ptr [esp + 0x10], eax
0045b80b  fmul       st(1)
0045b80d  fldcw      word ptr [esp + 0x10]
0045b811  fistp      dword ptr [esp + 0x10]
0045b815  movzx      edx, byte ptr [esp + 0x10]
0045b81a  sub        bl, dl
0045b81c  mov        byte ptr [0x4d47f8], bl
0045b822  fldcw      word ptr [esp + 8]
0045b826  fld        dword ptr [0x4cebf4]
0045b82c  call       0x4931e0

// block 0045b831
0045b831  fnstcw     word ptr [esp + 8]
0045b835  mov        cl, byte ptr [esp + 0xd]
0045b839  movzx      edx, cl
0045b83c  sub        edx, eax
0045b83e  movzx      eax, word ptr [esp + 8]
0045b843  mov        dword ptr [esp + 0x10], edx
0045b847  or         eax, 0xc00
0045b84c  fild       dword ptr [esp + 0x10]
0045b850  mov        dword ptr [esp + 0x10], eax
0045b854  fmul       st(1)
0045b856  fldcw      word ptr [esp + 0x10]
0045b85a  fistp      dword ptr [esp + 0x10]
0045b85e  movzx      eax, byte ptr [esp + 0x10]
0045b863  sub        cl, al
0045b865  mov        byte ptr [0x4d47f9], cl
0045b86b  fldcw      word ptr [esp + 8]
0045b86f  fld        dword ptr [0x4cebf8]
0045b875  call       0x4931e0

// block 0045b87a
0045b87a  fnstcw     word ptr [esp + 8]
0045b87e  mov        cl, byte ptr [esp + 0xe]
0045b882  movzx      edx, cl
0045b885  sub        edx, eax
0045b887  movzx      eax, word ptr [esp + 8]
0045b88c  mov        dword ptr [esp + 0x10], edx
0045b890  or         eax, 0xc00
0045b895  fild       dword ptr [esp + 0x10]
0045b899  mov        dword ptr [esp + 0x10], eax
0045b89d  fmul       st(1)
0045b89f  fldcw      word ptr [esp + 0x10]
0045b8a3  fistp      dword ptr [esp + 0x10]
0045b8a7  movzx      eax, byte ptr [esp + 0x10]
0045b8ac  sub        cl, al
0045b8ae  mov        byte ptr [0x4d47fa], cl
0045b8b4  fldcw      word ptr [esp + 8]
0045b8b8  movzx      ecx, byte ptr [esp + 0xf]
0045b8bd  mov        dword ptr [esp + 0x10], ecx
0045b8c1  fld1       
0045b8c3  fnstcw     word ptr [esp + 8]
0045b8c7  fsubrp     st(1)
0045b8c9  movzx      eax, word ptr [esp + 8]
0045b8ce  fild       dword ptr [esp + 0x10]
0045b8d2  or         eax, 0xc00
0045b8d7  mov        dword ptr [esp + 0x10], eax
0045b8db  fmulp      st(1)
0045b8dd  fldcw      word ptr [esp + 0x10]
0045b8e1  fistp      dword ptr [esp + 0x10]
0045b8e5  movzx      edx, byte ptr [esp + 0x10]
0045b8ea  mov        byte ptr [0x4d47fb], dl
0045b8f0  mov        eax, dword ptr [0x4d47f8]
0045b8f5  fldcw      word ptr [esp + 8]
0045b8f9  jmp        0x45b908

// block 0045b8fb
0045b8fb  mov        eax, dword ptr [esp + 0xc]
0045b8ff  fstp       st(1)
0045b901  fstp       st(0)
0045b903  mov        dword ptr [0x4d47f8], eax

// block 0045b908
0045b908  mov        dword ptr [0x4d4814], eax
0045b90d  mov        dword ptr [0x4d4830], eax
0045b912  mov        dword ptr [0x4d484c], eax
0045b917  push       2
0045b919  mov        eax, esi
0045b91b  mov        ecx, edi
0045b91d  call       0x459e50

// block 0045b922
0045b922  pop        ebx
0045b923  add        esp, 0x28
0045b926  ret        
