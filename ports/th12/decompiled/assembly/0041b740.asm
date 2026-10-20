
// block 0041b740
0041b740  push       ebp
0041b741  mov        ebp, esp
0041b743  and        esp, 0xfffffff8
0041b746  sub        esp, 0x70
0041b749  push       ebx
0041b74a  push       ebp
0041b74b  push       esi
0041b74c  push       edi
0041b74d  mov        ebx, ecx
0041b74f  push       0x214
0041b754  lea        esi, [ebx + 0x48c]
0041b75a  push       0
0041b75c  push       esi
0041b75d  call       0x477420

// block 0041b762
0041b762  fldz       
0041b764  mov        ebp, 1
0041b769  fst        dword ptr [ebx + 0x49c]
0041b76f  mov        eax, ebp
0041b771  fstp       dword ptr [ebx + 0x4a4]
0041b777  mov        ecx, 0x17
0041b77c  mov        word ptr [esi], cx
0041b77f  mov        word ptr [ebx + 0x68a], ax
0041b786  mov        word ptr [ebx + 0x684], ax
0041b78d  mov        eax, dword ptr [0x4b44f4]
0041b792  xor        edx, edx
0041b794  mov        ecx, ebp
0041b796  mov        word ptr [ebx + 0x48e], dx
0041b79d  mov        word ptr [ebx + 0x686], cx
0041b7a4  mov        dword ptr [ebx + 0x690], 0x16
0041b7ae  mov        dword ptr [ebx + 0x694], 0x28
0041b7b8  mov        dword ptr [ebx + 0x68c], 0x83
0041b7c2  mov        edi, dword ptr [eax + 0x18]
0041b7c5  add        esp, 0xc
0041b7c8  mov        dword ptr [eax + 0x48c], edi
0041b7ce  test       edi, edi
0041b7d0  je         0x41bb30

// block 0041b7d6
0041b7d6  fld        qword ptr [0x4a42a0]
0041b7dc  call       0x4938c0

// block 0041b7e1
0041b7e1  fstp       dword ptr [esp + 0x18]
0041b7e5  fld        dword ptr [esp + 0x18]
0041b7e9  fstp       dword ptr [esp + 0x20]
0041b7ed  fld        qword ptr [0x4a42a0]
0041b7f3  call       0x4939f0

// block 0041b7f8
0041b7f8  fstp       dword ptr [esp + 0x18]
0041b7fc  fld        dword ptr [esp + 0x18]
0041b800  fstp       dword ptr [esp + 0x1c]
0041b804  fld        qword ptr [0x4a4298]
0041b80a  call       0x4938c0

// block 0041b80f
0041b80f  fstp       dword ptr [esp + 0x18]
0041b813  fld        dword ptr [esp + 0x18]
0041b817  fstp       dword ptr [esp + 0x28]
0041b81b  fld        qword ptr [0x4a4298]
0041b821  call       0x4939f0

// block 0041b826
0041b826  fstp       dword ptr [esp + 0x18]
0041b82a  fld        dword ptr [esp + 0x18]
0041b82e  fstp       dword ptr [esp + 0x24]
0041b832  jmp        0x41b839

// block 0041b834
0041b834  mov        ebp, 1

// block 0041b839
0041b839  fld        dword ptr [edi + 0x50]
0041b83c  push       ecx
0041b83d  fsub       dword ptr [edi + 0x454]
0041b843  fstp       dword ptr [esp + 0x54]
0041b847  fld        dword ptr [edi + 0x54]
0041b84a  fsub       dword ptr [edi + 0x458]
0041b850  fstp       dword ptr [esp + 0x58]
0041b854  fld        dword ptr [edi + 0x68]
0041b857  fadd       qword ptr [0x4a4290]
0041b85d  fadd       qword ptr [0x4a3fe0]
0041b863  fstp       dword ptr [esp + 0x1c]
0041b867  fld        dword ptr [esp + 0x1c]
0041b86b  fstp       dword ptr [esp]
0041b86e  call       0x4646e0

// block 0041b873
0041b873  fstp       dword ptr [esp + 0x18]
0041b877  sub        esp, 8
0041b87a  fld        dword ptr [esp + 0x24]
0041b87e  lea        ecx, [esp + 0x70]
0041b882  fld        st(0)
0041b884  fld        dword ptr [esp + 0x58]
0041b888  fld        st(0)
0041b88a  fmulp      st(2)
0041b88c  fld        dword ptr [esp + 0x5c]
0041b890  fld        st(0)
0041b892  fld        dword ptr [esp + 0x28]
0041b896  fld        st(0)
0041b898  fmulp      st(2)
0041b89a  fxch       st(4)
0041b89c  fsubrp     st(1)
0041b89e  fstp       dword ptr [esp + 0x40]
0041b8a2  fmulp      st(3)
0041b8a4  fmulp      st(1)
0041b8a6  faddp      st(1)
0041b8a8  fstp       dword ptr [esp + 0x44]
0041b8ac  fld        dword ptr [0x4a4110]
0041b8b2  fstp       dword ptr [esp + 4]
0041b8b6  fld        dword ptr [esp + 0x20]
0041b8ba  fstp       dword ptr [esp]
0041b8bd  call       0x41c580

