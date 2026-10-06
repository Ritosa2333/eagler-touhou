
// block 00451200
00451200  push       ecx
00451201  mov        eax, dword ptr [0x4ce8f0]
00451206  mov        ecx, dword ptr [eax]
00451208  mov        edx, dword ptr [ecx + 0xe4]
0045120e  push       1
00451210  push       7
00451212  push       eax
00451213  call       edx

// block 00451215
00451215  mov        eax, dword ptr [0x4ce8f0]
0045121a  mov        ecx, dword ptr [eax]
0045121c  mov        edx, dword ptr [ecx + 0xe4]
00451222  push       0
00451224  push       0x89
00451229  push       eax
0045122a  call       edx

// block 0045122c
0045122c  mov        eax, dword ptr [0x4ce8f0]
00451231  mov        ecx, dword ptr [eax]
00451233  mov        edx, dword ptr [ecx + 0xe4]
00451239  push       1
0045123b  push       0x16
0045123d  push       eax
0045123e  call       edx

// block 00451240
00451240  mov        eax, dword ptr [0x4ce8f0]
00451245  mov        ecx, dword ptr [eax]
00451247  mov        edx, dword ptr [ecx + 0xe4]
0045124d  push       1
0045124f  push       0x1b
00451251  push       eax
00451252  call       edx

// block 00451254
00451254  mov        eax, dword ptr [0x4ce8f0]
00451259  mov        ecx, dword ptr [eax]
0045125b  mov        edx, dword ptr [ecx + 0xe4]
00451261  push       2
00451263  push       9
00451265  push       eax
00451266  call       edx

// block 00451268
00451268  mov        eax, dword ptr [0x4ce8f0]
0045126d  mov        ecx, dword ptr [eax]
0045126f  mov        edx, dword ptr [ecx + 0xe4]
00451275  push       5
00451277  push       0x13
00451279  push       eax
0045127a  call       edx

// block 0045127c
0045127c  mov        eax, dword ptr [0x4ce8f0]
00451281  mov        ecx, dword ptr [eax]
00451283  mov        edx, dword ptr [ecx + 0xe4]
00451289  push       6
0045128b  push       0x14
0045128d  push       eax
0045128e  call       edx

// block 00451290
00451290  mov        eax, dword ptr [0x4ce8f0]
00451295  mov        ecx, dword ptr [eax]
00451297  mov        edx, dword ptr [ecx + 0xe4]
0045129d  push       8
0045129f  push       0x17
004512a1  push       eax
004512a2  call       edx

// block 004512a4
004512a4  mov        eax, dword ptr [0x4ce8f0]
004512a9  mov        ecx, dword ptr [eax]
004512ab  mov        edx, dword ptr [ecx + 0xe4]
004512b1  push       1
004512b3  push       0xab
004512b8  push       eax
004512b9  call       edx

// block 004512bb
004512bb  mov        eax, dword ptr [0x4ce8f0]
004512c0  mov        ecx, dword ptr [eax]
004512c2  mov        edx, dword ptr [ecx + 0xe4]
004512c8  push       1
004512ca  push       0xf
004512cc  push       eax
004512cd  call       edx

// block 004512cf
004512cf  mov        eax, dword ptr [0x4ce8f0]
004512d4  mov        ecx, dword ptr [eax]
004512d6  mov        edx, dword ptr [ecx + 0xe4]
004512dc  push       1
004512de  push       0x18
004512e0  push       eax
004512e1  call       edx

// block 004512e3
004512e3  mov        eax, dword ptr [0x4ce8f0]
004512e8  mov        ecx, dword ptr [eax]
004512ea  push       7
004512ec  push       0x19
004512ee  mov        edx, dword ptr [ecx + 0xe4]
004512f4  push       eax
004512f5  call       edx

// block 004512f7
004512f7  mov        eax, dword ptr [0x4ce8f0]
004512fc  mov        ecx, dword ptr [eax]
004512fe  mov        edx, dword ptr [ecx + 0xe4]
00451304  push       1
00451306  push       0x1c
00451308  push       eax
00451309  call       edx

