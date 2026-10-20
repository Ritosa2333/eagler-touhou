
// block 0045b210
0045b210  sub        esp, 0x98
0045b216  push       esi
0045b217  mov        esi, eax
0045b219  fld        dword ptr [esi + 0x2c]
0045b21c  fstp       dword ptr [esp + 0x44]
0045b220  fld        dword ptr [esp + 0x44]
0045b224  fsincos    
0045b226  fstp       dword ptr [esp + 0x2c]
0045b22a  fstp       dword ptr [esp + 0x28]
0045b22e  fldz       
0045b230  lea        eax, [esp + 0x58]
0045b234  fst        dword ptr [esp + 4]
0045b238  push       eax
0045b239  fst        dword ptr [esp + 0xc]
0045b23d  mov        eax, dword ptr [0x4cee34]
0045b242  fst        dword ptr [esp + 0x10]
0045b246  lea        ecx, [eax + 0x4c]
0045b249  fst        dword ptr [esp + 0x88]
0045b250  push       ecx
0045b251  fst        dword ptr [esp + 0x84]
0045b258  lea        edx, [eax + 0x8c]
0045b25e  fst        dword ptr [esp + 0x80]
0045b265  push       edx
0045b266  fst        dword ptr [esp + 0x80]
0045b26d  add        eax, 0xcc
0045b272  fst        dword ptr [esp + 0x7c]
0045b276  push       eax
0045b277  fst        dword ptr [esp + 0x78]
0045b27b  lea        eax, [esp + 0x14]
0045b27f  fst        dword ptr [esp + 0x74]
0045b283  push       eax
0045b284  fst        dword ptr [esp + 0x74]
0045b288  lea        ecx, [esp + 0x48]
0045b28c  fstp       dword ptr [esp + 0x70]
0045b290  push       ecx
0045b291  fld1       
0045b293  fst        dword ptr [esp + 0xac]
0045b29a  fst        dword ptr [esp + 0x98]
0045b2a1  fst        dword ptr [esp + 0x84]
0045b2a8  fstp       dword ptr [esp + 0x70]
0045b2ac  fld        dword ptr [esi + 0x430]
0045b2b2  fadd       dword ptr [esi + 0x424]
0045b2b8  fadd       dword ptr [esi + 0x43c]
0045b2be  fstp       dword ptr [esp + 0xa0]
0045b2c5  fld        dword ptr [esi + 0x434]
0045b2cb  fadd       dword ptr [esi + 0x428]
0045b2d1  fadd       dword ptr [esi + 0x440]
0045b2d7  fstp       dword ptr [esp + 0xa4]
0045b2de  fld        dword ptr [esi + 0x438]
0045b2e4  fadd       dword ptr [esi + 0x42c]
0045b2ea  fadd       dword ptr [esi + 0x444]
0045b2f0  fstp       dword ptr [esp + 0xa8]
0045b2f7  call       0x46c6d4

// block 0045b2fc
0045b2fc  fldz       
0045b2fe  fld        dword ptr [esp + 0x3c]
0045b302  fcom       st(1)
0045b304  fnstsw     ax
0045b306  fstp       st(1)
0045b308  test       ah, 5
0045b30b  jnp        0x45b5d3

// block 0045b311
0045b311  fld1       
0045b313  fcompp     
0045b315  fnstsw     ax
0045b317  test       ah, 5
0045b31a  jnp        0x45b5d5

// block 0045b320
0045b320  mov        eax, dword ptr [0x4cee34]
0045b325  lea        edx, [esp + 0x58]
0045b329  push       edx
0045b32a  lea        ecx, [eax + 0x4c]
0045b32d  push       ecx
0045b32e  lea        edx, [eax + 0x8c]
0045b334  push       edx
0045b335  lea        ecx, [eax + 0xcc]
0045b33b  push       ecx
0045b33c  add        eax, 0x30
0045b33f  push       eax
0045b340  lea        edx, [esp + 0x5c]
0045b344  push       edx
0045b345  call       0x46c6d4

// block 0045b34a
0045b34a  fld        dword ptr [esp + 0x48]
0045b34e  fsub       dword ptr [esp + 0x34]
0045b352  push       ecx
0045b353  fstp       dword ptr [esp + 0x18]
0045b357  fld        dword ptr [esp + 0x50]
0045b35b  fsub       dword ptr [esp + 0x3c]
0045b35f  fstp       dword ptr [esp + 0x1c]
0045b363  fld        dword ptr [esp + 0x54]
0045b367  fsub       dword ptr [esp + 0x40]
0045b36b  fstp       dword ptr [esp + 0x20]
0045b36f  fld        dword ptr [esp + 0x1c]
0045b373  fld        dword ptr [esp + 0x18]
0045b377  fld        dword ptr [esp + 0x20]
0045b37b  fld        st(1)
0045b37d  fmulp      st(2)
0045b37f  fld        st(2)
0045b381  fmulp      st(3)
0045b383  fxch       st(1)
0045b385  faddp      st(2)
0045b387  fmul       st(0), st(0)
0045b389  faddp      st(1)
0045b38b  fstp       dword ptr [esp + 0x28]
0045b38f  fld        dword ptr [esp + 0x28]
0045b393  fstp       dword ptr [esp]
0045b396  call       0x408860