// block 0041b8c2
0041b8c2  fld        dword ptr [edi + 0x454]
0041b8c8  sub        esp, 8
0041b8cb  fadd       dword ptr [esp + 0x40]
0041b8cf  fstp       dword ptr [esp + 0x40]
0041b8d3  fld        dword ptr [edi + 0x458]
0041b8d9  fadd       dword ptr [esp + 0x44]
0041b8dd  fstp       dword ptr [esp + 0x44]
0041b8e1  fld        dword ptr [esp + 0x2c]
0041b8e5  fld        st(0)
0041b8e7  fld        dword ptr [esp + 0x58]
0041b8eb  fld        st(0)
0041b8ed  fmulp      st(2)
0041b8ef  fld        dword ptr [esp + 0x5c]
0041b8f3  fld        st(0)
0041b8f5  fld        dword ptr [esp + 0x30]
0041b8f9  fld        st(0)
0041b8fb  fmulp      st(2)
0041b8fd  fxch       st(4)
0041b8ff  fsubrp     st(1)
0041b901  fstp       dword ptr [esp + 0x4c]
0041b905  fmulp      st(3)
0041b907  fmulp      st(1)
0041b909  faddp      st(1)
0041b90b  fstp       dword ptr [esp + 0x50]
0041b90f  fld        dword ptr [0x4a4110]
0041b915  fstp       dword ptr [esp + 4]
0041b919  fld        dword ptr [edi + 0x68]
0041b91c  fsub       qword ptr [0x4a4290]
0041b922  fsub       qword ptr [0x4a3fe0]
0041b928  fstp       dword ptr [esp + 0x1c]
0041b92c  fld        dword ptr [esp + 0x1c]
0041b930  fstp       dword ptr [esp]
0041b933  call       0x4646e0

// block 0041b938
0041b938  push       ecx
0041b939  lea        ecx, [esp + 0x7c]
0041b93d  fstp       dword ptr [esp]
0041b940  call       0x41c580

// block 0041b945
0041b945  fld        dword ptr [esp + 0x44]
0041b949  fadd       dword ptr [edi + 0x454]
0041b94f  fstp       dword ptr [esp + 0x44]
0041b953  fld        dword ptr [edi + 0x458]
0041b959  fadd       dword ptr [esp + 0x48]
0041b95d  mov        ecx, dword ptr [edi + 0x58]
0041b960  fstp       dword ptr [esp + 0x48]
0041b964  fldz       
0041b966  mov        edx, dword ptr [edi + 0x50]
0041b969  mov        eax, dword ptr [edi + 0x54]
0041b96c  fstp       dword ptr [esp + 0x14]
0041b970  fld        dword ptr [0x4a4110]
0041b976  sub        esp, 8
0041b979  fstp       dword ptr [esp + 4]
0041b97d  mov        dword ptr [esp + 0x3c], ecx
0041b981  fld        dword ptr [edi + 0x68]
0041b984  lea        ecx, [esp + 0x64]
0041b988  fstp       dword ptr [esp]
0041b98b  mov        dword ptr [esp + 0x34], edx
0041b98f  mov        dword ptr [esp + 0x38], eax
0041b993  call       0x41c580

// block 0041b998
0041b998  fld        dword ptr [esp + 0x18]
0041b99c  mov        dword ptr [ebx + 0x508], 0x2000
0041b9a6  mov        dword ptr [esi + 0x50], ebp
0041b9a9  mov        dword ptr [esi + 0x48], 0
0041b9b0  mov        dword ptr [esi + 0x4c], 0x400
0041b9b7  mov        ecx, 0xb4
0041b9bc  mov        dword ptr [esi + 0x44], ecx
0041b9bf  mov        dword ptr [esi + 0x34], 0x200
0041b9c6  mov        dword ptr [esi + 0x2c], 0x4b0
0041b9cd  fstp       dword ptr [ebx + 0x588]
0041b9d3  mov        dword ptr [ebx + 0x590], ebp
0041b9d9  fld        dword ptr [edi + 0x74]
0041b9dc  fstp       dword ptr [ebx + 0x58c]
0041b9e2  xor        ebp, ebp
0041b9e4  fld        dword ptr [edi + 0x454]
0041b9ea  fstp       dword ptr [ebx + 0x5a0]
0041b9f0  fld        dword ptr [edi + 0x458]
0041b9f6  fstp       dword ptr [ebx + 0x5a4]
0041b9fc  fld        dword ptr [edi + 0x6c]
0041b9ff  fcomp      qword ptr [0x4a3d58]
0041ba05  fnstsw     ax
0041ba07  test       ah, 0x41
0041ba0a  jne        0x41bb02

