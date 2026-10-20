
// block 0040c480
0040c480  fld        dword ptr [0x4a3da8]
0040c486  sub        esp, 0x38
0040c489  push       esi
0040c48a  push       ecx
0040c48b  lea        esi, [edi + 0x970]
0040c491  fstp       dword ptr [esp]
0040c494  call       0x464a20

// block 0040c499
0040c499  cmp        dword ptr [edi + 0x998], 0
0040c4a0  je         0x40c880

// block 0040c4a6
0040c4a6  mov        eax, dword ptr [edi + 0x3fc]
0040c4ac  fld        dword ptr [eax + 0x34]
0040c4af  sub        esp, 8
0040c4b2  fstp       dword ptr [esp + 4]
0040c4b6  lea        esi, [edi + 0x4bc]
0040c4bc  fld        dword ptr [eax + 0x38]
0040c4bf  mov        ecx, esi
0040c4c1  fstp       dword ptr [esp]
0040c4c4  call       0x409970

// block 0040c4c9
0040c4c9  test       eax, eax
0040c4cb  je         0x40c880

// block 0040c4d1
0040c4d1  fld1       
0040c4d3  sub        esp, 8
0040c4d6  fstp       dword ptr [esp + 4]
0040c4da  lea        ecx, [esp + 0x10]
0040c4de  fld        dword ptr [edi + 0x4d8]
0040c4e4  fstp       dword ptr [esp]
0040c4e7  call       0x40d4d0

// block 0040c4ec
0040c4ec  mov        eax, dword ptr [edi + 0x3fc]
0040c4f2  fld        dword ptr [eax + 0x38]
0040c4f5  fsubr      qword ptr [0x4a3d90]
0040c4fb  fld        qword ptr [0x4a3cf8]
0040c501  fmul       st(1), st(0)
0040c503  fld        dword ptr [esi]
0040c505  fsubp      st(2)
0040c507  fxch       st(1)
0040c509  fstp       dword ptr [esp + 0x10]
0040c50d  fld        dword ptr [eax + 0x34]
0040c510  lea        eax, [esp + 0x10]
0040c514  fsubr      qword ptr [0x4a4160]
0040c51a  push       eax
0040c51b  mov        ecx, eax
0040c51d  push       ecx
0040c51e  fmulp      st(1)
0040c520  fadd       qword ptr [0x4a40b0]
0040c526  fsub       dword ptr [edi + 0x4c0]
0040c52c  fstp       dword ptr [esp + 0x1c]
0040c530  call       0x46c6ce

// block 0040c535
0040c535  fld        dword ptr [esp + 8]
0040c539  mov        eax, dword ptr [edi + 0x3fc]
0040c53f  fld        st(0)
0040c541  lea        edx, [esp + 0x10]
0040c545  fld        dword ptr [esp + 0x14]
0040c549  push       edx
0040c54a  fld        st(0)
0040c54c  fmulp      st(2)
0040c54e  fld        dword ptr [esp + 0x10]
0040c552  fld        st(0)
0040c554  fld        dword ptr [esp + 0x14]
0040c558  fld        st(0)
0040c55a  fmulp      st(2)
0040c55c  fxch       st(4)
0040c55e  fsubrp     st(1)
0040c560  fstp       dword ptr [esp + 0x1c]
0040c564  fmulp      st(1)
0040c566  fxch       st(2)
0040c568  fmulp      st(1)
0040c56a  faddp      st(1)
0040c56c  fstp       dword ptr [esp + 0x2c]
0040c570  fld        dword ptr [eax + 0x38]
0040c573  fadd       qword ptr [0x4a3d80]
0040c579  fld        qword ptr [0x4a3cf8]
0040c57f  fmul       st(1), st(0)
0040c581  fld        dword ptr [esi]
0040c583  fsubp      st(2)
0040c585  fxch       st(1)
0040c587  fstp       dword ptr [esp + 0x14]
0040c58b  fld        dword ptr [eax + 0x34]
0040c58e  mov        eax, edx
0040c590  fsubr      qword ptr [0x4a4160]
0040c596  push       eax
0040c597  fmulp      st(1)
0040c599  fadd       qword ptr [0x4a40b0]
0040c59f  fsub       dword ptr [edi + 0x4c0]
0040c5a5  fstp       dword ptr [esp + 0x1c]
0040c5a9  call       0x46c6ce

