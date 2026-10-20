
// block 0040caa0
0040caa0  mov        eax, dword ptr [0x4b43cc]
0040caa5  sub        esp, 0x14
0040caa8  push       ebp
0040caa9  push       esi
0040caaa  mov        esi, dword ptr [0x4b43c8]
0040cab0  add        esi, 0x64
0040cab3  test       byte ptr [eax + 0x7c], 1
0040cab7  push       edi
0040cab8  je         0x40cad5

// block 0040caba
0040caba  mov        eax, dword ptr [eax + 0x78]
0040cabd  cmp        eax, 0x60
0040cac0  jl         0x40cad5

// block 0040cac2
0040cac2  cmp        eax, 0x63
0040cac5  jg         0x40cad5

// block 0040cac7
0040cac7  fld        dword ptr [esp + 0x24]
0040cacb  fdiv       qword ptr [0x4a3fd8]
0040cad1  fstp       dword ptr [esp + 0x24]

// block 0040cad5
0040cad5  mov        ebp, 0x7d0

// block 0040cada
0040cada  movzx      eax, word ptr [esi + 0x532]
0040cae1  test       ax, ax
0040cae4  je         0x40cbab

// block 0040caea
0040caea  cmp        ax, 3
0040caee  je         0x40cbab

// block 0040caf4
0040caf4  cmp        dword ptr [esp + 0x2c], 0
0040caf9  je         0x40cb05

// block 0040cafb
0040cafb  cmp        dword ptr [esi + 4], 0
0040caff  jne        0x40cbab

// block 0040cb05
0040cb05  fld        dword ptr [esi + 0x4bc]
0040cb0b  lea        edi, [esi + 0x4bc]
0040cb11  fsub       dword ptr [ebx]
0040cb13  fstp       dword ptr [esp + 0x14]
0040cb17  fld        dword ptr [esi + 0x4c0]
0040cb1d  fsub       dword ptr [ebx + 4]
0040cb20  fstp       dword ptr [esp + 0x18]
0040cb24  fld        dword ptr [esi + 0x4dc]
0040cb2a  fmul       qword ptr [0x4a3cf8]
0040cb30  fadd       dword ptr [esp + 0x24]
0040cb34  fstp       dword ptr [esp + 0x10]
0040cb38  fld        dword ptr [esp + 0x18]
0040cb3c  fld        dword ptr [esp + 0x14]
0040cb40  fld        dword ptr [esp + 0x10]
0040cb44  fld        st(1)
0040cb46  fmulp      st(2)
0040cb48  fld        st(2)
0040cb4a  fmulp      st(3)
0040cb4c  fxch       st(1)
0040cb4e  faddp      st(2)
0040cb50  fxch       st(1)
0040cb52  fstp       dword ptr [esp + 0x10]
0040cb56  fld        dword ptr [esp + 0x10]
0040cb5a  fld        st(1)
0040cb5c  fmulp      st(2)
0040cb5e  fcompp     
0040cb60  fnstsw     ax
0040cb62  test       ah, 0x41
0040cb65  je         0x40cbab

// block 0040cb67
0040cb67  call       0x40c8b0

// block 0040cb6c
0040cb6c  fld        dword ptr [0x4a4014]
0040cb72  sub        esp, 8
0040cb75  fst        dword ptr [esp + 4]
0040cb79  mov        ecx, edi
0040cb7b  fstp       dword ptr [esp]
0040cb7e  call       0x40d560

// block 0040cb83
0040cb83  test       eax, eax
0040cb85  jne        0x40cbab

// block 0040cb87
0040cb87  cmp        dword ptr [esp + 0x28], eax
0040cb8b  je         0x40cbab

// block 0040cb8d
0040cb8d  fld        dword ptr [0x4a41f4]
0040cb93  sub        esp, 8
0040cb96  fstp       dword ptr [esp + 4]
0040cb9a  fld        dword ptr [0x4a4060]
0040cba0  fstp       dword ptr [esp]
0040cba3  push       edi
0040cba4  push       9
0040cba6  call       0x4273f0

// block 0040cbab
0040cbab  add        esi, 0x9f8
0040cbb1  sub        ebp, 1
0040cbb4  jne        0x40cada

// block 0040cbba
0040cbba  mov        eax, dword ptr [esp + 0x28]
0040cbbe  fld        dword ptr [esp + 0x24]
0040cbc2  push       eax
0040cbc3  push       ecx
0040cbc4  fstp       dword ptr [esp]
0040cbc7  push       ebx
0040cbc8  call       0x414f40

// block 0040cbcd
0040cbcd  pop        edi
0040cbce  pop        esi
0040cbcf  xor        eax, eax
0040cbd1  pop        ebp
0040cbd2  add        esp, 0x14
0040cbd5  ret        0xc