// block 0041ba10
0041ba10  jmp        0x41ba17

// block 0041ba12
0041ba12  mov        ecx, 0xb4

// block 0041ba17
0041ba17  mov        edx, ebp
0041ba19  and        edx, 0x80000001
0041ba1f  jns        0x41ba26

// block 0041ba21
0041ba21  dec        edx
0041ba22  or         edx, 0xfffffffe
0041ba25  inc        edx

// block 0041ba26
0041ba26  fld        dword ptr [esp + 0x48]
0041ba2a  mov        dword ptr [esi + 0x68], 1
0041ba31  fld        dword ptr [esp + 0x44]
0041ba35  mov        dword ptr [esi + 0x5c], ecx
0041ba38  fld        dword ptr [esp + 0x3c]
0041ba3c  mov        dword ptr [esi + 0x60], 9
0041ba43  fld        dword ptr [esp + 0x38]
0041ba47  mov        dword ptr [esi + 0x64], 0x2000000
0041ba4e  jne        0x41ba5a

// block 0041ba50
0041ba50  fst        dword ptr [esi + 0x54]
0041ba53  fxch       st(1)
0041ba55  fst        dword ptr [esi + 0x58]
0041ba58  jmp        0x41ba6a

// block 0041ba5a
0041ba5a  fxch       st(2)
0041ba5c  fst        dword ptr [esi + 0x54]
0041ba5f  fxch       st(3)
0041ba61  fst        dword ptr [esi + 0x58]
0041ba64  fxch       st(3)
0041ba66  fxch       st(2)
0041ba68  fxch       st(1)

// block 0041ba6a
0041ba6a  fld        dword ptr [esp + 0x5c]
0041ba6e  mov        eax, dword ptr [esp + 0x2c]
0041ba72  fadd       dword ptr [esp + 0x2c]
0041ba76  mov        ecx, dword ptr [esp + 0x30]
0041ba7a  mov        edx, dword ptr [esp + 0x34]
0041ba7e  mov        dword ptr [ebx + 0x490], eax
0041ba84  fstp       dword ptr [esp + 0x2c]
0041ba88  mov        dword ptr [ebx + 0x494], ecx
0041ba8e  fld        dword ptr [esp + 0x60]
0041ba92  mov        dword ptr [ebx + 0x498], edx
0041ba98  fadd       dword ptr [esp + 0x30]
0041ba9c  fstp       dword ptr [esp + 0x30]
0041baa0  fld        dword ptr [esp + 0x64]
0041baa4  fadd       dword ptr [esp + 0x34]
0041baa8  fstp       dword ptr [esp + 0x34]
0041baac  fld        dword ptr [esp + 0x68]
0041bab0  faddp      st(2)
0041bab2  fxch       st(1)
0041bab4  fstp       dword ptr [esp + 0x38]
0041bab8  fadd       dword ptr [esp + 0x6c]
0041babc  fstp       dword ptr [esp + 0x3c]
0041bac0  fadd       dword ptr [esp + 0x74]
0041bac4  fstp       dword ptr [esp + 0x44]
0041bac8  fadd       dword ptr [esp + 0x78]
0041bacc  fstp       dword ptr [esp + 0x48]
0041bad0  call       0x40b5e0

// block 0041bad5
0041bad5  fld        dword ptr [esp + 0x14]
0041bad9  mov        dword ptr [ebx + 0x590], 0
0041bae3  fadd       qword ptr [0x4a4288]
0041bae9  inc        ebp
0041baea  fstp       dword ptr [esp + 0x14]
0041baee  fld        dword ptr [esp + 0x14]
0041baf2  fld        dword ptr [edi + 0x6c]
0041baf5  fcompp     
0041baf7  fnstsw     ax
0041baf9  test       ah, 0x41
0041bafc  je         0x41ba12

// block 0041bb02
0041bb02  mov        eax, dword ptr [edi]
0041bb04  mov        edx, dword ptr [eax + 0x14]
0041bb07  push       0
0041bb09  push       0
0041bb0b  mov        ecx, edi
0041bb0d  call       edx

// block 0041bb0f
0041bb0f  mov        ecx, dword ptr [0x4b44f4]
0041bb15  mov        eax, dword ptr [ecx + 0x48c]
0041bb1b  test       eax, eax
0041bb1d  je         0x41bb30

// block 0041bb1f
0041bb1f  mov        edi, dword ptr [eax + 8]
0041bb22  mov        dword ptr [ecx + 0x48c], edi
0041bb28  test       edi, edi
0041bb2a  jne        0x41b834

// block 0041bb30
0041bb30  pop        edi
0041bb31  xor        eax, eax
0041bb33  pop        esi
0041bb34  pop        ebp
0041bb35  pop        ebx
0041bb36  mov        esp, ebp
0041bb38  pop        ebp
0041bb39  ret        
