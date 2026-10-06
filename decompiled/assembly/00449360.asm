
// block 00449360
00449360  push       ebp
00449361  mov        ebp, esp
00449363  and        esp, 0xfffffff8
00449366  sub        esp, 0x24
00449369  push       ebx
0044936a  mov        ebx, dword ptr [ebp + 8]
0044936d  mov        eax, dword ptr [ebx + 0x24]
00449370  push       esi
00449371  push       edi
00449372  cmp        eax, 3
00449375  ja         0x449d5f

// block 0044937b
0044937b  jmp        dword ptr [eax*4 + 0x449d6c]

// block 00449382
00449382  cmp        dword ptr [ebx + 0x2b8], 1
00449389  jne        0x449521

// block 0044938f
0044938f  lea        ecx, [ebx + 0x28]
00449392  mov        dword ptr [ebx + 0x30], 6
00449399  mov        eax, dword ptr [ecx + 8]
0044939c  mov        dword ptr [esp + 0x10], ecx
004493a0  test       eax, eax
004493a2  je         0x4493b1

// block 004493a4
004493a4  jg         0x4493ab

// block 004493a6
004493a6  dec        eax
004493a7  mov        dword ptr [ecx], eax
004493a9  jmp        0x4493b7

// block 004493ab
004493ab  xor        eax, eax
004493ad  mov        dword ptr [ecx], eax
004493af  jmp        0x4493b7

// block 004493b1
004493b1  mov        dword ptr [ecx], 0

// block 004493b7
004493b7  cmp        dword ptr [ebx + 0x66c], 0
004493be  jne        0x4493ec

// block 004493c0
004493c0  mov        ecx, dword ptr [0x4b43b8]
004493c6  mov        edx, dword ptr [ecx + 0x18fb4]
004493cc  push       0
004493ce  push       0x14
004493d0  lea        eax, [esp + 0x28]
004493d4  push       eax
004493d5  push       edx
004493d6  mov        eax, 0x17
004493db  xor        ecx, ecx
004493dd  call       0x4615a0

// block 004493e2
004493e2  mov        eax, dword ptr [esp + 0x20]
004493e6  mov        dword ptr [ebx + 0x66c], eax

// block 004493ec
004493ec  mov        esi, 0x73
004493f1  mov        edi, ebx
004493f3  call       0x43efa0

// block 004493f8
004493f8  push       0
004493fa  lea        ecx, [esp + 0x10]
004493fe  push       ecx
004493ff  mov        eax, 0x4a2098
00449404  mov        dword ptr [esp + 0x24], 0
0044940c  call       0x463c10

// block 00449411
00449411  mov        dword ptr [ebx + 0x5c10], eax
00449417  test       eax, eax
00449419  je         0x449c27

// block 0044941f
0044941f  cmp        dword ptr [esp + 0xc], 0
00449424  jle        0x4494ab

// block 0044942a
0044942a  lea        edx, [ebx + 0xf34]
00449430  lea        ecx, [ebx + 0x734]
00449436  lea        esi, [ebx + 0x1774]
0044943c  mov        dword ptr [esp + 0x18], edx
00449440  mov        dword ptr [esp + 0x14], ecx

// block 00449444
00449444  mov        cl, byte ptr [eax]
00449446  cmp        cl, 0x23
00449449  je         0x44949b

// block 0044944b
0044944b  cmp        cl, 0x40
0044944e  jne        0x44949b

// block 00449450
00449450  mov        edx, dword ptr [esp + 0x14]
00449454  lea        ecx, [eax + 1]
00449457  push       edx
00449458  lea        edi, [esp + 0x10]
0044945c  call       0x449dc0

// block 00449461
00449461  mov        ecx, dword ptr [esp + 0x18]
00449465  push       ecx
00449466  mov        ecx, eax
00449468  call       0x449dc0

// block 0044946d
0044946d  mov        dword ptr [esp + 0x20], 8

// block 00449475
00449475  push       esi
00449476  lea        edi, [esp + 0x10]
0044947a  mov        ecx, eax
0044947c  call       0x449dc0

// block 00449481
00449481  add        esi, 0x42
00449484  sub        dword ptr [esp + 0x20], 1
00449489  jne        0x449475

// block 0044948b
0044948b  inc        dword ptr [esp + 0x1c]
0044948f  add        dword ptr [esp + 0x14], 0x40
00449494  add        dword ptr [esp + 0x18], 0x42
00449499  jmp        0x4494a4

