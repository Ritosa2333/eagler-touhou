
// block 00448fb0
00448fb0  mov        eax, dword ptr [esp + 4]
00448fb4  mov        ecx, dword ptr [eax + 0x24]
00448fb7  sub        esp, 0x10
00448fba  sub        ecx, 2
00448fbd  push       ebx
00448fbe  push       ebp
00448fbf  push       esi
00448fc0  push       edi
00448fc1  je         0x449225

// block 00448fc7
00448fc7  sub        ecx, 1
00448fca  jne        0x449345

// block 00448fd0
00448fd0  cmp        dword ptr [eax + 0x2b8], 0xa
00448fd7  fld        dword ptr [0x4a3f30]
00448fdd  mov        edi, dword ptr [eax + 0x5a78]
00448fe3  fstp       dword ptr [esp + 0x14]
00448fe7  fld        dword ptr [0x4a3f2c]
00448fed  fstp       dword ptr [esp + 0x18]
00448ff1  fldz       
00448ff3  fstp       dword ptr [esp + 0x1c]
00448ff7  jge        0x44902f

// block 00448ff9
00448ff9  fld        dword ptr [eax + 0x2bc]
00448fff  mov        eax, edi
00449001  fld        qword ptr [0x4a3e40]
00449007  shl        eax, 4
0044900a  fld        st(0)
0044900c  sub        eax, edi
0044900e  fsubrp     st(2)
00449010  add        eax, 0x50
00449013  mov        dword ptr [esp + 0x10], eax
00449017  fild       dword ptr [esp + 0x10]
0044901b  fld        qword ptr [0x4a3ee0]
00449021  fsub       st(1), st(0)
00449023  fxch       st(3)
00449025  fmulp      st(1)
00449027  fdivrp     st(1)
00449029  faddp      st(1)
0044902b  fstp       dword ptr [esp + 0x18]

// block 0044902f
0044902f  mov        ecx, dword ptr [0x4b4518]
00449035  mov        esi, dword ptr [ecx + 0x1c]
00449038  lea        edx, [esi + 0xc]
0044903b  push       edx
0044903c  call       0x46d8e4

// block 00449041
00449041  fld        dword ptr [esi + 0x54]
00449044  mov        edx, dword ptr [esi + 0x64]
00449047  push       ecx
00449048  mov        ecx, dword ptr [0x4aeea8]
0044904e  fstp       qword ptr [esp]
00449051  push       ecx
00449052  mov        ecx, dword ptr [edx*4 + 0x4aee38]
00449059  mov        edx, dword ptr [esi + 0x5c]
0044905c  push       ecx
0044905d  mov        ecx, dword ptr [esi + 0x60]
00449060  lea        edx, [ecx + edx*2]
00449063  mov        ecx, dword ptr [edx*4 + 0x4aee20]
0044906a  mov        edx, dword ptr [eax + 4]
0044906d  push       ecx
0044906e  mov        ecx, dword ptr [eax + 8]
00449071  push       edx
00449072  mov        edx, dword ptr [eax + 0xc]
00449075  push       ecx
00449076  mov        ecx, dword ptr [eax + 0x10]
00449079  mov        eax, dword ptr [eax + 0x14]
0044907c  push       edx
0044907d  inc        ecx
0044907e  push       ecx
0044907f  cdq        
00449080  mov        ecx, 0x64
00449085  idiv       ecx
00449087  mov        esi, dword ptr [0x4b43b8]
0044908d  inc        edi
0044908e  lea        ebx, [esp + 0x38]
00449092  push       edx
00449093  push       0x4a0ee4
00449098  push       edi
00449099  push       0x4a1e78
0044909e  call       0x4015c0

// block 004490a3
004490a3  mov        ebp, dword ptr [esp + 0x58]
004490a7  add        esp, 0x34
004490aa  cmp        dword ptr [ebp + 0x2b8], 0xa
004490b1  jl         0x44932c

