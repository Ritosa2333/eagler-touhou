
// block 0042cca0
0042cca0  sub        esp, 0x10
0042cca3  fldz       
0042cca5  push       ebx
0042cca6  push       ebp
0042cca7  fstp       dword ptr [esp + 0xc]
0042ccab  push       esi
0042ccac  push       edi
0042ccad  mov        edi, ecx
0042ccaf  mov        esi, dword ptr [edi + 0xfa0]
0042ccb5  mov        ebx, dword ptr [edi + 0xf9c]
0042ccbb  xor        ebp, ebp
0042ccbd  cmp        dword ptr [edi + 0x470], ebp
0042ccc3  jle        0x42cfac

// block 0042ccc9
0042ccc9  lea        esp, [esp]

// block 0042ccd0
0042ccd0  fld        qword ptr [0x4a4058]
0042ccd6  mov        dword ptr [esi + 0x10], 0xffffffff
0042ccdd  fld1       
0042ccdf  push       ecx
0042cce0  fstp       dword ptr [esi + 0xc]
0042cce3  fld        dword ptr [esp + 0x18]
0042cce7  fstp       dword ptr [esi + 0x14]
0042ccea  fld        dword ptr [edi + 0x6b4]
0042ccf0  fstp       dword ptr [esi + 0x18]
0042ccf3  fadd       dword ptr [ebx + 0xc]
0042ccf6  test       ebp, ebp
0042ccf8  jne        0x42cd07

// block 0042ccfa
0042ccfa  fstp       dword ptr [esp + 0x1c]
0042ccfe  fld        dword ptr [esp + 0x1c]
0042cd02  jmp        0x42cdbe

// block 0042cd07
0042cd07  fstp       dword ptr [esp + 0x1c]
0042cd0b  fld        dword ptr [esp + 0x1c]
0042cd0f  fstp       dword ptr [esp]
0042cd12  call       0x4646e0

// block 0042cd17
0042cd17  fstp       dword ptr [esp + 0x10]
0042cd1b  push       ecx
0042cd1c  fld        dword ptr [ebx - 8]
0042cd1f  fadd       qword ptr [0x4a4058]
0042cd25  fstp       dword ptr [esp + 0x1c]
0042cd29  fld        dword ptr [esp + 0x1c]
0042cd2d  fstp       dword ptr [esp]
0042cd30  call       0x4646e0

// block 0042cd35
0042cd35  fstp       dword ptr [esp + 0x1c]
0042cd39  fld        dword ptr [esp + 0x1c]
0042cd3d  fld        st(0)
0042cd3f  fld        dword ptr [esp + 0x10]
0042cd43  fld        st(0)
0042cd45  fsubp      st(2)
0042cd47  fld        qword ptr [0x4a3cd8]
0042cd4d  fcom       st(2)
0042cd4f  fnstsw     ax
0042cd51  test       ah, 5
0042cd54  jp         0x42cd68

// block 0042cd56
0042cd56  fstp       st(2)
0042cd58  fstp       st(1)
0042cd5a  fadd       qword ptr [0x4a3cd0]
0042cd60  fsubp      st(1)
0042cd62  fstp       dword ptr [esp + 0x18]
0042cd66  jmp        0x42cd8d

// block 0042cd68
0042cd68  fld        st(1)
0042cd6a  fsub       st(4)
0042cd6c  fcompp     
0042cd6e  fnstsw     ax
0042cd70  test       ah, 0x41
0042cd73  jne        0x42cd85

// block 0042cd75
0042cd75  fstp       st(1)
0042cd77  fsub       qword ptr [0x4a3cd0]
0042cd7d  fsubp      st(1)
0042cd7f  fstp       dword ptr [esp + 0x18]
0042cd83  jmp        0x42cd8d

// block 0042cd85
0042cd85  fstp       st(0)
0042cd87  fstp       st(1)
0042cd89  fstp       dword ptr [esp + 0x18]

// block 0042cd8d
0042cd8d  fld        dword ptr [esp + 0x18]
0042cd91  push       ecx
0042cd92  fstp       dword ptr [esp]
0042cd95  call       0x4646e0

// block 0042cd9a
0042cd9a  fmul       qword ptr [0x4a3cf8]
0042cda0  push       ecx
0042cda1  fstp       dword ptr [esp + 0x1c]
0042cda5  fld        dword ptr [esp + 0x1c]
0042cda9  fstp       dword ptr [esp]
0042cdac  call       0x4646e0

// block 0042cdb1
0042cdb1  fadd       dword ptr [esp + 0x10]
0042cdb5  push       ecx
0042cdb6  fstp       dword ptr [esp + 0x1c]
0042cdba  fld        dword ptr [esp + 0x1c]

// block 0042cdbe
0042cdbe  fstp       dword ptr [esp]
0042cdc1  call       0x4646e0