// block 0044949b
0044949b  lea        edx, [esp + 0xc]
0044949f  call       0x449d80

// block 004494a4
004494a4  cmp        dword ptr [esp + 0xc], 0
004494a9  jg         0x449444

// block 004494ab
004494ab  mov        edx, dword ptr [esp + 0x1c]
004494af  mov        ecx, dword ptr [esp + 0x10]
004494b3  mov        dword ptr [ebx + 0x30], edx
004494b6  mov        eax, dword ptr [ecx + 8]
004494b9  xor        esi, esi
004494bb  cmp        eax, esi
004494bd  je         0x4494cc

// block 004494bf
004494bf  jg         0x4494c6

// block 004494c1
004494c1  dec        eax
004494c2  mov        dword ptr [ecx], eax
004494c4  jmp        0x4494ce

// block 004494c6
004494c6  xor        eax, eax
004494c8  mov        dword ptr [ecx], eax
004494ca  jmp        0x4494ce

// block 004494cc
004494cc  mov        dword ptr [ecx], esi

// block 004494ce
004494ce  mov        dword ptr [ebx + 0x5974], esi
004494d4  mov        dword ptr [ebx + 0x724], edx
004494da  lea        edi, [ebx + 0x700]

// block 004494e0
004494e0  mov        ecx, dword ptr [0x4cee70]
004494e6  push       0
004494e8  lea        edx, [esi + 0x29]
004494eb  push       edx
004494ec  lea        eax, [esp + 0x28]
004494f0  push       eax
004494f1  push       ecx
004494f2  mov        eax, 0x17
004494f7  xor        ecx, ecx
004494f9  call       0x4615a0

// block 004494fe
004494fe  mov        edx, dword ptr [esp + 0x20]
00449502  mov        dword ptr [edi], edx
00449504  inc        esi
00449505  add        edi, 4
00449508  cmp        esi, 8
0044950b  jl         0x4494e0

// block 0044950d
0044950d  xor        eax, eax
0044950f  mov        dword ptr [ebx + 0x728], eax
00449515  mov        dword ptr [ebx + 0x72c], eax
0044951b  mov        dword ptr [ebx + 0x730], eax

// block 00449521
00449521  mov        eax, dword ptr [ebx + 0x2b8]
00449527  cmp        eax, 0xa
0044952a  jge        0x4496f0

// block 00449530
00449530  fld        dword ptr [0x4a3e04]
00449536  lea        ecx, [eax + eax*4]
00449539  fstp       dword ptr [esp + 0x24]
0044953d  lea        edx, [ecx*8 - 0x28]
00449544  fild       dword ptr [ebx + 0x5974]
0044954a  mov        dword ptr [esp + 0x20], edx
0044954e  add        eax, eax
00449550  lea        ecx, [eax - 2]
00449553  cmp        ecx, eax
00449555  fmul       qword ptr [0x4a4170]
0044955b  mov        dword ptr [esp + 0xc], ecx
0044955f  fsubr      qword ptr [0x4a40c8]
00449565  fiadd      dword ptr [esp + 0x20]
00449569  fstp       dword ptr [esp + 0x28]
0044956d  fldz       
0044956f  fstp       dword ptr [esp + 0x2c]
00449573  jge        0x4496f0

// block 00449579
00449579  mov        eax, ecx
0044957b  shl        ecx, 5
0044957e  add        ecx, eax
00449580  lea        edx, [ebx + ecx*2 + 0xf34]
00449587  lea        eax, [ebx + eax*4 + 0x6b0]
0044958e  mov        dword ptr [esp + 0x18], edx
00449592  mov        dword ptr [esp + 0x1c], eax

// block 00449596
00449596  mov        ecx, dword ptr [esp + 0xc]
0044959a  cmp        ecx, dword ptr [ebx + 0x724]
004495a0  jge        0x4496f0

// block 004495a6
004495a6  mov        edx, ecx
004495a8  mov        ecx, dword ptr [ebx + 0x14]
004495ab  push       0
004495ad  add        edx, 0xd3
004495b3  push       edx
004495b4  lea        eax, [esp + 0x1c]
004495b8  push       eax
004495b9  push       ecx
004495ba  mov        eax, 0x17
004495bf  xor        ecx, ecx
004495c1  call       0x4615a0