// block 0045b39b
0045b39b  fmul       qword ptr [0x4a3cf8]
0045b3a1  fstp       dword ptr [esp + 0x24]
0045b3a5  fld        dword ptr [esi + 0x58]
0045b3a8  fld        dword ptr [esp + 0x24]
0045b3ac  fld        st(0)
0045b3ae  fmulp      st(2)
0045b3b0  fld        dword ptr [esi + 0x40]
0045b3b3  fmulp      st(2)
0045b3b5  fxch       st(1)
0045b3b7  fstp       dword ptr [esp + 0x40]
0045b3bb  fmul       dword ptr [esi + 0x5c]
0045b3be  fmul       dword ptr [esi + 0x44]
0045b3c1  fstp       dword ptr [esp + 0x30]
0045b3c5  fld        dword ptr [esp + 0x34]
0045b3c9  fstp       dword ptr [esp + 0x24]
0045b3cd  fld        dword ptr [esp + 0x38]
0045b3d1  fstp       dword ptr [esp + 0x54]
0045b3d5  fld        dword ptr [esp + 0x3c]
0045b3d9  fst        dword ptr [0x4d4844]
0045b3df  fst        dword ptr [0x4d4828]
0045b3e5  fst        dword ptr [0x4d480c]
0045b3eb  fstp       dword ptr [0x4d47f0]
0045b3f1  fld        dword ptr [esp + 0x44]
0045b3f5  fsincos    
0045b3f7  fstp       dword ptr [esp + 0x2c]
0045b3fb  fstp       dword ptr [esp + 0x28]
0045b3ff  mov        esi, dword ptr [esi + 0x47c]
0045b405  mov        eax, esi
0045b407  shr        eax, 0x13
0045b40a  and        eax, 3
0045b40d  sub        eax, 0
0045b410  je         0x45b452

// block 0045b412
0045b412  sub        eax, 1
0045b415  je         0x45b43a

// block 0045b417
0045b417  sub        eax, 1
0045b41a  jne        0x45b47e

// block 0045b41c
0045b41c  fld        dword ptr [esp + 0x40]
0045b420  fchs       
0045b422  fstp       dword ptr [esp + 0xc]
0045b426  fld        dword ptr [esp + 0xc]
0045b42a  fstp       dword ptr [esp + 4]
0045b42e  fldz       
0045b430  fst        dword ptr [esp + 0x10]
0045b434  fst        dword ptr [esp + 8]
0045b438  jmp        0x45b480

// block 0045b43a
0045b43a  fldz       
0045b43c  fst        dword ptr [esp + 0xc]
0045b440  fst        dword ptr [esp + 4]
0045b444  fld        dword ptr [esp + 0x40]
0045b448  fst        dword ptr [esp + 0x10]
0045b44c  fstp       dword ptr [esp + 8]
0045b450  jmp        0x45b480

// block 0045b452
0045b452  fld        dword ptr [esp + 0x40]
0045b456  fld        st(0)
0045b458  fchs       
0045b45a  fld        qword ptr [0x4a3cf8]
0045b460  fmul       st(1), st(0)
0045b462  fxch       st(1)
0045b464  fstp       dword ptr [esp + 0xc]
0045b468  fld        dword ptr [esp + 0xc]
0045b46c  fstp       dword ptr [esp + 4]
0045b470  fmulp      st(1)
0045b472  fstp       dword ptr [esp + 0x10]
0045b476  fld        dword ptr [esp + 0x10]
0045b47a  fstp       dword ptr [esp + 8]

// block 0045b47e
0045b47e  fldz       

// block 0045b480
0045b480  shr        esi, 0x15
0045b483  and        esi, 3
0045b486  sub        esi, 0
0045b489  je         0x45b4c7

// block 0045b48b
0045b48b  sub        esi, 1
0045b48e  je         0x45b4b1

// block 0045b490
0045b490  sub        esi, 1
0045b493  jne        0x45b4f7

// block 0045b495
0045b495  fld        dword ptr [esp + 0x30]
0045b499  fchs       
0045b49b  fstp       dword ptr [esp + 0x18]
0045b49f  fld        dword ptr [esp + 0x18]
0045b4a3  fstp       dword ptr [esp + 0x14]
0045b4a7  fst        dword ptr [esp + 0x20]
0045b4ab  fstp       dword ptr [esp + 0x1c]
0045b4af  jmp        0x45b4f9