// block 0042cdc6
0042cdc6  fstp       dword ptr [esp + 0x10]
0042cdca  sub        esp, 8
0042cdcd  fld        dword ptr [edi + 0x464]
0042cdd3  mov        ecx, esi
0042cdd5  fmul       qword ptr [0x4a3cf8]
0042cddb  fstp       dword ptr [esp + 0x20]
0042cddf  fld        dword ptr [esp + 0x20]
0042cde3  fstp       dword ptr [esp + 4]
0042cde7  fld        dword ptr [esp + 0x18]
0042cdeb  fstp       dword ptr [esp]
0042cdee  call       0x42e880

// block 0042cdf3
0042cdf3  fld        dword ptr [ebx]
0042cdf5  add        esi, 0x1c
0042cdf8  fadd       dword ptr [esi - 0x1c]
0042cdfb  push       ecx
0042cdfc  fstp       dword ptr [esi - 0x1c]
0042cdff  fld        dword ptr [ebx + 4]
0042ce02  fadd       dword ptr [esi - 0x18]
0042ce05  fstp       dword ptr [esi - 0x18]
0042ce08  fld        dword ptr [esi - 0x1c]
0042ce0b  fadd       qword ptr [0x4a3d70]
0042ce11  fadd       qword ptr [0x4a3d28]
0042ce17  fstp       dword ptr [esi - 0x1c]
0042ce1a  fld        dword ptr [esi - 0x18]
0042ce1d  fadd       qword ptr [0x4a3d68]
0042ce23  fstp       dword ptr [esi - 0x18]
0042ce26  fldz       
0042ce28  fstp       dword ptr [esi - 0x14]
0042ce2b  fld1       
0042ce2d  mov        dword ptr [esi + 0x10], 0xffffffff
0042ce34  fstp       dword ptr [esi + 0xc]
0042ce37  fld        dword ptr [esp + 0x18]
0042ce3b  fstp       dword ptr [esi + 0x14]
0042ce3e  fld        dword ptr [edi + 0x6c4]
0042ce44  fstp       dword ptr [esi + 0x18]
0042ce47  fld        dword ptr [ebx + 0xc]
0042ce4a  fsub       qword ptr [0x4a4058]
0042ce50  test       ebp, ebp
0042ce52  jne        0x42ce61

// block 0042ce54
0042ce54  fstp       dword ptr [esp + 0x1c]
0042ce58  fld        dword ptr [esp + 0x1c]
0042ce5c  jmp        0x42cf18

// block 0042ce61
0042ce61  fstp       dword ptr [esp + 0x1c]
0042ce65  fld        dword ptr [esp + 0x1c]
0042ce69  fstp       dword ptr [esp]
0042ce6c  call       0x4646e0

// block 0042ce71
0042ce71  fstp       dword ptr [esp + 0x10]
0042ce75  push       ecx
0042ce76  fld        dword ptr [ebx - 8]
0042ce79  fsub       qword ptr [0x4a4058]
0042ce7f  fstp       dword ptr [esp + 0x1c]
0042ce83  fld        dword ptr [esp + 0x1c]
0042ce87  fstp       dword ptr [esp]
0042ce8a  call       0x4646e0

// block 0042ce8f
0042ce8f  fstp       dword ptr [esp + 0x1c]
0042ce93  fld        dword ptr [esp + 0x1c]
0042ce97  fld        st(0)
0042ce99  fld        dword ptr [esp + 0x10]
0042ce9d  fld        st(0)
0042ce9f  fsubp      st(2)
0042cea1  fld        qword ptr [0x4a3cd8]
0042cea7  fcom       st(2)
0042cea9  fnstsw     ax
0042ceab  test       ah, 5
0042ceae  jp         0x42cec2

// block 0042ceb0
0042ceb0  fstp       st(2)
0042ceb2  fstp       st(1)
0042ceb4  fadd       qword ptr [0x4a3cd0]
0042ceba  fsubp      st(1)
0042cebc  fstp       dword ptr [esp + 0x18]
0042cec0  jmp        0x42cee7

// block 0042cec2
0042cec2  fld        st(1)
0042cec4  fsub       st(4)
0042cec6  fcompp     
0042cec8  fnstsw     ax
0042ceca  test       ah, 0x41
0042cecd  jne        0x42cedf

// block 0042cecf
0042cecf  fstp       st(1)
0042ced1  fsub       qword ptr [0x4a3cd0]
0042ced7  fsubp      st(1)
0042ced9  fstp       dword ptr [esp + 0x18]
0042cedd  jmp        0x42cee7

// block 0042cedf
0042cedf  fstp       st(0)
0042cee1  fstp       st(1)
0042cee3  fstp       dword ptr [esp + 0x18]

// block 0042cee7
0042cee7  fld        dword ptr [esp + 0x18]
0042ceeb  push       ecx
0042ceec  fstp       dword ptr [esp]
0042ceef  call       0x4646e0

