
// block 00439630
00439630  sub        esp, 0x10
00439633  push       ebp
00439634  mov        ebp, dword ptr [esp + 0x18]
00439638  cmp        byte ptr [ebp + 0x1d], 2
0043963c  push       esi
0043963d  push       edi
0043963e  lea        edi, [ebx + 0xa58]
00439644  jne        0x439658

// block 00439646
00439646  movsx      eax, byte ptr [ebp + 0x1c]
0043964a  cmp        dword ptr [ebx + eax*4 + 0xc430], 0
00439652  jne        0x4399c5

// block 00439658
00439658  xor        esi, esi
0043965a  xor        eax, eax
0043965c  lea        esp, [esp]

// block 00439660
00439660  cmp        dword ptr [edi + 0x48], esi
00439663  je         0x43967b

// block 00439665
00439665  inc        eax
00439666  add        edi, 0x78
00439669  cmp        eax, 0x100
0043966e  jl         0x439660

// block 00439670
00439670  xor        eax, eax
00439672  pop        edi
00439673  pop        esi
00439674  pop        ebp
00439675  add        esp, 0x10
00439678  ret        8

// block 0043967b
0043967b  mov        eax, dword ptr [edi + 0x10]
0043967e  fldz       
00439680  mov        edx, 1
00439685  mov        dword ptr [edi + 0x48], edx
00439688  mov        dword ptr [edi + 0x74], ebp
0043968b  test       dl, al
0043968d  jne        0x4396a7

// block 0043968f
0043968f  or         eax, edx
00439691  fst        dword ptr [edi + 8]
00439694  mov        dword ptr [edi + 4], esi
00439697  mov        dword ptr [edi], 0xfff0bdc1
0043969d  mov        dword ptr [edi + 0xc], 0x4b2ed0
004396a4  mov        dword ptr [edi + 0x10], eax

// block 004396a7
004396a7  fst        dword ptr [edi + 8]
004396aa  mov        dword ptr [edi + 4], esi
004396ad  mov        dword ptr [edi], 0xffffffff
004396b3  movsx      eax, word ptr [ebp + 2]
004396b7  mov        dword ptr [edi + 0x60], eax
004396ba  fld        dword ptr [ebp + 0xc]
004396bd  fstp       dword ptr [edi + 0x68]
004396c0  fld        dword ptr [ebp + 0x10]
004396c3  fstp       dword ptr [edi + 0x6c]
004396c6  mov        al, byte ptr [ebp + 0x1c]
004396c9  test       al, al
004396cb  jne        0x4396df

// block 004396cd
004396cd  mov        eax, dword ptr [ecx]
004396cf  fstp       st(0)
004396d1  mov        dword ptr [edi + 0x14], eax
004396d4  mov        eax, dword ptr [ecx + 4]
004396d7  mov        dword ptr [edi + 0x18], eax
004396da  mov        ecx, dword ptr [ecx + 8]
004396dd  jmp        0x439725

// block 004396df
004396df  movsx      eax, al
004396e2  imul       eax, eax, 0xe4
004396e8  fild       dword ptr [eax + ebx + 0x81d8]
004396ef  fld        qword ptr [0x4a3d40]
004396f5  fmul       st(1), st(0)
004396f7  fxch       st(1)
004396f9  fstp       dword ptr [esp + 0x10]
004396fd  mov        ecx, dword ptr [esp + 0x10]
00439701  fimul      dword ptr [eax + ebx + 0x81dc]
00439708  lea        eax, [eax + ebx + 0x81d8]
0043970f  mov        dword ptr [edi + 0x14], ecx
00439712  fstp       dword ptr [esp + 0x14]
00439716  mov        eax, dword ptr [esp + 0x14]
0043971a  mov        dword ptr [edi + 0x18], eax
0043971d  fstp       dword ptr [esp + 0x18]
00439721  mov        ecx, dword ptr [esp + 0x18]

// block 00439725
00439725  mov        dword ptr [edi + 0x1c], ecx
00439728  cmp        byte ptr [ebp + 0x1d], 2
0043972c  jne        0x439739

// block 0043972e
0043972e  movsx      eax, byte ptr [ebp + 0x1c]
00439732  mov        dword ptr [ebx + eax*4 + 0xc430], edx

// block 00439739
00439739  fld        dword ptr [ebp + 0x18]
0043973c  fstp       dword ptr [edi + 0x2c]
0043973f  fld        dword ptr [ebp + 0x14]
00439742  fcomp      qword ptr [0x4a3d30]
00439748  fnstsw     ax
0043974a  test       ah, 1
0043974d  jne        0x439803

// block 00439753
00439753  cmp        byte ptr [ebp + 0x1c], 0
00439757  je         0x43982d

// block 0043975d
0043975d  mov        esi, 0x4ce568
00439762  call       0x464440