// block 0045b4b1
0045b4b1  fst        dword ptr [esp + 0x18]
0045b4b5  fstp       dword ptr [esp + 0x14]
0045b4b9  fld        dword ptr [esp + 0x30]
0045b4bd  fst        dword ptr [esp + 0x20]
0045b4c1  fstp       dword ptr [esp + 0x1c]
0045b4c5  jmp        0x45b4f9

// block 0045b4c7
0045b4c7  fstp       st(0)
0045b4c9  fld        dword ptr [esp + 0x30]
0045b4cd  fld        st(0)
0045b4cf  fchs       
0045b4d1  fld        qword ptr [0x4a3cf8]
0045b4d7  fmul       st(1), st(0)
0045b4d9  fxch       st(1)
0045b4db  fstp       dword ptr [esp + 0x18]
0045b4df  fld        dword ptr [esp + 0x18]
0045b4e3  fstp       dword ptr [esp + 0x14]
0045b4e7  fmulp      st(1)
0045b4e9  fstp       dword ptr [esp + 0x20]
0045b4ed  fld        dword ptr [esp + 0x20]
0045b4f1  fstp       dword ptr [esp + 0x1c]
0045b4f5  jmp        0x45b4f9

// block 0045b4f7
0045b4f7  fstp       st(0)

// block 0045b4f9
0045b4f9  fld        dword ptr [esp + 4]
0045b4fd  xor        eax, eax
0045b4ff  fld        st(0)
0045b501  fld        dword ptr [esp + 0x2c]
0045b505  fld        st(0)
0045b507  fmulp      st(2)
0045b509  fld        dword ptr [esp + 0x14]
0045b50d  fld        st(0)
0045b50f  fld        dword ptr [esp + 0x28]
0045b513  fld        st(0)
0045b515  fmulp      st(2)
0045b517  fxch       st(4)
0045b519  fsubrp     st(1)
0045b51b  fld        dword ptr [esp + 0x24]
0045b51f  fld        st(0)
0045b521  faddp      st(2)
0045b523  fxch       st(1)
0045b525  fstp       dword ptr [0x4d47e8]
0045b52b  fld        st(2)
0045b52d  fmulp      st(2)
0045b52f  fld        st(3)
0045b531  fmulp      st(5)
0045b533  fxch       st(1)
0045b535  faddp      st(4)
0045b537  fld        dword ptr [esp + 0x54]
0045b53b  fld        st(0)
0045b53d  faddp      st(5)
0045b53f  fxch       st(4)
0045b541  fstp       dword ptr [0x4d47ec]
0045b547  fld        dword ptr [esp + 8]
0045b54b  fld        st(0)
0045b54d  fmul       st(3)
0045b54f  fld        dword ptr [esp + 0x18]
0045b553  fmul       st(5)
0045b555  fsubp      st(1)
0045b557  fadd       st(2)
0045b559  fstp       dword ptr [0x4d4804]
0045b55f  fmul       st(3)
0045b561  fld        dword ptr [esp + 0x18]
0045b565  fmul       st(3)
0045b567  faddp      st(1)
0045b569  fadd       st(4)
0045b56b  fstp       dword ptr [0x4d4808]
0045b571  fld        dword ptr [esp + 0xc]
0045b575  fld        st(0)
0045b577  fmul       st(3)
0045b579  fld        dword ptr [esp + 0x1c]
0045b57d  fmul       st(5)
0045b57f  fsubp      st(1)
0045b581  fadd       st(2)
0045b583  fstp       dword ptr [0x4d4820]
0045b589  fld        dword ptr [esp + 0x1c]
0045b58d  fmul       st(3)
0045b58f  fld        st(4)
0045b591  fmulp      st(2)
0045b593  faddp      st(1)
0045b595  fadd       st(4)
0045b597  fstp       dword ptr [0x4d4824]
0045b59d  fld        dword ptr [esp + 0x10]
0045b5a1  fmul       st(2)
0045b5a3  fld        dword ptr [esp + 0x20]
0045b5a7  fld        st(0)
0045b5a9  fmul       st(5)
0045b5ab  fsubp      st(2)
0045b5ad  fxch       st(1)
0045b5af  faddp      st(2)
0045b5b1  fxch       st(1)
0045b5b3  fstp       dword ptr [0x4d483c]
0045b5b9  fmulp      st(1)
0045b5bb  fld        dword ptr [esp + 0x10]
0045b5bf  fmulp      st(2)
0045b5c1  faddp      st(1)
0045b5c3  faddp      st(1)
0045b5c5  fstp       dword ptr [0x4d4840]
0045b5cb  pop        esi
0045b5cc  add        esp, 0x98
0045b5d2  ret        

// block 0045b5d3
0045b5d3  fstp       st(0)

// block 0045b5d5
0045b5d5  or         eax, 0xffffffff
0045b5d8  pop        esi
0045b5d9  add        esp, 0x98
0045b5df  ret        