// block 004490b7
004490b7  fld        dword ptr [0x4a3f28]
004490bd  mov        esi, dword ptr [0x4b43b8]
004490c3  fstp       dword ptr [esp + 0x14]
004490c7  lea        edx, [ebp + 0x5978]
004490cd  fld        dword ptr [0x4a3f2c]
004490d3  push       edx
004490d4  fstp       dword ptr [esp + 0x1c]
004490d8  or         edi, 0xffffffff
004490db  fldz       
004490dd  push       0x4a0f40
004490e2  lea        ebx, [esp + 0x1c]
004490e6  fstp       dword ptr [esp + 0x24]
004490ea  mov        dword ptr [esi + 0x18f80], edi
004490f0  call       0x4015c0

// block 004490f5
004490f5  mov        eax, dword ptr [ebp + 0x5984]
004490fb  lea        ecx, [eax + eax*8]
004490fe  mov        dword ptr [esp + 0x2c], ecx
00449102  fild       dword ptr [esp + 0x2c]
00449106  add        esp, 8
00449109  fadd       qword ptr [0x4a3f20]
0044910f  fstp       dword ptr [esp + 0x14]
00449113  cmp        eax, 8
00449116  jne        0x449126

// block 00449118
00449118  fld        dword ptr [esp + 0x14]
0044911c  fsub       qword ptr [0x4a3f18]
00449122  fstp       dword ptr [esp + 0x14]

// block 00449126
00449126  mov        esi, dword ptr [0x4b43b8]
0044912c  push       0x4a0f44
00449131  lea        ebx, [esp + 0x18]
00449135  mov        dword ptr [esi + 0x18f80], 0xffffff00
0044913f  call       0x4015c0

// block 00449144
00449144  fld        dword ptr [0x4a3f14]
0044914a  mov        esi, dword ptr [0x4b43b8]
00449150  fstp       dword ptr [esp + 0x18]
00449154  fld        dword ptr [0x4a3f10]
0044915a  add        esp, 4
0044915d  fstp       dword ptr [esp + 0x18]
00449161  mov        dword ptr [esi + 0x18f80], edi
00449167  fldz       
00449169  xor        edi, edi
0044916b  fstp       dword ptr [esp + 0x1c]
0044916f  jmp        0x449177

// block 00449171
00449171  mov        esi, dword ptr [0x4b43b8]

// block 00449177
00449177  mov        edx, dword ptr [ebp + 0x5990]
0044917d  sub        edx, edi
0044917f  neg        edx
00449181  sbb        edx, edx
00449183  and        edx, 0xff808180
00449189  add        edx, 0xffffff00
0044918f  cmp        edi, 0x58
00449192  mov        dword ptr [esi + 0x18f80], edx
00449198  jge        0x4491a3

// block 0044919a
0044919a  movsx      eax, byte ptr [edi + 0x4a0e88]
004491a1  jmp        0x4491b7

// block 004491a3
004491a3  jne        0x4491ac

// block 004491a5
004491a5  mov        eax, 0x81
004491aa  jmp        0x4491b7

// block 004491ac
004491ac  xor        eax, eax
004491ae  cmp        edi, 0x59
004491b1  setne      al
004491b4  add        eax, 0x7f

// block 004491b7
004491b7  push       eax
004491b8  push       0x4a0f48
004491bd  lea        ebx, [esp + 0x1c]
004491c1  call       0x4015c0

// block 004491c6
004491c6  mov        eax, 0x4ec4ec4f
004491cb  mul        edi
004491cd  shr        edx, 2
004491d0  imul       edx, edx, 0xd
004491d3  mov        ecx, edi
004491d5  sub        ecx, edx
004491d7  add        esp, 8
004491da  cmp        ecx, 0xc
004491dd  jne        0x4491f9

// block 004491df
004491df  fld        dword ptr [0x4a3f14]
004491e5  fstp       dword ptr [esp + 0x14]
004491e9  fld        dword ptr [esp + 0x18]
004491ed  fadd       qword ptr [0x4a3d68]
004491f3  fstp       dword ptr [esp + 0x18]
004491f7  jmp        0x449207

// block 004491f9
004491f9  fld        dword ptr [esp + 0x14]
004491fd  fadd       qword ptr [0x4a3f08]
00449203  fstp       dword ptr [esp + 0x14]

// block 00449207
00449207  inc        edi
00449208  cmp        edi, 0x5b
0044920b  jl         0x449171

// block 00449211
00449211  mov        eax, dword ptr [0x4b43b8]
00449216  mov        dword ptr [eax + 0x18f80], 0xffffffff
00449220  jmp        0x449331