// block 00439767
00439767  mov        dword ptr [esp + 0x20], eax
0043976b  fild       dword ptr [esp + 0x20]
0043976f  test       eax, eax
00439771  jge        0x439779

// block 00439773
00439773  fadd       dword ptr [0x4a3e10]

// block 00439779
00439779  fmul       qword ptr [0x4a3ea8]
0043977f  movsx      ecx, byte ptr [ebp + 0x1c]
00439783  imul       ecx, ecx, 0xe4
00439789  fsub       qword ptr [0x4a3ce0]
0043978f  fstp       dword ptr [esp + 0x20]
00439793  push       ecx
00439794  fld        dword ptr [ecx + ebx + 0x8224]
0043979b  fld        dword ptr [esp + 0x24]
0043979f  fmul       qword ptr [0x4a3cd8]
004397a5  fdiv       qword ptr [0x4a4128]
004397ab  faddp      st(1)
004397ad  fstp       dword ptr [esp + 0x24]
004397b1  fld        dword ptr [esp + 0x24]
004397b5  fstp       dword ptr [esp]
004397b8  call       0x4646e0

// block 004397bd
004397bd  push       ecx
004397be  fstp       dword ptr [esp]
004397c1  call       0x4646e0

// block 004397c6
004397c6  mov        esi, 0x4ce568
004397cb  fstp       dword ptr [edi + 0x30]
004397ce  call       0x464440

// block 004397d3
004397d3  mov        dword ptr [esp + 0x20], eax
004397d7  fild       dword ptr [esp + 0x20]
004397db  test       eax, eax
004397dd  jge        0x4397e5

// block 004397df
004397df  fadd       dword ptr [0x4a3e10]

// block 004397e5
004397e5  fmul       qword ptr [0x4a3ea8]
004397eb  fsub       qword ptr [0x4a3ce0]
004397f1  fstp       dword ptr [esp + 0x20]
004397f5  fld        dword ptr [esp + 0x20]
004397f9  fadd       st(0), st(0)
004397fb  fadd       dword ptr [ebp + 0x18]
004397fe  fstp       dword ptr [edi + 0x2c]
00439801  jmp        0x439845

// block 00439803
00439803  fld        dword ptr [ebp + 0x14]
00439806  fcomp      qword ptr [0x4a4248]
0043980c  fnstsw     ax
0043980e  test       ah, 1
00439811  jne        0x43982d

// block 00439813
00439813  mov        al, byte ptr [ebp + 0x1c]
00439816  push       ecx
00439817  test       al, al
00439819  je         0x43982e

// block 0043981b
0043981b  movsx      edx, al
0043981e  imul       edx, edx, 0xe4
00439824  fld        dword ptr [edx + ebx + 0x8224]
0043982b  jmp        0x439831

// block 0043982d
0043982d  push       ecx

// block 0043982e
0043982e  fld        dword ptr [ebp + 0x14]

// block 00439831
00439831  fstp       dword ptr [esp]
00439834  call       0x4646e0

// block 00439839
00439839  push       ecx
0043983a  fstp       dword ptr [esp]
0043983d  call       0x4646e0

// block 00439842
00439842  fstp       dword ptr [edi + 0x30]

// block 00439845
00439845  test       byte ptr [edi + 0x44], 1
00439849  jne        0x43986a

// block 0043984b
0043984b  fld        dword ptr [edi + 0x2c]
0043984e  sub        esp, 8
00439851  fstp       dword ptr [esp + 4]
00439855  lea        ecx, [edi + 0x20]
00439858  fld        dword ptr [edi + 0x30]
0043985b  fstp       dword ptr [esp]
0043985e  call       0x465390

// block 00439863
00439863  fldz       
00439865  fstp       dword ptr [edi + 0x28]
00439868  jmp        0x439896

// block 0043986a
0043986a  fld        dword ptr [edi + 0x38]
0043986d  push       ecx
0043986e  fadd       dword ptr [edi + 0x34]
00439871  fstp       dword ptr [edi + 0x34]
00439874  fld        dword ptr [edi + 0x2c]
00439877  fadd       dword ptr [edi + 0x30]
0043987a  fstp       dword ptr [esp + 0x24]
0043987e  fld        dword ptr [esp + 0x24]
00439882  fstp       dword ptr [esp]
00439885  call       0x4646e0

// block 0043988a
0043988a  push       ecx
0043988b  fstp       dword ptr [esp]
0043988e  call       0x4646e0

// block 00439893
00439893  fstp       dword ptr [edi + 0x30]