// block 004495c6
004495c6  mov        edx, dword ptr [esp + 0x14]
004495ca  mov        edi, dword ptr [esp + 0x1c]
004495ce  mov        eax, edx
004495d0  mov        dword ptr [edi], edx
004495d2  mov        edx, dword ptr [0x4ce8cc]
004495d8  push       eax
004495d9  call       0x461920

// block 004495de
004495de  mov        esi, eax
004495e0  test       esi, esi
004495e2  jne        0x4495e6

// block 004495e4
004495e4  mov        dword ptr [edi], eax

// block 004495e6
004495e6  mov        ecx, dword ptr [0x4b451c]
004495ec  mov        edx, dword ptr [esp + 0xc]
004495f0  xor        edi, edi
004495f2  cmp        byte ptr [ecx + edx + 0x1e9da], 0
004495fa  je         0x449613

// block 004495fc
004495fc  mov        eax, dword ptr [esp + 0x18]
00449600  push       eax
00449601  push       edi
00449602  push       edi
00449603  push       edi
00449604  push       0xffffff
00449609  call       0x460760

// block 0044960e
0044960e  add        esp, 0x14
00449611  jmp        0x449631

// block 00449613
00449613  mov        ecx, dword ptr [esp + 0xc]
00449617  inc        ecx
00449618  push       ecx
00449619  push       0x4a20a8
0044961e  push       0
00449620  push       0
00449622  push       0
00449624  push       0xffffff
00449629  call       0x460760

// block 0044962e
0044962e  add        esp, 0x18

// block 00449631
00449631  mov        eax, dword ptr [ebx + 0x5974]
00449637  mov        edi, dword ptr [esp + 0xc]
0044963b  cmp        edi, eax
0044963d  jl         0x44964f

// block 0044963f
0044963f  add        eax, 0xa
00449642  cmp        edi, eax
00449644  jge        0x44964f

// block 00449646
00449646  or         dword ptr [esi + 0x47c], 2
0044964d  jmp        0x449656

// block 0044964f
0044964f  and        dword ptr [esi + 0x47c], 0xfffffffd

// block 00449656
00449656  cmp        edi, dword ptr [ebx + 0x28]
00449659  jne        0x449669

// block 0044965b
0044965b  fld        dword ptr [esp + 0x24]
0044965f  fsub       qword ptr [0x4a4108]
00449665  fstp       dword ptr [esp + 0x24]

// block 00449669
00449669  push       0
0044966b  lea        edx, [esi + 0x424]
00449671  push       4
00449673  lea        ecx, [esp + 0x2c]
00449677  mov        eax, esi
00449679  call       0x427b90

// block 0044967e
0044967e  mov        eax, dword ptr [ebx + 0x28]
00449681  cmp        edi, eax
00449683  jne        0x449693

// block 00449685
00449685  fld        dword ptr [esp + 0x24]
00449689  fadd       qword ptr [0x4a4108]
0044968f  fstp       dword ptr [esp + 0x24]

// block 00449693
00449693  fld        dword ptr [esp + 0x28]
00449697  xor        edx, edx
00449699  fadd       qword ptr [0x4a4170]
0044969f  cmp        edi, eax
004496a1  setne      dl
004496a4  fstp       dword ptr [esp + 0x28]
004496a8  add        edx, 2
004496ab  movzx      eax, dx
004496ae  mov        dword ptr [esp + 0x20], eax
004496b2  mov        eax, dword ptr [esi + 0x494]
004496b8  test       eax, eax
004496ba  je         0x4496c5

// block 004496bc
004496bc  movsx      edx, word ptr [esp + 0x20]
004496c1  mov        ecx, esi
004496c3  call       eax

// block 004496c5
004496c5  mov        cx, word ptr [esp + 0x20]
004496ca  add        dword ptr [esp + 0x1c], 4
004496cf  add        dword ptr [esp + 0x18], 0x42
004496d4  mov        word ptr [esi + 0x3c4], cx
004496db  mov        edx, dword ptr [ebx + 0x2b8]
004496e1  inc        edi
004496e2  add        edx, edx
004496e4  cmp        edi, edx
004496e6  mov        dword ptr [esp + 0xc], edi
004496ea  jl         0x449596

// block 004496f0
004496f0  cmp        dword ptr [ebx + 0x2b8], 0xa
004496f7  jl         0x449d5f