// block 0045130b
0045130b  fld1       
0045130d  mov        eax, dword ptr [0x4ce8f0]
00451312  fstp       dword ptr [esp]
00451315  mov        ecx, dword ptr [eax]
00451317  mov        edx, dword ptr [esp]
0045131a  push       edx
0045131b  push       0x26
0045131d  push       eax
0045131e  mov        eax, dword ptr [ecx + 0xe4]
00451324  call       eax

// block 00451326
00451326  mov        eax, dword ptr [0x4ce8f0]
0045132b  mov        ecx, dword ptr [eax]
0045132d  mov        edx, dword ptr [ecx + 0xe4]
00451333  push       0
00451335  push       0x23
00451337  push       eax
00451338  call       edx

// block 0045133a
0045133a  mov        eax, dword ptr [0x4ce8f0]
0045133f  mov        ecx, dword ptr [eax]
00451341  mov        edx, dword ptr [ecx + 0xe4]
00451347  push       3
00451349  push       0x8c
0045134e  push       eax
0045134f  call       edx

// block 00451351
00451351  mov        eax, dword ptr [0x4ce8f0]
00451356  mov        ecx, dword ptr [eax]
00451358  mov        edx, dword ptr [ecx + 0xe4]
0045135e  push       0xffa0a0a0
00451363  push       0x22
00451365  push       eax
00451366  call       edx

// block 00451368
00451368  fld        dword ptr [0x4a3de4]
0045136e  mov        eax, dword ptr [0x4ce8f0]
00451373  fstp       dword ptr [esp]
00451376  mov        edx, dword ptr [esp]
00451379  mov        ecx, dword ptr [eax]
0045137b  push       edx
0045137c  push       0x24
0045137e  push       eax
0045137f  mov        eax, dword ptr [ecx + 0xe4]
00451385  call       eax

// block 00451387
00451387  fld        dword ptr [0x4a3de0]
0045138d  mov        eax, dword ptr [0x4ce8f0]
00451392  fstp       dword ptr [esp]
00451395  mov        edx, dword ptr [esp]
00451398  mov        ecx, dword ptr [eax]
0045139a  push       edx
0045139b  push       0x25
0045139d  push       eax
0045139e  mov        eax, dword ptr [ecx + 0xe4]
004513a4  call       eax

// block 004513a6
004513a6  mov        eax, dword ptr [0x4ce8f0]
004513ab  mov        ecx, dword ptr [eax]
004513ad  mov        edx, dword ptr [ecx + 0xe4]
004513b3  push       0
004513b5  push       0xa1
004513ba  push       eax
004513bb  call       edx

// block 004513bd
004513bd  mov        eax, dword ptr [0x4ce8f0]
004513c2  mov        ecx, dword ptr [eax]
004513c4  mov        edx, dword ptr [ecx + 0x10c]
004513ca  push       4
004513cc  push       4
004513ce  push       0
004513d0  push       eax
004513d1  call       edx

// block 004513d3
004513d3  mov        eax, dword ptr [0x4ce8f0]
004513d8  mov        ecx, dword ptr [eax]
004513da  push       2
004513dc  push       5
004513de  push       0
004513e0  mov        edx, dword ptr [ecx + 0x10c]
004513e6  push       eax
004513e7  call       edx

// block 004513e9
004513e9  mov        eax, dword ptr [0x4ce8f0]
004513ee  mov        ecx, dword ptr [eax]
004513f0  mov        edx, dword ptr [ecx + 0x10c]
004513f6  push       3
004513f8  push       6
004513fa  push       0
004513fc  push       eax
004513fd  call       edx

// block 004513ff
004513ff  mov        eax, dword ptr [0x4ce8f0]
00451404  mov        ecx, dword ptr [eax]
00451406  mov        edx, dword ptr [ecx + 0x10c]
0045140c  push       4
0045140e  push       1
00451410  push       0
00451412  push       eax
00451413  call       edx

