
// block 00448690
00448690  mov        ecx, dword ptr [esp + 4]
00448694  mov        eax, dword ptr [ecx + 0x24]
00448697  sub        esp, 0x10
0044869a  sub        eax, 2
0044869d  push       ebx
0044869e  push       ebp
0044869f  push       esi
004486a0  push       edi
004486a1  jne        0x4489d0

// block 004486a7
004486a7  fld        dword ptr [0x4a3f50]
004486ad  mov        ebp, dword ptr [0x4b0ca8]
004486b3  mov        esi, dword ptr [0x4b43b8]
004486b9  fstp       dword ptr [esp + 0x14]
004486bd  fld        dword ptr [0x4a3f4c]
004486c3  mov        eax, 0xff
004486c8  fstp       dword ptr [esp + 0x18]
004486cc  xor        edi, edi
004486ce  imul       ebp, ebp, 0x118
004486d4  fldz       
004486d6  fstp       dword ptr [esp + 0x1c]
004486da  mov        dword ptr [esi + 0x18f94], 1
004486e4  mov        dword ptr [esp + 0x10], eax
004486e8  jmp        0x448700

// block 004486f0
004486f0  mov        esi, dword ptr [0x4b43b8]
004486f6  fstp       st(0)
004486f8  mov        eax, dword ptr [esp + 0x10]
004486fc  mov        ecx, dword ptr [esp + 0x24]

// block 00448700
00448700  cmp        dword ptr [ecx + 0x5988], 0
00448707  je         0x448721

// block 00448709
00448709  mov        ecx, eax
0044870b  shl        ecx, 8
0044870e  or         ecx, eax
00448710  shl        ecx, 8
00448713  or         ecx, 0xff0000ff
00448719  mov        dword ptr [esi + 0x18f80], ecx
0044871f  jmp        0x448737

// block 00448721
00448721  mov        edx, dword ptr [ecx + 0x28]
00448724  sub        edx, edi
00448726  neg        edx
00448728  sbb        edx, edx
0044872a  and        edx, 0xff404041
00448730  dec        edx
00448731  mov        dword ptr [esi + 0x18f80], edx

// block 00448737
00448737  mov        ecx, dword ptr [0x4b0c90]
0044873d  mov        eax, dword ptr [0x4b0c94]
00448742  mov        edx, dword ptr [0x4b451c]
00448748  lea        eax, [eax + ecx*2]
0044874b  imul       eax, eax, 0x45f4
00448751  lea        ecx, [eax + ebp]
00448754  mov        ebx, dword ptr [ecx + edx + 0x28]
00448758  add        ecx, edx
0044875a  or         ebx, dword ptr [ecx + 0x2c]
0044875d  je         0x4487e5

// block 00448763
00448763  lea        eax, [eax + edx + 8]
00448767  lea        edx, [eax + ebp + 0x20]
0044876b  push       edx
0044876c  call       0x46d8e4

// block 00448771
00448771  mov        edx, dword ptr [0x4b0c90]
00448777  mov        ecx, dword ptr [0x4b0c94]
0044877d  lea        ecx, [ecx + edx*2]
00448780  mov        edx, dword ptr [0x4b451c]
00448786  imul       ecx, ecx, 0x45f4
0044878c  add        ecx, ebp
0044878e  add        ecx, edx
00448790  movsx      edx, byte ptr [ecx + 0x1c]
00448794  fld        dword ptr [ecx + 0x30]
00448797  mov        edx, dword ptr [edx*4 + 0x4aee60]
0044879e  push       ecx
0044879f  mov        esi, dword ptr [0x4b43b8]
004487a5  add        ecx, 0x1e
004487a8  fstp       qword ptr [esp]
004487ab  push       edx
004487ac  mov        edx, dword ptr [eax + 4]
004487af  push       edx
004487b0  mov        edx, dword ptr [eax + 8]
004487b3  push       edx
004487b4  mov        edx, dword ptr [eax + 0xc]
004487b7  push       edx
004487b8  mov        edx, dword ptr [eax + 0x10]
004487bb  mov        eax, dword ptr [eax + 0x14]
004487be  inc        edx
004487bf  push       edx
004487c0  movsx      edx, byte ptr [ecx - 1]
004487c4  add        eax, 0x76c
004487c9  push       eax
004487ca  mov        eax, dword ptr [ecx - 6]
004487cd  push       edx
004487ce  push       eax
004487cf  push       ecx
004487d0  inc        edi
004487d1  push       edi
004487d2  push       0x4a2014
004487d7  lea        ebx, [esp + 0x48]
004487db  call       0x4015c0

// block 004487e0
004487e0  add        esp, 0x34
004487e3  jmp        0x44880e

// block 004487e5
004487e5  movsx      edx, byte ptr [ecx + 0x1d]
004487e9  fld        dword ptr [ecx + 0x30]
004487ec  mov        eax, dword ptr [ecx + 0x18]
004487ef  sub        esp, 8
004487f2  add        ecx, 0x1e
004487f5  inc        edi
004487f6  fstp       qword ptr [esp]
004487f9  push       edx
004487fa  push       eax
004487fb  push       ecx
004487fc  push       edi
004487fd  push       0x4a204c
00448802  lea        ebx, [esp + 0x30]
00448806  call       0x4015c0

// block 0044880b
0044880b  add        esp, 0x1c