// block 004496fd
004496fd  mov        dword ptr [ebx + 0x24], 1
00449704  mov        eax, dword ptr [ebx + 0x2c4]
0044970a  test       al, 1
0044970c  jne        0x44973d

// block 0044970e
0044970e  fldz       
00449710  or         eax, 1
00449713  fstp       dword ptr [ebx + 0x2bc]
00449719  mov        dword ptr [ebx + 0x2b8], 0
00449723  mov        dword ptr [ebx + 0x2b4], 0xfff0bdc1
0044972d  mov        dword ptr [ebx + 0x2c0], 0x4b2ed0
00449737  mov        dword ptr [ebx + 0x2c4], eax

// block 0044973d
0044973d  fldz       
0044973f  mov        dword ptr [ebx + 0x2b8], 0
00449749  fstp       dword ptr [ebx + 0x2bc]
0044974f  mov        dword ptr [ebx + 0x2b4], 0xffffffff
00449759  xor        eax, eax
0044975b  pop        edi
0044975c  pop        esi
0044975d  pop        ebx
0044975e  mov        esp, ebp
00449760  pop        ebp
00449761  ret        4

// block 00449764
00449764  mov        eax, dword ptr [ebx + 0x2b8]
0044976a  and        eax, 0x80000001
0044976f  jns        0x449776

// block 00449771
00449771  dec        eax
00449772  or         eax, 0xfffffffe
00449775  inc        eax

// block 00449776
00449776  jne        0x449810

// block 0044977c
0044977c  mov        eax, dword ptr [ebx + 0x728]
00449782  cmp        eax, 8
00449785  jge        0x449810

// block 0044978b
0044978b  lea        esi, [ebx + eax*4 + 0x700]
00449792  call       0x461c50

// block 00449797
00449797  mov        ecx, dword ptr [0x4b451c]
0044979d  mov        esi, eax
0044979f  mov        eax, dword ptr [ebx + 0x72c]
004497a5  cmp        byte ptr [eax + ecx + 0x1e9da], 0
004497ad  jne        0x4497d3

// block 004497af
004497af  cmp        dword ptr [ebx + 0x730], 0
004497b6  je         0x4497d3

// block 004497b8
004497b8  mov        edx, dword ptr [ebx + 0x728]
004497be  mov        eax, dword ptr [edx*4 + 0x4b33c0]
004497c5  push       eax
004497c6  push       0
004497c8  push       0
004497ca  push       0
004497cc  push       0x8080ff
004497d1  jmp        0x4497f6

// block 004497d3
004497d3  mov        ecx, dword ptr [ebx + 0x728]
004497d9  lea        eax, [ecx + eax*8]
004497dc  mov        edx, eax
004497de  shl        edx, 5
004497e1  add        edx, eax
004497e3  lea        eax, [ebx + edx*2 + 0x1774]
004497ea  push       eax
004497eb  push       0
004497ed  push       0
004497ef  push       0
004497f1  push       0xffffff

// block 004497f6
004497f6  xor        edi, edi
004497f8  call       0x460760

// block 004497fd
004497fd  add        esp, 0x14
00449800  mov        edi, 2
00449805  call       0x40d6e0

// block 0044980a
0044980a  inc        dword ptr [ebx + 0x728]

// block 00449810
00449810  cmp        dword ptr [ebx + 0x2b8], 4
00449817  jle        0x449d5f

// block 0044981d
0044981d  mov        ecx, 2
00449822  mov        eax, ebx
00449824  call       0x43ef40

// block 00449829
00449829  xor        eax, eax
0044982b  pop        edi
0044982c  pop        esi
0044982d  pop        ebx
0044982e  mov        esp, ebp
00449830  pop        ebp
00449831  ret        4

// block 00449834
00449834  mov        ecx, dword ptr [ebx + 0x2b8]
0044983a  and        ecx, 0x80000001
00449840  jns        0x449847

// block 00449842
00449842  dec        ecx
00449843  or         ecx, 0xfffffffe
00449846  inc        ecx

// block 00449847
00449847  jne        0x4498e1

// block 0044984d
0044984d  mov        eax, dword ptr [ebx + 0x728]
00449853  cmp        eax, 8
00449856  jge        0x4498e1

// block 0044985c
0044985c  lea        esi, [ebx + eax*4 + 0x700]
00449863  call       0x461c50

