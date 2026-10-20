
// block 004508b0
004508b0  sub        esp, 0x14
004508b3  test       dword ptr [0x4cee78], 0x8000
004508bd  je         0x4508d0

// block 004508bf
004508bf  push       0x4cf170
004508c4  call       dword ptr [0x498088]

// block 004508ca
004508ca  inc        byte ptr [0x4cf21d]

// block 004508d0
004508d0  mov        eax, dword ptr [0x4cf408]
004508d5  or         eax, dword ptr [0x4cf40c]
004508db  je         0x450950

// block 004508dd
004508dd  lea        ecx, [esp + 8]
004508e1  push       ecx
004508e2  call       dword ptr [0x4980d4]

// block 004508e8
004508e8  mov        edx, dword ptr [esp + 8]
004508ec  sub        edx, dword ptr [0x4cf410]
004508f2  mov        eax, dword ptr [esp + 0xc]
004508f6  sbb        eax, dword ptr [0x4cf414]
004508fc  mov        dword ptr [esp], edx
004508ff  mov        dword ptr [esp + 4], eax
00450903  fild       qword ptr [esp]
00450906  fild       qword ptr [0x4cf408]
0045090c  fdivp      st(1)
0045090e  fst        qword ptr [esp]
00450911  fcom       qword ptr [0x4cf448]
00450917  fnstsw     ax
00450919  test       ah, 5
0045091c  jp         0x450924

// block 0045091e
0045091e  fst        qword ptr [0x4cf448]

// block 00450924
00450924  test       dword ptr [0x4cee78], 0x8000
0045092e  je         0x450946

// block 00450930
00450930  push       0x4cf170
00450935  fstp       st(0)
00450937  call       dword ptr [0x49808c]

// block 0045093d
0045093d  fld        qword ptr [esp]
00450940  dec        byte ptr [0x4cf21d]

// block 00450946
00450946  fsub       qword ptr [0x4cf448]
0045094c  add        esp, 0x14
0045094f  ret        

// block 00450950
00450950  call       dword ptr [0x498298]

// block 00450956
00450956  mov        dword ptr [esp], eax
00450959  fild       dword ptr [esp]
0045095c  test       eax, eax
0045095e  jge        0x450966

// block 00450960
00450960  fadd       qword ptr [0x4a3d38]

// block 00450966
00450966  fld        qword ptr [0x4cf448]
0045096c  fcom       st(1)
0045096e  fnstsw     ax
00450970  test       ah, 0x41
00450973  jne        0x45097f

// block 00450975
00450975  fstp       st(0)
00450977  fld        st(0)
00450979  fst        qword ptr [0x4cf448]

// block 0045097f
0045097f  test       dword ptr [0x4cee78], 0x8000
00450989  fld        qword ptr [0x4a3d30]
0045098f  fmul       st(1), st(0)
00450991  fxch       st(2)
00450993  fsubrp     st(1)
00450995  fdivrp     st(1)
00450997  fstp       qword ptr [esp]
0045099a  je         0x4509ad

// block 0045099c
0045099c  push       0x4cf170
004509a1  call       dword ptr [0x49808c]

// block 004509a7
004509a7  dec        byte ptr [0x4cf21d]

// block 004509ad
004509ad  fld        qword ptr [esp]
004509b0  add        esp, 0x14
004509b3  ret        