// block 0040c5ae
0040c5ae  fld        dword ptr [esp + 8]
0040c5b2  fld        st(0)
0040c5b4  fld        dword ptr [esp + 0x14]
0040c5b8  fld        st(0)
0040c5ba  fmulp      st(2)
0040c5bc  fld        dword ptr [esp + 0xc]
0040c5c0  fld        st(0)
0040c5c2  fld        dword ptr [esp + 0x10]
0040c5c6  fld        st(0)
0040c5c8  fmulp      st(2)
0040c5ca  fxch       st(4)
0040c5cc  fsubrp     st(1)
0040c5ce  fstp       dword ptr [esp + 0x1c]
0040c5d2  fmulp      st(1)
0040c5d4  fxch       st(2)
0040c5d6  fmulp      st(1)
0040c5d8  mov        eax, dword ptr [edi + 0x3fc]
0040c5de  faddp      st(1)
0040c5e0  lea        ecx, [esp + 0x10]
0040c5e4  push       ecx
0040c5e5  fstp       dword ptr [esp + 0x30]
0040c5e9  mov        edx, ecx
0040c5eb  fld        dword ptr [eax + 0x38]
0040c5ee  push       edx
0040c5ef  fsubr      qword ptr [0x4a3d90]
0040c5f5  fld        qword ptr [0x4a3cf8]
0040c5fb  fmul       st(1), st(0)
0040c5fd  fld        dword ptr [esi]
0040c5ff  fsubp      st(2)
0040c601  fxch       st(1)
0040c603  fstp       dword ptr [esp + 0x18]
0040c607  fld        dword ptr [eax + 0x34]
0040c60a  fadd       qword ptr [0x4a3d50]
0040c610  fmulp      st(1)
0040c612  fadd       qword ptr [0x4a40b0]
0040c618  fsub       dword ptr [edi + 0x4c0]
0040c61e  fstp       dword ptr [esp + 0x1c]
0040c622  call       0x46c6ce

// block 0040c627
0040c627  fld        dword ptr [esp + 8]
0040c62b  mov        eax, dword ptr [edi + 0x3fc]
0040c631  fld        st(0)
0040c633  fld        dword ptr [esp + 0x14]
0040c637  fld        st(0)
0040c639  fmulp      st(2)
0040c63b  fld        dword ptr [esp + 0xc]
0040c63f  fld        st(0)
0040c641  fld        dword ptr [esp + 0x10]
0040c645  fld        st(0)
0040c647  fmulp      st(2)
0040c649  fxch       st(4)
0040c64b  fsubrp     st(1)
0040c64d  fstp       dword ptr [esp + 0x20]
0040c651  fmulp      st(1)
0040c653  fxch       st(2)
0040c655  fmulp      st(1)
0040c657  faddp      st(1)
0040c659  fstp       dword ptr [esp + 0x30]
0040c65d  fld        dword ptr [eax + 0x38]
0040c660  fadd       qword ptr [0x4a3d80]
0040c666  fld        qword ptr [0x4a3cf8]
0040c66c  fmul       st(1), st(0)
0040c66e  fld        dword ptr [esi]
0040c670  fsubp      st(2)
0040c672  fxch       st(1)
0040c674  fstp       dword ptr [esp + 0x10]
0040c678  fld        dword ptr [eax + 0x34]
0040c67b  lea        eax, [esp + 0x10]
0040c67f  fadd       qword ptr [0x4a3d50]
0040c685  push       eax
0040c686  mov        ecx, eax
0040c688  push       ecx
0040c689  fmulp      st(1)
0040c68b  fadd       qword ptr [0x4a40b0]
0040c691  fsub       dword ptr [edi + 0x4c0]
0040c697  fstp       dword ptr [esp + 0x1c]
0040c69b  call       0x46c6ce