// block 0042cef4
0042cef4  fmul       qword ptr [0x4a3cf8]
0042cefa  push       ecx
0042cefb  fstp       dword ptr [esp + 0x1c]
0042ceff  fld        dword ptr [esp + 0x1c]
0042cf03  fstp       dword ptr [esp]
0042cf06  call       0x4646e0

// block 0042cf0b
0042cf0b  fadd       dword ptr [esp + 0x10]
0042cf0f  push       ecx
0042cf10  fstp       dword ptr [esp + 0x1c]
0042cf14  fld        dword ptr [esp + 0x1c]

// block 0042cf18
0042cf18  fstp       dword ptr [esp]
0042cf1b  call       0x4646e0

// block 0042cf20
0042cf20  fstp       dword ptr [esp + 0x10]
0042cf24  sub        esp, 8
0042cf27  fld        dword ptr [edi + 0x464]
0042cf2d  mov        ecx, esi
0042cf2f  fmul       qword ptr [0x4a3cf8]
0042cf35  fstp       dword ptr [esp + 0x20]
0042cf39  fld        dword ptr [esp + 0x20]
0042cf3d  fstp       dword ptr [esp + 4]
0042cf41  fld        dword ptr [esp + 0x18]
0042cf45  fstp       dword ptr [esp]
0042cf48  call       0x42e880

// block 0042cf4d
0042cf4d  fld        dword ptr [ebx]
0042cf4f  inc        ebp
0042cf50  fadd       dword ptr [esi]
0042cf52  add        esi, 0x1c
0042cf55  add        ebx, 0x14
0042cf58  fstp       dword ptr [esi - 0x1c]
0042cf5b  fld        dword ptr [ebx - 0x10]
0042cf5e  fadd       dword ptr [esi - 0x18]
0042cf61  fstp       dword ptr [esi - 0x18]
0042cf64  fld        dword ptr [esi - 0x1c]
0042cf67  fadd       qword ptr [0x4a3d70]
0042cf6d  fadd       qword ptr [0x4a3d28]
0042cf73  fstp       dword ptr [esi - 0x1c]
0042cf76  fld        dword ptr [esi - 0x18]
0042cf79  fadd       qword ptr [0x4a3d68]
0042cf7f  fstp       dword ptr [esi - 0x18]
0042cf82  fldz       
0042cf84  fstp       dword ptr [esi - 0x14]
0042cf87  mov        eax, dword ptr [edi + 0x470]
0042cf8d  cmp        ebp, eax
0042cf8f  lea        ecx, [eax - 1]
0042cf92  mov        dword ptr [esp + 0x18], ecx
0042cf96  fild       dword ptr [esp + 0x18]
0042cf9a  fld1       
0042cf9c  fdivrp     st(1)
0042cf9e  fadd       dword ptr [esp + 0x14]
0042cfa2  fstp       dword ptr [esp + 0x14]
0042cfa6  jl         0x42ccd0

// block 0042cfac
0042cfac  mov        edx, dword ptr [edi + 0x470]
0042cfb2  mov        eax, dword ptr [edi + 0xfa0]
0042cfb8  mov        ecx, dword ptr [0x4ce8cc]
0042cfbe  add        edx, edx
0042cfc0  push       edx
0042cfc1  push       eax
0042cfc2  lea        eax, [edi + 0x634]
0042cfc8  call       0x45c3a0

// block 0042cfcd
0042cfcd  mov        eax, dword ptr [edi + 0x470]
0042cfd3  cmp        eax, dword ptr [edi + 0x18]
0042cfd6  jl         0x42d025

// block 0042cfd8
0042cfd8  mov        edx, dword ptr [edi + 0xf9c]
0042cfde  lea        ecx, [eax + eax*4]
0042cfe1  fld        dword ptr [edx + ecx*4 - 0x14]
0042cfe5  lea        eax, [edx + ecx*4 - 0x14]
0042cfe9  fadd       qword ptr [0x4a3d70]
0042cfef  mov        ecx, dword ptr [0x4ce8cc]
0042cff5  push       ecx
0042cff6  fadd       qword ptr [0x4a3d28]
0042cffc  fstp       dword ptr [edi + 0xf0c]
0042d002  fld        dword ptr [eax + 4]
0042d005  fadd       qword ptr [0x4a3d68]
0042d00b  fstp       dword ptr [edi + 0xf10]
0042d011  fld        dword ptr [eax + 8]
0042d014  lea        eax, [edi + 0xae8]
0042d01a  fstp       dword ptr [edi + 0xf14]
0042d020  call       0x45c900

// block 0042d025
0042d025  pop        edi
0042d026  pop        esi
0042d027  pop        ebp
0042d028  xor        eax, eax
0042d02a  pop        ebx
0042d02b  add        esp, 0x10
0042d02e  ret        