// block 00449225
00449225  fld        dword ptr [0x4a3f30]
0044922b  mov        esi, dword ptr [0x4b43b8]
00449231  fstp       dword ptr [esp + 0x14]
00449235  mov        dword ptr [esi + 0x18f94], 1
0044923f  fld        dword ptr [0x4a3f00]
00449245  xor        edi, edi
00449247  fstp       dword ptr [esp + 0x18]
0044924b  lea        ebp, [eax + 0x5a80]
00449251  fldz       
00449253  fstp       dword ptr [esp + 0x1c]
00449257  jmp        0x44926a

// block 00449260
00449260  mov        esi, dword ptr [0x4b43b8]
00449266  mov        eax, dword ptr [esp + 0x24]

// block 0044926a
0044926a  mov        edx, dword ptr [eax + 0x28]
0044926d  sub        edx, edi
0044926f  neg        edx
00449271  sbb        edx, edx
00449273  and        edx, 0xff808180
00449279  add        edx, 0xffffff00
0044927f  mov        dword ptr [esi + 0x18f80], edx
00449285  mov        eax, dword ptr [ebp]
00449288  test       eax, eax
0044928a  je         0x4492ff

// block 0044928c
0044928c  mov        esi, dword ptr [eax + 0x1c]
0044928f  lea        eax, [esi + 0xc]
00449292  push       eax
00449293  call       0x46d8e4

// block 00449298
00449298  fld        dword ptr [esi + 0x54]
0044929b  push       ecx
0044929c  mov        ecx, dword ptr [esi + 0x68]
0044929f  fstp       qword ptr [esp]
004492a2  mov        edx, dword ptr [ecx*4 + 0x4aee88]
004492a9  mov        ecx, dword ptr [esi + 0x64]
004492ac  push       edx
004492ad  mov        edx, dword ptr [ecx*4 + 0x4aee38]
004492b4  mov        ecx, dword ptr [esi + 0x5c]
004492b7  push       edx
004492b8  mov        edx, dword ptr [esi + 0x60]
004492bb  lea        ecx, [edx + ecx*2]
004492be  mov        edx, dword ptr [ecx*4 + 0x4aee20]
004492c5  mov        ecx, dword ptr [eax + 4]
004492c8  push       edx
004492c9  mov        edx, dword ptr [eax + 8]
004492cc  push       ecx
004492cd  mov        ecx, dword ptr [eax + 0xc]
004492d0  push       edx
004492d1  mov        edx, dword ptr [eax + 0x10]
004492d4  mov        eax, dword ptr [eax + 0x14]
004492d7  push       ecx
004492d8  inc        edx
004492d9  push       edx
004492da  cdq        
004492db  mov        ecx, 0x64
004492e0  idiv       ecx
004492e2  inc        edi
004492e3  lea        ebx, [esp + 0x38]
004492e7  push       edx
004492e8  push       esi
004492e9  mov        esi, dword ptr [0x4b43b8]
004492ef  push       edi
004492f0  push       0x4a1e78
004492f5  call       0x4015c0

// block 004492fa
004492fa  add        esp, 0x34
004492fd  jmp        0x449312

// block 004492ff
004492ff  inc        edi
00449300  push       edi
00449301  push       0x4a1e3c
00449306  lea        ebx, [esp + 0x1c]
0044930a  call       0x4015c0

// block 0044930f
0044930f  add        esp, 8

// block 00449312
00449312  fld        dword ptr [esp + 0x18]
00449316  add        ebp, 4
00449319  cmp        edi, 0x19
0044931c  fadd       qword ptr [0x4a3ef8]
00449322  fstp       dword ptr [esp + 0x18]
00449326  jl         0x449260

// block 0044932c
0044932c  mov        eax, dword ptr [0x4b43b8]

// block 00449331
00449331  mov        dword ptr [eax + 0x18f94], 0
0044933b  mov        dword ptr [eax + 0x18f80], 0xffffffff

// block 00449345
00449345  pop        edi
00449346  pop        esi
00449347  pop        ebp
00449348  mov        eax, 1
0044934d  pop        ebx
0044934e  add        esp, 0x10
00449351  ret        4