// block 00451415
00451415  mov        eax, dword ptr [0x4ce8f0]
0045141a  mov        ecx, dword ptr [eax]
0045141c  mov        edx, dword ptr [ecx + 0x10c]
00451422  push       2
00451424  push       2
00451426  push       0
00451428  push       eax
00451429  call       edx

// block 0045142b
0045142b  mov        eax, dword ptr [0x4ce8f0]
00451430  mov        ecx, dword ptr [eax]
00451432  mov        edx, dword ptr [ecx + 0x10c]
00451438  push       3
0045143a  push       3
0045143c  push       0
0045143e  push       eax
0045143f  call       edx

// block 00451441
00451441  mov        eax, dword ptr [0x4ce8f0]
00451446  mov        ecx, dword ptr [eax]
00451448  mov        edx, dword ptr [ecx + 0x10c]
0045144e  push       2
00451450  push       0x18
00451452  push       0
00451454  push       eax
00451455  call       edx

// block 00451457
00451457  mov        eax, dword ptr [0x4ce8f0]
0045145c  mov        ecx, dword ptr [eax]
0045145e  mov        edx, dword ptr [ecx + 0x10c]
00451464  push       0
00451466  push       0xb
00451468  push       0
0045146a  push       eax
0045146b  call       edx

// block 0045146d
0045146d  mov        eax, dword ptr [0x4ce8f0]
00451472  mov        ecx, dword ptr [eax]
00451474  mov        edx, dword ptr [ecx + 0x114]
0045147a  push       0
0045147c  push       7
0045147e  push       0
00451480  push       eax
00451481  call       edx

// block 00451483
00451483  mov        eax, dword ptr [0x4ce8f0]
00451488  mov        ecx, dword ptr [eax]
0045148a  mov        edx, dword ptr [ecx + 0x114]
00451490  push       2
00451492  push       5
00451494  push       0
00451496  push       eax
00451497  call       edx

// block 00451499
00451499  mov        eax, dword ptr [0x4ce8f0]
0045149e  mov        ecx, dword ptr [eax]
004514a0  mov        edx, dword ptr [ecx + 0x114]
004514a6  push       2
004514a8  push       6
004514aa  push       0
004514ac  push       eax
004514ad  call       edx

// block 004514af
004514af  mov        eax, dword ptr [0x4ce8f0]
004514b4  mov        ecx, dword ptr [eax]
004514b6  push       3
004514b8  push       3
004514ba  push       0
004514bc  push       eax
004514bd  mov        edx, dword ptr [ecx + 0x114]
004514c3  call       edx

// block 004514c5
004514c5  mov        eax, dword ptr [0x4ce8f0]
004514ca  mov        ecx, dword ptr [eax]
004514cc  mov        edx, dword ptr [ecx + 0x114]
004514d2  push       1
004514d4  push       1
004514d6  push       0
004514d8  push       eax
004514d9  call       edx

// block 004514db
004514db  mov        eax, dword ptr [0x4ce8f0]
004514e0  mov        ecx, dword ptr [eax]
004514e2  mov        edx, dword ptr [ecx + 0x114]
004514e8  push       1
004514ea  push       2
004514ec  push       0
004514ee  push       eax
004514ef  call       edx

// block 004514f1
004514f1  mov        eax, dword ptr [0x4ce8cc]
004514f6  test       eax, eax
004514f8  je         0x45151f

// block 004514fa
004514fa  mov        cl, 0xff
004514fc  mov        byte ptr [eax + 0x4b5640], 7
00451503  mov        byte ptr [eax + 0x4b5641], cl
00451509  mov        byte ptr [eax + 0x4b5642], cl
0045150f  mov        dword ptr [eax + 0x4b563c], 0
00451519  mov        byte ptr [eax + 0x4b5644], cl

// block 0045151f
0045151f  pop        ecx
00451520  ret        