// block 0040c6a0
0040c6a0  fld        dword ptr [esp + 8]
0040c6a4  fld        st(0)
0040c6a6  fld        dword ptr [esp + 0x14]
0040c6aa  fld        st(0)
0040c6ac  fmulp      st(2)
0040c6ae  fld        dword ptr [esp + 0xc]
0040c6b2  fld        st(0)
0040c6b4  fld        dword ptr [esp + 0x10]
0040c6b8  fld        st(0)
0040c6ba  fmulp      st(2)
0040c6bc  fxch       st(4)
0040c6be  fsubrp     st(1)
0040c6c0  fstp       dword ptr [esp + 0x24]
0040c6c4  fmulp      st(1)
0040c6c6  fxch       st(2)
0040c6c8  fmulp      st(1)
0040c6ca  faddp      st(1)
0040c6cc  fstp       dword ptr [esp + 0x34]
0040c6d0  fld        dword ptr [0x4a4158]
0040c6d6  fst        dword ptr [esp + 8]
0040c6da  fldz       
0040c6dc  fld        dword ptr [esp + 0x18]
0040c6e0  fcom       st(1)
0040c6e2  fnstsw     ax
0040c6e4  fld        dword ptr [esp + 0x28]
0040c6e8  test       ah, 0x41
0040c6eb  jp         0x40c707

// block 0040c6ed
0040c6ed  fcom       qword ptr [0x4a4150]
0040c6f3  fnstsw     ax
0040c6f5  test       ah, 0x41
0040c6f8  jne        0x40c707

// block 0040c6fa
0040c6fa  fcom       st(2)
0040c6fc  fnstsw     ax
0040c6fe  test       ah, 1
0040c701  jne        0x40c707

// block 0040c703
0040c703  fst        dword ptr [esp + 8]

// block 0040c707
0040c707  fxch       st(2)
0040c709  fcom       dword ptr [esp + 0x1c]
0040c70d  fnstsw     ax
0040c70f  fld        dword ptr [esp + 0x2c]
0040c713  test       ah, 1
0040c716  jne        0x40c732

// block 0040c718
0040c718  fld        dword ptr [esp + 8]
0040c71c  fcomp      st(1)
0040c71e  fnstsw     ax
0040c720  test       ah, 5
0040c723  jp         0x40c732

// block 0040c725
0040c725  fcom       st(1)
0040c727  fnstsw     ax
0040c729  test       ah, 1
0040c72c  jne        0x40c732

// block 0040c72e
0040c72e  fst        dword ptr [esp + 8]

// block 0040c732
0040c732  fxch       st(1)
0040c734  fcom       dword ptr [esp + 0x20]
0040c738  fnstsw     ax
0040c73a  fld        dword ptr [esp + 0x30]
0040c73e  test       ah, 1
0040c741  jne        0x40c75d

// block 0040c743
0040c743  fld        dword ptr [esp + 8]
0040c747  fcomp      st(1)
0040c749  fnstsw     ax
0040c74b  test       ah, 5
0040c74e  jp         0x40c75d

// block 0040c750
0040c750  fcom       st(1)
0040c752  fnstsw     ax
0040c754  test       ah, 1
0040c757  jne        0x40c75d

// block 0040c759
0040c759  fst        dword ptr [esp + 8]

// block 0040c75d
0040c75d  fxch       st(1)
0040c75f  fcom       dword ptr [esp + 0x24]
0040c763  fnstsw     ax
0040c765  fld        dword ptr [esp + 0x34]
0040c769  test       ah, 1
0040c76c  jne        0x40c79e

// block 0040c76e
0040c76e  fstp       st(5)
0040c770  fld        dword ptr [esp + 8]
0040c774  fcomp      st(5)
0040c776  fnstsw     ax
0040c778  test       ah, 5
0040c77b  jp         0x40c792

// block 0040c77d
0040c77d  fcom       st(4)
0040c77f  fnstsw     ax
0040c781  test       ah, 0x41
0040c784  jp         0x40c792

// block 0040c786
0040c786  fxch       st(4)
0040c788  fst        dword ptr [esp + 8]
0040c78c  fld        dword ptr [esp + 0x28]
0040c790  jmp        0x40c7a2

// block 0040c792
0040c792  fld        dword ptr [esp + 0x28]
0040c796  fxch       st(1)
0040c798  fxch       st(5)
0040c79a  fxch       st(1)
0040c79c  jmp        0x40c7a2

// block 0040c79e
0040c79e  fxch       st(1)
0040c7a0  fxch       st(5)

// block 0040c7a2
0040c7a2  fxch       st(6)
0040c7a4  fstp       dword ptr [esp + 0x10]
0040c7a8  fxch       st(3)
0040c7aa  fcomp      st(4)
0040c7ac  fnstsw     ax
0040c7ae  test       ah, 1
0040c7b1  jne        0x40c7d1