// block 00439896
00439896  fld        dword ptr [ebp + 4]
00439899  mov        edx, dword ptr [ebx + 0x10]
0043989c  fsub       dword ptr [edi + 0x20]
0043989f  push       0
004398a1  lea        ecx, [esp + 0x24]
004398a5  fadd       dword ptr [edi + 0x14]
004398a8  fstp       dword ptr [edi + 0x14]
004398ab  fld        dword ptr [ebp + 8]
004398ae  fsub       dword ptr [edi + 0x24]
004398b1  fadd       dword ptr [edi + 0x18]
004398b4  fstp       dword ptr [edi + 0x18]
004398b7  movsx      eax, word ptr [ebp + 0x1e]
004398bb  add        eax, 5
004398be  push       eax
004398bf  push       ecx
004398c0  push       edx
004398c1  mov        eax, 0x17
004398c6  xor        ecx, ecx
004398c8  call       0x4615a0

// block 004398cd
004398cd  mov        eax, dword ptr [esp + 0x20]
004398d1  mov        ecx, eax
004398d3  mov        dword ptr [edi + 0x4c], eax
004398d6  test       ecx, ecx
004398d8  jne        0x4398de

// block 004398da
004398da  xor        esi, esi
004398dc  jmp        0x43991e

// block 004398de
004398de  mov        edx, dword ptr [0x4ce8cc]
004398e4  mov        eax, dword ptr [edx + 0x8856b8]
004398ea  test       eax, eax
004398ec  je         0x4398fd

// block 004398ee
004398ee  mov        edi, edi

// block 004398f0
004398f0  mov        esi, dword ptr [eax]
004398f2  cmp        dword ptr [esi], ecx
004398f4  je         0x43991a

// block 004398f6
004398f6  mov        eax, dword ptr [eax + 4]
004398f9  test       eax, eax
004398fb  jne        0x4398f0

// block 004398fd
004398fd  mov        eax, dword ptr [edx + 0x8856c0]
00439903  test       eax, eax
00439905  je         0x4398da

// block 00439907
00439907  mov        edx, dword ptr [eax]
00439909  cmp        dword ptr [edx], ecx
0043990b  je         0x439918

// block 0043990d
0043990d  mov        eax, dword ptr [eax + 4]
00439910  test       eax, eax
00439912  jne        0x439907

// block 00439914
00439914  xor        esi, esi
00439916  jmp        0x43991e

// block 00439918
00439918  mov        esi, edx

// block 0043991a
0043991a  test       esi, esi
0043991c  jne        0x439925

// block 0043991e
0043991e  mov        dword ptr [edi + 0x4c], 0

// block 00439925
00439925  mov        eax, dword ptr [esi + 0x47c]
0043992b  test       eax, 0x20000000
00439930  je         0x439941

// block 00439932
00439932  fld        dword ptr [ebp + 0x14]
00439935  or         eax, 4
00439938  fstp       dword ptr [esi + 0x2c]
0043993b  mov        dword ptr [esi + 0x47c], eax

// block 00439941
00439941  cmp        byte ptr [ebp + 0x1d], 2
00439945  jne        0x439969

// block 00439947
00439947  mov        ecx, dword ptr [ebx + 0x10]
0043994a  push       0
0043994c  push       8
0043994e  lea        eax, [esp + 0x28]
00439952  push       eax
00439953  push       ecx
00439954  mov        eax, 0x17
00439959  xor        ecx, ecx
0043995b  call       0x4615a0

// block 00439960
00439960  mov        edx, dword ptr [esp + 0x20]
00439964  mov        dword ptr [edi + 0x50], edx
00439967  jmp        0x439970

// block 00439969
00439969  mov        dword ptr [edi + 0x50], 0

// block 00439970
00439970  mov        eax, dword ptr [ebp + 0x24]
00439973  test       eax, eax
00439975  je         0x439982

// block 00439977
00439977  mov        ecx, dword ptr [esp + 0x24]
0043997b  push       ecx
0043997c  mov        edx, edi
0043997e  mov        ecx, ebx
00439980  call       eax

// block 00439982
00439982  movzx      eax, word ptr [ebp + 0x22]
00439986  test       ax, ax
00439989  jl         0x439998

// block 0043998b
0043998b  fld        dword ptr [edi + 0x14]
0043998e  push       ecx
0043998f  cwde       
00439990  fstp       dword ptr [esp]
00439993  call       0x453e20

// block 00439998
00439998  fld        dword ptr [edi + 0x14]
0043999b  fadd       qword ptr [0x4a3d70]
004399a1  fadd       qword ptr [0x4a3d28]
004399a7  fstp       dword ptr [esi + 0x430]
004399ad  fld        dword ptr [edi + 0x18]
004399b0  fadd       qword ptr [0x4a3d68]
004399b6  fstp       dword ptr [esi + 0x434]
004399bc  fld        dword ptr [edi + 0x1c]
004399bf  fstp       dword ptr [esi + 0x438]

// block 004399c5
004399c5  pop        edi
004399c6  pop        esi
004399c7  xor        eax, eax
004399c9  pop        ebp
004399ca  add        esp, 0x10
004399cd  ret        8