// block 00449868
00449868  mov        edx, dword ptr [0x4b451c]
0044986e  mov        esi, eax
00449870  mov        eax, dword ptr [ebx + 0x72c]
00449876  cmp        byte ptr [eax + edx + 0x1e9da], 0
0044987e  jne        0x4498a4

// block 00449880
00449880  cmp        dword ptr [ebx + 0x730], 0
00449887  je         0x4498a4

// block 00449889
00449889  mov        eax, dword ptr [ebx + 0x728]
0044988f  mov        ecx, dword ptr [eax*4 + 0x4b33c0]
00449896  push       ecx
00449897  push       0
00449899  push       0
0044989b  push       0
0044989d  push       0x8080ff
004498a2  jmp        0x4498c7

// block 004498a4
004498a4  mov        edx, dword ptr [ebx + 0x728]
004498aa  lea        eax, [edx + eax*8]
004498ad  mov        ecx, eax
004498af  shl        ecx, 5
004498b2  add        ecx, eax
004498b4  lea        edx, [ebx + ecx*2 + 0x1774]
004498bb  push       edx
004498bc  push       0
004498be  push       0
004498c0  push       0
004498c2  push       0xffffff

// block 004498c7
004498c7  xor        edi, edi
004498c9  call       0x460760

// block 004498ce
004498ce  add        esp, 0x14
004498d1  mov        edi, 2
004498d6  call       0x40d6e0

// block 004498db
004498db  inc        dword ptr [ebx + 0x728]

// block 004498e1
004498e1  mov        eax, dword ptr [ebx + 0x28]
004498e4  lea        esi, [ebx + 0x28]
004498e7  mov        dword ptr [esi + 4], eax
004498ea  mov        al, 0x10
004498ec  mov        dword ptr [esp + 0x10], esi
004498f0  test       byte ptr [0x4d48c4], al
004498f6  jne        0x449900

// block 004498f8
004498f8  test       byte ptr [0x4d48c0], al
004498fe  je         0x449909

// block 00449900
00449900  push       -1
00449902  mov        eax, esi
00449904  call       0x464970

// block 00449909
00449909  mov        al, 0x20
0044990b  test       byte ptr [0x4d48c4], al
00449911  jne        0x44991b

// block 00449913
00449913  test       byte ptr [0x4d48c0], al
00449919  je         0x449924

// block 0044991b
0044991b  push       1
0044991d  mov        eax, esi
0044991f  call       0x464970

// block 00449924
00449924  mov        ecx, dword ptr [esi + 4]
00449927  cmp        ecx, dword ptr [esi]
00449929  je         0x449ac1

// block 0044992f
0044992f  mov        edx, 0xa
00449934  call       0x453d90

// block 00449939
00449939  mov        ecx, dword ptr [ebx + 0x5974]
0044993f  mov        edx, esi
00449941  mov        eax, dword ptr [edx]
00449943  cmp        eax, ecx
00449945  jl         0x449951

// block 00449947
00449947  add        ecx, 0xa
0044994a  cmp        eax, ecx
0044994c  jl         0x449957

// block 0044994e
0044994e  add        eax, -9

// block 00449951
00449951  mov        dword ptr [ebx + 0x5974], eax

// block 00449957
00449957  fld        dword ptr [0x4a3e04]
0044995d  xor        edi, edi
0044995f  cmp        dword ptr [ebx + 0x724], edi
00449965  fstp       dword ptr [esp + 0x24]
00449969  fild       dword ptr [ebx + 0x5974]
0044996f  fmul       qword ptr [0x4a4170]
00449975  fsubr      qword ptr [0x4a40c8]
0044997b  fstp       dword ptr [esp + 0x28]
0044997f  fldz       
00449981  fstp       dword ptr [esp + 0x2c]
00449985  jle        0x449aae

// block 0044998b
0044998b  lea        eax, [ebx + 0x6b0]
00449991  mov        dword ptr [esp + 0xc], eax

// block 00449995
00449995  mov        ecx, dword ptr [esp + 0xc]
00449999  mov        edx, dword ptr [ecx]
0044999b  push       edx
0044999c  mov        edx, dword ptr [0x4ce8cc]
004499a2  call       0x461920

// block 004499a7
004499a7  mov        esi, eax
004499a9  test       esi, esi
004499ab  jne        0x4499b3