// block 0040c7b3
0040c7b3  fxch       st(4)
0040c7b5  fcom       qword ptr [0x4a4150]
0040c7bb  fnstsw     ax
0040c7bd  test       ah, 0x41
0040c7c0  jne        0x40c7d5

// block 0040c7c2
0040c7c2  fcom       st(3)
0040c7c4  fnstsw     ax
0040c7c6  test       ah, 1
0040c7c9  jne        0x40c7d5

// block 0040c7cb
0040c7cb  fstp       dword ptr [esp + 0x10]
0040c7cf  jmp        0x40c7d7

// block 0040c7d1
0040c7d1  fstp       st(4)
0040c7d3  jmp        0x40c7d7

// block 0040c7d5
0040c7d5  fstp       st(0)

// block 0040c7d7
0040c7d7  fxch       st(2)
0040c7d9  fcom       dword ptr [esp + 0x1c]
0040c7dd  fnstsw     ax
0040c7df  test       ah, 0x41
0040c7e2  jp         0x40c802

// block 0040c7e4
0040c7e4  fld        dword ptr [esp + 0x10]
0040c7e8  fcomp      st(3)
0040c7ea  fnstsw     ax
0040c7ec  test       ah, 5
0040c7ef  jp         0x40c802

// block 0040c7f1
0040c7f1  fcom       st(2)
0040c7f3  fnstsw     ax
0040c7f5  test       ah, 0x41
0040c7f8  jp         0x40c802

// block 0040c7fa
0040c7fa  fxch       st(2)
0040c7fc  fstp       dword ptr [esp + 0x10]
0040c800  jmp        0x40c804

// block 0040c802
0040c802  fstp       st(2)

// block 0040c804
0040c804  fxch       st(1)
0040c806  fcom       dword ptr [esp + 0x20]
0040c80a  fnstsw     ax
0040c80c  test       ah, 0x41
0040c80f  jp         0x40c82f

// block 0040c811
0040c811  fld        dword ptr [esp + 0x10]
0040c815  fcomp      st(3)
0040c817  fnstsw     ax
0040c819  test       ah, 5
0040c81c  jp         0x40c82f

// block 0040c81e
0040c81e  fcom       st(2)
0040c820  fnstsw     ax
0040c822  test       ah, 0x41
0040c825  jp         0x40c82f

// block 0040c827
0040c827  fxch       st(2)
0040c829  fstp       dword ptr [esp + 0x10]
0040c82d  jmp        0x40c831

// block 0040c82f
0040c82f  fstp       st(2)

// block 0040c831
0040c831  fxch       st(1)
0040c833  fcom       dword ptr [esp + 0x24]
0040c837  fnstsw     ax
0040c839  test       ah, 0x41
0040c83c  jp         0x40c85a

// block 0040c83e
0040c83e  fld        dword ptr [esp + 0x10]
0040c842  fcomp      st(2)
0040c844  fnstsw     ax
0040c846  test       ah, 5
0040c849  jp         0x40c85a

// block 0040c84b
0040c84b  fcomp      st(1)
0040c84d  fnstsw     ax
0040c84f  test       ah, 0x41
0040c852  jp         0x40c85c

// block 0040c854
0040c854  fstp       dword ptr [esp + 0x10]
0040c858  jmp        0x40c85e

// block 0040c85a
0040c85a  fstp       st(0)

// block 0040c85c
0040c85c  fstp       st(0)

// block 0040c85e
0040c85e  fld        dword ptr [esp + 8]
0040c862  fld        qword ptr [0x4a4148]
0040c868  fcom       st(1)
0040c86a  fnstsw     ax
0040c86c  fstp       st(1)
0040c86e  test       ah, 0x41
0040c871  je         0x40c890

// block 0040c873
0040c873  fld        dword ptr [esp + 0x10]
0040c877  fcompp     
0040c879  fnstsw     ax
0040c87b  test       ah, 5
0040c87e  jnp        0x40c892

// block 0040c880
0040c880  cmp        dword ptr [edi + 0x974], 0
0040c887  jle        0x40c892

// block 0040c889
0040c889  xor        eax, eax
0040c88b  pop        esi
0040c88c  add        esp, 0x38
0040c88f  ret        

// block 0040c890
0040c890  fstp       st(0)

// block 0040c892
0040c892  xor        dword ptr [edi + 0x528], 0x400
0040c89c  mov        eax, 1
0040c8a1  pop        esi
0040c8a2  add        esp, 0x38
0040c8a5  ret        
