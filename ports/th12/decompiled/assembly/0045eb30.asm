
// block 0045eb30
0045eb30  push       ecx
0045eb31  mov        eax, dword ptr [0x4ce8cc]
0045eb36  fld        dword ptr [0x4a3dbc]
0045eb3c  fst        dword ptr [eax + 0x4b5678]
0045eb42  push       ebx
0045eb43  fst        dword ptr [eax + 0x4b5650]
0045eb49  push       esi
0045eb4a  fld        dword ptr [0x4a3db8]
0045eb50  lea        esi, [eax + 0x4b5650]
0045eb56  fst        dword ptr [eax + 0x4b568c]
0045eb5c  push       edi
0045eb5d  fst        dword ptr [eax + 0x4b5664]
0045eb63  push       0
0045eb65  fst        dword ptr [eax + 0x4b5690]
0045eb6b  lea        ebx, [eax + 0x4b564c]
0045eb71  fstp       dword ptr [eax + 0x4b567c]
0045eb77  push       ebx
0045eb78  push       1
0045eb7a  fst        dword ptr [eax + 0x4b5668]
0045eb80  push       0x102
0045eb85  fstp       dword ptr [eax + 0x4b5654]
0045eb8b  push       0
0045eb8d  fldz       
0045eb8f  push       0x50
0045eb91  fst        dword ptr [eax + 0x4b5694]
0045eb97  fst        dword ptr [eax + 0x4b5680]
0045eb9d  fst        dword ptr [eax + 0x4b566c]
0045eba3  fst        dword ptr [eax + 0x4b5658]
0045eba9  mov        ecx, dword ptr [esi]
0045ebab  fst        dword ptr [eax + 0x4b5684]
0045ebb1  fst        dword ptr [eax + 0x4b565c]
0045ebb7  fld1       
0045ebb9  fst        dword ptr [eax + 0x4b5698]
0045ebbf  fst        dword ptr [eax + 0x4b5670]
0045ebc5  fst        dword ptr [eax + 0x4b569c]
0045ebcb  fstp       dword ptr [eax + 0x4b5688]
0045ebd1  fst        dword ptr [eax + 0x4b5674]
0045ebd7  fstp       dword ptr [eax + 0x4b5660]
0045ebdd  mov        dword ptr [0x4d4858], ecx
0045ebe3  mov        edx, dword ptr [esi + 4]
0045ebe6  mov        dword ptr [0x4d485c], edx
0045ebec  mov        ecx, dword ptr [esi + 8]
0045ebef  mov        dword ptr [0x4d4860], ecx
0045ebf5  mov        edx, dword ptr [eax + 0x4b5664]
0045ebfb  mov        dword ptr [0x4d4870], edx
0045ec01  mov        ecx, dword ptr [eax + 0x4b5668]
0045ec07  mov        dword ptr [0x4d4874], ecx
0045ec0d  mov        edx, dword ptr [eax + 0x4b566c]
0045ec13  mov        dword ptr [0x4d4878], edx
0045ec19  mov        ecx, dword ptr [eax + 0x4b5678]
0045ec1f  mov        dword ptr [0x4d4888], ecx
0045ec25  mov        edx, dword ptr [eax + 0x4b567c]
0045ec2b  mov        dword ptr [0x4d488c], edx
0045ec31  mov        ecx, dword ptr [eax + 0x4b5680]
0045ec37  mov        dword ptr [0x4d4890], ecx
0045ec3d  mov        edx, dword ptr [eax + 0x4b568c]
0045ec43  mov        dword ptr [0x4d48a0], edx
0045ec49  mov        ecx, dword ptr [eax + 0x4b5690]
0045ec4f  mov        dword ptr [0x4d48a4], ecx
0045ec55  mov        edx, dword ptr [eax + 0x4b5694]
0045ec5b  mov        ecx, dword ptr [0x4ce8f0]
0045ec61  mov        dword ptr [0x4d48a8], edx
0045ec67  fld        dword ptr [eax + 0x4b565c]
0045ec6d  fstp       dword ptr [0x4d4868]
0045ec73  push       ecx
0045ec74  fld        dword ptr [eax + 0x4b5660]
0045ec7a  fstp       dword ptr [0x4d486c]
0045ec80  fld        dword ptr [eax + 0x4b5670]
0045ec86  fstp       dword ptr [0x4d4880]
0045ec8c  fld        dword ptr [eax + 0x4b5674]
0045ec92  fstp       dword ptr [0x4d4884]
0045ec98  fld        dword ptr [eax + 0x4b5684]
0045ec9e  fstp       dword ptr [0x4d4898]
0045eca4  fld        dword ptr [eax + 0x4b5688]
0045ecaa  fstp       dword ptr [0x4d489c]
0045ecb0  fld        dword ptr [eax + 0x4b5698]
0045ecb6  fstp       dword ptr [0x4d48b0]

// block 0045ecbc
0045ecbc  fld        dword ptr [eax + 0x4b569c]
0045ecc2  fstp       dword ptr [0x4d48b4]
0045ecc8  mov        eax, dword ptr [ecx]
0045ecca  mov        ecx, dword ptr [eax + 0x68]
0045eccd  call       ecx

// block 0045eccf
0045eccf  mov        eax, dword ptr [ebx]
0045ecd1  mov        edx, dword ptr [eax]
0045ecd3  mov        edx, dword ptr [edx + 0x2c]
0045ecd6  push       0
0045ecd8  lea        ecx, [esp + 0x10]
0045ecdc  push       ecx
0045ecdd  push       0
0045ecdf  push       0
0045ece1  push       eax
0045ece2  call       edx

// block 0045ece4
0045ece4  mov        edi, dword ptr [esp + 0xc]
0045ece8  mov        ecx, 0x14

// block 0045eced
0045eced  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045ecef
0045ecef  mov        eax, dword ptr [ebx]
0045ecf1  mov        ecx, dword ptr [eax]
0045ecf3  mov        edx, dword ptr [ecx + 0x30]
0045ecf6  push       eax
0045ecf7  call       edx

// block 0045ecf9
0045ecf9  mov        edx, dword ptr [ebx]
0045ecfb  mov        eax, dword ptr [0x4ce8f0]
0045ed00  mov        ecx, dword ptr [eax]
0045ed02  push       0x14
0045ed04  push       0
0045ed06  push       edx
0045ed07  push       0