// block 004499ad
004499ad  mov        eax, dword ptr [esp + 0xc]
004499b1  mov        dword ptr [eax], esi

// block 004499b3
004499b3  mov        eax, dword ptr [ebx + 0x5974]
004499b9  cmp        edi, eax
004499bb  jl         0x4499cd

// block 004499bd
004499bd  add        eax, 0xa
004499c0  cmp        edi, eax
004499c2  jge        0x4499cd

// block 004499c4
004499c4  or         dword ptr [esi + 0x47c], 2
004499cb  jmp        0x4499d4

// block 004499cd
004499cd  and        dword ptr [esi + 0x47c], 0xfffffffd

// block 004499d4
004499d4  mov        ecx, dword ptr [esp + 0x10]
004499d8  cmp        edi, dword ptr [ecx]
004499da  jne        0x4499ea

// block 004499dc
004499dc  fld        dword ptr [esp + 0x24]
004499e0  fsub       qword ptr [0x4a4108]
004499e6  fstp       dword ptr [esp + 0x24]

// block 004499ea
004499ea  fld        dword ptr [esi + 0x428]
004499f0  fsub       dword ptr [esp + 0x28]
004499f4  fstp       dword ptr [esp + 0x20]
004499f8  fld        dword ptr [esp + 0x20]
004499fc  fabs       
004499fe  fstp       dword ptr [esp + 0x20]
00449a02  fld        dword ptr [esp + 0x20]
00449a06  fcomp      dword ptr [0x4a4010]
00449a0c  fnstsw     ax
00449a0e  test       ah, 5
00449a11  jp         0x449a2a

// block 00449a13
00449a13  push       0
00449a15  lea        edx, [esi + 0x424]
00449a1b  push       4
00449a1d  lea        ecx, [esp + 0x2c]
00449a21  mov        eax, esi
00449a23  call       0x427b90

// block 00449a28
00449a28  jmp        0x449a48

// block 00449a2a
00449a2a  mov        edx, dword ptr [esp + 0x24]
00449a2e  mov        eax, dword ptr [esp + 0x28]
00449a32  mov        ecx, dword ptr [esp + 0x2c]
00449a36  mov        dword ptr [esi + 0x424], edx
00449a3c  mov        dword ptr [esi + 0x428], eax
00449a42  mov        dword ptr [esi + 0x42c], ecx

// block 00449a48
00449a48  mov        edx, dword ptr [esp + 0x10]
00449a4c  mov        eax, dword ptr [edx]
00449a4e  cmp        edi, eax
00449a50  jne        0x449a60

// block 00449a52
00449a52  fld        dword ptr [esp + 0x24]
00449a56  fadd       qword ptr [0x4a4108]
00449a5c  fstp       dword ptr [esp + 0x24]

// block 00449a60
00449a60  fld        dword ptr [esp + 0x28]
00449a64  xor        ecx, ecx
00449a66  fadd       qword ptr [0x4a4170]
00449a6c  cmp        edi, eax
00449a6e  mov        eax, dword ptr [esi + 0x494]
00449a74  setne      cl
00449a77  fstp       dword ptr [esp + 0x28]
00449a7b  add        ecx, 2
00449a7e  movzx      edx, cx
00449a81  mov        dword ptr [esp + 0x20], edx
00449a85  test       eax, eax
00449a87  je         0x449a90

// block 00449a89
00449a89  movsx      edx, dx
00449a8c  mov        ecx, esi
00449a8e  call       eax

// block 00449a90
00449a90  mov        ax, word ptr [esp + 0x20]
00449a95  add        dword ptr [esp + 0xc], 4
00449a9a  inc        edi
00449a9b  mov        word ptr [esi + 0x3c4], ax
00449aa2  cmp        edi, dword ptr [ebx + 0x724]
00449aa8  jl         0x449995

// block 00449aae
00449aae  cmp        dword ptr [ebx + 0x728], 8
00449ab5  jl         0x449ac1

// block 00449ab7
00449ab7  mov        dword ptr [ebx + 0x730], 0

// block 00449ac1
00449ac1  mov        eax, dword ptr [0x4d48c4]
00449ac6  test       eax, 0x80001
00449acb  je         0x449c1c

// block 00449ad1
00449ad1  lea        esi, [ebx + 0x700]
00449ad7  mov        edi, 8