// block 0044880e
0044880e  fld        dword ptr [esp + 0x18]
00448812  mov        eax, dword ptr [esp + 0x10]
00448816  fld        qword ptr [0x4a3f08]
0044881c  sub        eax, 0x10
0044881f  fadd       st(1), st(0)
00448821  add        ebp, 0x1c
00448824  cmp        eax, 0x5f
00448827  fxch       st(1)
00448829  mov        dword ptr [esp + 0x10], eax
0044882d  fstp       dword ptr [esp + 0x18]
00448831  jg         0x4486f0

// block 00448837
00448837  mov        ebp, dword ptr [esp + 0x24]
0044883b  cmp        dword ptr [ebp + 0x5988], 0
00448842  jne        0x4489ce

// block 00448848
00448848  fld        dword ptr [0x4a3f48]
0044884e  mov        esi, dword ptr [0x4b43b8]
00448854  fstp       dword ptr [esp + 0x14]
00448858  lea        ecx, [ebp + 0x5978]
0044885e  push       ecx
0044885f  fimul      dword ptr [ebp + 0x28]
00448862  or         edi, 0xffffffff
00448865  push       0x4a0f40
0044886a  lea        ebx, [esp + 0x1c]
0044886e  mov        dword ptr [esi + 0x18f80], edi
00448874  fadd       qword ptr [0x4a3f40]
0044887a  fstp       dword ptr [esp + 0x20]
0044887e  fldz       
00448880  fstp       dword ptr [esp + 0x24]
00448884  call       0x4015c0

// block 00448889
00448889  mov        eax, dword ptr [ebp + 0x5984]
0044888f  lea        edx, [eax + eax*8]
00448892  mov        dword ptr [esp + 0x2c], edx
00448896  fild       dword ptr [esp + 0x2c]
0044889a  add        esp, 8
0044889d  fadd       qword ptr [0x4a3f38]
004488a3  fstp       dword ptr [esp + 0x14]
004488a7  cmp        eax, 8
004488aa  jne        0x4488ba

// block 004488ac
004488ac  fld        dword ptr [esp + 0x14]
004488b0  fsub       qword ptr [0x4a3f18]
004488b6  fstp       dword ptr [esp + 0x14]

// block 004488ba
004488ba  mov        esi, dword ptr [0x4b43b8]
004488c0  push       0x4a0f44
004488c5  lea        ebx, [esp + 0x18]
004488c9  mov        dword ptr [esi + 0x18f80], 0xffffff00
004488d3  call       0x4015c0

// block 004488d8
004488d8  fld        dword ptr [0x4a3f14]
004488de  mov        esi, dword ptr [0x4b43b8]
004488e4  fstp       dword ptr [esp + 0x18]
004488e8  fld        dword ptr [0x4a3f10]
004488ee  add        esp, 4
004488f1  fstp       dword ptr [esp + 0x18]
004488f5  mov        dword ptr [esi + 0x18f80], edi
004488fb  fldz       
004488fd  xor        edi, edi
004488ff  fstp       dword ptr [esp + 0x1c]
00448903  jmp        0x448916

// block 00448910
00448910  mov        esi, dword ptr [0x4b43b8]

// block 00448916
00448916  mov        eax, dword ptr [ebp + 0x5990]
0044891c  sub        eax, edi
0044891e  neg        eax
00448920  sbb        eax, eax
00448922  and        eax, 0xff808180
00448927  add        eax, 0xffffff00
0044892c  cmp        edi, 0x58
0044892f  mov        dword ptr [esi + 0x18f80], eax
00448935  jge        0x448941

// block 00448937
00448937  movsx      ecx, byte ptr [edi + 0x4a0e88]
0044893e  push       ecx
0044893f  jmp        0x448956

// block 00448941
00448941  jne        0x44894a

// block 00448943
00448943  mov        eax, 0x81
00448948  jmp        0x448955

// block 0044894a
0044894a  xor        eax, eax
0044894c  cmp        edi, 0x59
0044894f  setne      al
00448952  add        eax, 0x7f

// block 00448955
00448955  push       eax

// block 00448956
00448956  push       0x4a0f48
0044895b  lea        ebx, [esp + 0x1c]
0044895f  call       0x4015c0

// block 00448964
00448964  mov        eax, 0x4ec4ec4f
00448969  mul        edi
0044896b  shr        edx, 2
0044896e  imul       edx, edx, 0xd
00448971  mov        eax, edi
00448973  sub        eax, edx
00448975  add        esp, 8
00448978  cmp        eax, 0xc
0044897b  jne        0x448997

// block 0044897d
0044897d  fld        dword ptr [0x4a3f14]
00448983  fstp       dword ptr [esp + 0x14]
00448987  fld        dword ptr [esp + 0x18]
0044898b  fadd       qword ptr [0x4a3d68]
00448991  fstp       dword ptr [esp + 0x18]
00448995  jmp        0x4489a5

// block 00448997
00448997  fld        dword ptr [esp + 0x14]
0044899b  fadd       qword ptr [0x4a3f08]
004489a1  fstp       dword ptr [esp + 0x14]

// block 004489a5
004489a5  inc        edi
004489a6  cmp        edi, 0x5b
004489a9  jl         0x448910

// block 004489af
004489af  mov        ecx, dword ptr [0x4b43b8]
004489b5  mov        dword ptr [ecx + 0x18f80], 0xffffffff
004489bf  mov        eax, 1
004489c4  pop        edi
004489c5  pop        esi
004489c6  pop        ebp
004489c7  pop        ebx
004489c8  add        esp, 0x10
004489cb  ret        4

// block 004489ce
004489ce  fstp       st(0)

// block 004489d0
004489d0  pop        edi
004489d1  pop        esi
004489d2  pop        ebp
004489d3  mov        eax, 1
004489d8  pop        ebx
004489d9  add        esp, 0x10
004489dc  ret        4
