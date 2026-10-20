
// block 00437810
00437810  sub        esp, 0x10
00437813  fld        dword ptr [eax]
00437815  push       esi
00437816  fld        qword ptr [0x4a3cf8]
0043781c  mov        esi, dword ptr [0x4b4514]
00437822  fmul       st(1), st(0)
00437824  fld        dword ptr [ecx]
00437826  fsub       st(2)
00437828  fstp       dword ptr [esp + 0xc]
0043782c  fmul       dword ptr [eax + 4]
0043782f  fld        dword ptr [ecx + 4]
00437832  fsub       st(1)
00437834  fstp       dword ptr [esp + 0x10]
00437838  fld        dword ptr [ecx]
0043783a  faddp      st(2)
0043783c  fxch       st(1)
0043783e  fstp       dword ptr [esp + 4]
00437842  fadd       dword ptr [ecx + 4]
00437845  fstp       dword ptr [esp + 8]
00437849  fld        dword ptr [esi + 0x9cc]
0043784f  fld        dword ptr [esp + 4]
00437853  fcompp     
00437855  fnstsw     ax
00437857  test       ah, 5
0043785a  jnp        0x4378f9

// block 00437860
00437860  fld        dword ptr [esi + 0x9d0]
00437866  fld        dword ptr [esp + 8]
0043786a  fcompp     
0043786c  fnstsw     ax
0043786e  test       ah, 5
00437871  jnp        0x4378f9

// block 00437877
00437877  fld        dword ptr [esi + 0x9d8]
0043787d  fld        dword ptr [esp + 0xc]
00437881  fcompp     
00437883  fnstsw     ax
00437885  test       ah, 0x41
00437888  je         0x4378f9

// block 0043788a
0043788a  fld        dword ptr [esi + 0x9dc]
00437890  fld        dword ptr [esp + 0x10]
00437894  fcompp     
00437896  fnstsw     ax
00437898  test       ah, 0x41
0043789b  je         0x4378f9

// block 0043789d
0043789d  mov        eax, dword ptr [0x4b43e4]
004378a2  test       eax, eax
004378a4  je         0x4378b3

// block 004378a6
004378a6  cmp        dword ptr [eax + 0x6d30], 0
004378ad  jne        0x437977

// block 004378b3
004378b3  mov        eax, dword ptr [esi + 0xa28]
004378b9  cmp        eax, 2
004378bc  je         0x437977

// block 004378c2
004378c2  cmp        eax, 4
004378c5  je         0x437977

// block 004378cb
004378cb  cmp        eax, 3
004378ce  je         0x437977

// block 004378d4
004378d4  test       byte ptr [esi + 0xc414], 2
004378db  jne        0x437977

// block 004378e1
004378e1  cmp        dword ptr [esi + 0xc404], 0
004378e8  jg         0x4378ef

// block 004378ea
004378ea  call       0x438370

// block 004378ef
004378ef  mov        eax, 1
004378f4  pop        esi
004378f5  add        esp, 0x10
004378f8  ret        

// block 004378f9
004378f9  fld        dword ptr [ecx]
004378fb  fld        qword ptr [0x4a4258]
00437901  fsub       st(1), st(0)
00437903  fxch       st(1)
00437905  fstp       dword ptr [esp + 0xc]
00437909  fld        dword ptr [ecx + 4]
0043790c  fsub       st(1)
0043790e  fstp       dword ptr [esp + 0x10]
00437912  fld        dword ptr [ecx]
00437914  fadd       st(1)
00437916  fstp       dword ptr [esp + 4]
0043791a  fadd       dword ptr [ecx + 4]
0043791d  fstp       dword ptr [esp + 8]
00437921  fld        dword ptr [esi + 0x9cc]
00437927  fld        dword ptr [esp + 4]
0043792b  fcompp     
0043792d  fnstsw     ax
0043792f  test       ah, 5
00437932  jnp        0x437977

// block 00437934
00437934  fld        dword ptr [esi + 0x9d0]
0043793a  fld        dword ptr [esp + 8]
0043793e  fcompp     
00437940  fnstsw     ax
00437942  test       ah, 5
00437945  jnp        0x437977

// block 00437947
00437947  fld        dword ptr [esi + 0x9d8]
0043794d  fld        dword ptr [esp + 0xc]
00437951  fcompp     
00437953  fnstsw     ax
00437955  test       ah, 0x41
00437958  je         0x437977

// block 0043795a
0043795a  fld        dword ptr [esi + 0x9dc]
00437960  fld        dword ptr [esp + 0x10]
00437964  fcompp     
00437966  fnstsw     ax
00437968  test       ah, 0x41
0043796b  je         0x437977

// block 0043796d
0043796d  mov        eax, 2
00437972  pop        esi
00437973  add        esp, 0x10
00437976  ret        

// block 00437977
00437977  xor        eax, eax
00437979  pop        esi
0043797a  add        esp, 0x10
0043797d  ret        