// block 00449adc
00449adc  mov        ecx, dword ptr [esi]
00449ade  push       ecx
00449adf  mov        ebx, 3
00449ae4  call       0x461970

// block 00449ae9
00449ae9  add        esi, 4
00449aec  sub        edi, 1
00449aef  jne        0x449adc

// block 00449af1
00449af1  mov        ecx, dword ptr [esp + 0x10]
00449af5  mov        esi, dword ptr [ebp + 8]
00449af8  mov        edx, dword ptr [ecx]
00449afa  xor        ebx, ebx
00449afc  mov        dword ptr [esi + 0x72c], edx
00449b02  mov        dword ptr [esi + 0x728], ebx
00449b08  mov        eax, dword ptr [esi + 0x2c4]
00449b0e  test       al, 1
00449b10  jne        0x449b3d

// block 00449b12
00449b12  fldz       
00449b14  or         eax, 1
00449b17  fstp       dword ptr [esi + 0x2bc]
00449b1d  mov        dword ptr [esi + 0x2b8], ebx
00449b23  mov        dword ptr [esi + 0x2b4], 0xfff0bdc1
00449b2d  mov        dword ptr [esi + 0x2c0], 0x4b2ed0
00449b37  mov        dword ptr [esi + 0x2c4], eax

// block 00449b3d
00449b3d  fldz       
00449b3f  mov        edx, dword ptr [0x4b451c]
00449b45  fstp       dword ptr [esi + 0x2bc]
00449b4b  mov        dword ptr [esi + 0x2b8], ebx
00449b51  mov        dword ptr [esi + 0x2b4], 0xffffffff
00449b5b  mov        eax, dword ptr [esi + 0x72c]
00449b61  cmp        byte ptr [eax + edx + 0x1e9da], bl
00449b68  jne        0x449bbe

// block 00449b6a
00449b6a  cmp        dword ptr [esi + 0x730], ebx
00449b70  jne        0x449bbe

// block 00449b72
00449b72  test       byte ptr [0x4ceae8], 0x10
00449b79  push       ebx
00449b7a  mov        edi, 0x4a0a20
00449b7f  mov        eax, 0x4cf4e8
00449b84  je         0x449ba2

// block 00449b86
00449b86  push       4
00449b88  call       0x454960

// block 00449b8d
00449b8d  mov        dword ptr [esi + 0x730], 1
00449b97  xor        eax, eax
00449b99  pop        edi
00449b9a  pop        esi
00449b9b  pop        ebx
00449b9c  mov        esp, ebp
00449b9e  pop        ebp
00449b9f  ret        4

// block 00449ba2
00449ba2  push       3
00449ba4  call       0x454960

// block 00449ba9
00449ba9  mov        dword ptr [esi + 0x730], 1
00449bb3  xor        eax, eax
00449bb5  pop        edi
00449bb6  pop        esi
00449bb7  pop        ebx
00449bb8  mov        esp, ebp
00449bba  pop        ebp
00449bbb  ret        4

// block 00449bbe
00449bbe  mov        eax, dword ptr [ecx]
00449bc0  shl        eax, 6
00449bc3  lea        ecx, [eax + esi + 0x734]
00449bca  push       ecx
00449bcb  push       ebx
00449bcc  call       0x4300d0

// block 00449bd1
00449bd1  test       byte ptr [0x4ceae8], 0x10
00449bd8  je         0x449bec

// block 00449bda
00449bda  push       ebx
00449bdb  push       4
00449bdd  mov        edi, 0x4a0a20
00449be2  mov        eax, 0x4cf4e8
00449be7  call       0x454960

// block 00449bec
00449bec  push       ebx
00449bed  push       2
00449bef  mov        edi, 0x4a0a20
00449bf4  mov        eax, 0x4cf4e8
00449bf9  call       0x454960

// block 00449bfe
00449bfe  mov        edx, dword ptr [0x4b451c]
00449c04  mov        byte ptr [edx + 0x1e9da], 1
00449c0b  mov        dword ptr [esi + 0x730], ebx
00449c11  xor        eax, eax
00449c13  pop        edi
00449c14  pop        esi
00449c15  pop        ebx
00449c16  mov        esp, ebp
00449c18  pop        ebp
00449c19  ret        4

// block 00449c1c
00449c1c  test       eax, 0x102
00449c21  je         0x449d5f

// block 00449c27
00449c27  mov        eax, dword ptr [ebx + 0x5c10]
00449c2d  xor        esi, esi
00449c2f  cmp        eax, esi
00449c31  je         0x449c42

// block 00449c33
00449c33  push       eax
00449c34  call       0x46c941

// block 00449c39
00449c39  add        esp, 4
00449c3c  mov        dword ptr [ebx + 0x5c10], esi

// block 00449c42
00449c42  mov        eax, dword ptr [esp + 0x10]
00449c46  mov        dword ptr [ebx + 0x5c10], esi
00449c4c  call       0x464940

// block 00449c51
00449c51  cmp        dword ptr [ebx + 0x724], esi
00449c57  jle        0x449c7d

// block 00449c59
00449c59  lea        edi, [ebx + 0x6b0]

// block 00449c5f
00449c5f  mov        eax, dword ptr [edi]
00449c61  push       eax
00449c62  mov        ebx, 1
00449c67  call       0x461970

// block 00449c6c
00449c6c  mov        ecx, dword ptr [ebp + 8]
00449c6f  inc        esi
00449c70  add        edi, 4
00449c73  cmp        esi, dword ptr [ecx + 0x724]
00449c79  jl         0x449c5f

// block 00449c7b
00449c7b  mov        ebx, ecx

// block 00449c7d
00449c7d  lea        esi, [ebx + 0x700]
00449c83  mov        edi, 8

// block 00449c88
00449c88  mov        edx, dword ptr [esi]
00449c8a  push       edx
00449c8b  mov        ebx, 1
00449c90  call       0x461970

// block 00449c95
00449c95  add        esi, 4
00449c98  sub        edi, ebx
00449c9a  jne        0x449c88

// block 00449c9c
00449c9c  lea        edx, [ebx + 8]
00449c9f  call       0x453d90

// block 00449ca4
00449ca4  mov        eax, dword ptr [ebp + 8]
00449ca7  mov        dword ptr [eax + 0x24], 3
00449cae  mov        ecx, dword ptr [eax + 0x2c4]
00449cb4  test       bl, cl
00449cb6  jne        0x449ce2

// block 00449cb8
00449cb8  fldz       
00449cba  or         ecx, ebx
00449cbc  fstp       dword ptr [eax + 0x2bc]
00449cc2  mov        dword ptr [eax + 0x2b8], edi
00449cc8  mov        dword ptr [eax + 0x2b4], 0xfff0bdc1
00449cd2  mov        dword ptr [eax + 0x2c0], 0x4b2ed0
00449cdc  mov        dword ptr [eax + 0x2c4], ecx

// block 00449ce2
00449ce2  fldz       
00449ce4  mov        dword ptr [eax + 0x2b8], 0
00449cee  fstp       dword ptr [eax + 0x2bc]
00449cf4  mov        dword ptr [eax + 0x2b4], 0xffffffff
00449cfe  xor        eax, eax
00449d00  pop        edi
00449d01  pop        esi
00449d02  pop        ebx
00449d03  mov        esp, ebp
00449d05  pop        ebp
00449d06  ret        4

// block 00449d09
00449d09  cmp        dword ptr [ebx + 0x2b8], 0xa
00449d10  jl         0x449d5f

// block 00449d12
00449d12  mov        esi, 0x73
00449d17  mov        edi, ebx
00449d19  call       0x43efd0

// block 00449d1e
00449d1e  mov        eax, dword ptr [ebx + 0x66c]
00449d24  push       eax
00449d25  lea        ebx, [esi - 0x72]
00449d28  call       0x461970

// block 00449d2d
00449d2d  mov        esi, edi
00449d2f  mov        edx, ebx
00449d31  mov        eax, esi
00449d33  mov        dword ptr [esi + 0x66c], 0
00449d3d  call       0x43eee0

// block 00449d42
00449d42  push       0x4a1f48
00449d47  push       0
00449d49  call       0x4300d0

// block 00449d4e
00449d4e  push       0
00449d50  push       0
00449d52  call       0x430150

// block 00449d57
00449d57  lea        eax, [esi + 0x28]
00449d5a  call       0x464940

// block 00449d5f
00449d5f  pop        edi
00449d60  pop        esi
00449d61  xor        eax, eax
00449d63  pop        ebx
00449d64  mov        esp, ebp
00449d66  pop        ebp
00449d67  ret        4
