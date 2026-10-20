
// block 00483f5e
00483f5e  mov        edi, edi
00483f60  push       ebp
00483f61  mov        ebp, esp
00483f63  sub        esp, 0x18
00483f66  push       ebx
00483f67  mov        ebx, dword ptr [ebp + 8]
00483f6a  push       esi
00483f6b  xor        esi, esi
00483f6d  push       edi
00483f6e  mov        dword ptr [ebp - 0x18], ebx
00483f71  mov        dword ptr [ebp - 0x14], esi
00483f74  cmp        dword ptr [ebx + 0x1c], esi
00483f77  jne        0x483f90

// block 00483f79
00483f79  cmp        dword ptr [ebx + 0x18], esi
00483f7c  jne        0x483f90

// block 00483f7e
00483f7e  mov        dword ptr [ebp - 4], esi
00483f81  mov        dword ptr [ebp - 8], esi
00483f84  mov        dword ptr [ebp + 8], 0x4adf68
00483f8b  jmp        0x4840ca

// block 00483f90
00483f90  push       0x30
00483f92  push       1
00483f94  call       0x477813

// block 00483f99
00483f99  mov        edi, eax
00483f9b  pop        ecx
00483f9c  pop        ecx
00483f9d  mov        dword ptr [ebp + 8], edi
00483fa0  cmp        edi, esi
00483fa2  jne        0x483fac

// block 00483fa4
00483fa4  xor        eax, eax
00483fa6  inc        eax
00483fa7  jmp        0x484123

// block 00483fac
00483fac  mov        esi, dword ptr [ebx + 0xbc]
00483fb2  push       0xc
00483fb4  pop        ecx
00483fb5  push       4

// block 00483fb7
00483fb7  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00483fb9
00483fb9  call       0x4777ce

// block 00483fbe
00483fbe  xor        esi, esi
00483fc0  pop        ecx
00483fc1  mov        dword ptr [ebp - 8], eax
00483fc4  cmp        eax, esi
00483fc6  jne        0x483fd3

// block 00483fc8
00483fc8  push       dword ptr [ebp + 8]
00483fcb  call       0x46c941

// block 00483fd0
00483fd0  pop        ecx
00483fd1  jmp        0x483fa4

// block 00483fd3
00483fd3  mov        dword ptr [eax], esi
00483fd5  cmp        dword ptr [ebx + 0x1c], esi
00483fd8  je         0x484099

// block 00483fde
00483fde  push       4
00483fe0  call       0x4777ce

// block 00483fe5
00483fe5  pop        ecx
00483fe6  mov        dword ptr [ebp - 4], eax
00483fe9  cmp        eax, esi
00483feb  jne        0x484009

// block 00483fed
00483fed  xor        esi, esi
00483fef  inc        esi

// block 00483ff0
00483ff0  push       dword ptr [ebp + 8]
00483ff3  call       0x46c941

// block 00483ff8
00483ff8  push       dword ptr [ebp - 8]
00483ffb  call       0x46c941

// block 00484000
00484000  pop        ecx
00484001  pop        ecx
00484002  mov        eax, esi
00484004  jmp        0x484123

// block 00484009
00484009  mov        dword ptr [eax], esi
0048400b  mov        esi, dword ptr [ebp + 8]
0048400e  movzx      edi, word ptr [ebx + 0x3e]
00484012  push       esi
00484013  push       0xe
00484015  push       edi
00484016  lea        eax, [ebp - 0x18]
00484019  push       1
0048401b  push       eax
0048401c  call       0x47a12b

// block 00484021
00484021  mov        dword ptr [ebp - 0xc], eax
00484024  lea        eax, [esi + 4]
00484027  push       eax
00484028  push       0xf
0048402a  push       edi
0048402b  lea        eax, [ebp - 0x18]
0048402e  push       1
00484030  push       eax
00484031  call       0x47a12b

// block 00484036
00484036  or         dword ptr [ebp - 0xc], eax
00484039  lea        eax, [esi + 8]
0048403c  push       eax
0048403d  push       0x10
0048403f  push       edi
00484040  mov        dword ptr [ebp - 0x10], eax
00484043  lea        eax, [ebp - 0x18]
00484046  push       1
00484048  push       eax
00484049  call       0x47a12b

// block 0048404e
0048404e  add        esp, 0x3c
00484051  or         eax, dword ptr [ebp - 0xc]
00484054  je         0x484062

// block 00484056
00484056  push       esi
00484057  call       0x483f19

// block 0048405c
0048405c  pop        ecx
0048405d  or         esi, 0xffffffff
00484060  jmp        0x483ff0

// block 00484062
00484062  mov        eax, dword ptr [ebp - 0x10]
00484065  mov        eax, dword ptr [eax]
00484067  jmp        0x48407b

// block 00484069
00484069  mov        cl, byte ptr [eax]
0048406b  cmp        cl, 0x30
0048406e  jl         0x484082

// block 00484070
00484070  cmp        cl, 0x39
00484073  jg         0x484082

// block 00484075
00484075  sub        cl, 0x30
00484078  mov        byte ptr [eax], cl

// block 0048407a
0048407a  inc        eax

// block 0048407b
0048407b  cmp        byte ptr [eax], 0
0048407e  jne        0x484069

// block 00484080
00484080  jmp        0x4840b9

// block 00484082
00484082  cmp        cl, 0x3b
00484085  jne        0x48407a

// block 00484087
00484087  mov        esi, eax

// block 00484089
00484089  lea        edi, [esi + 1]
0048408c  mov        cl, byte ptr [edi]
0048408e  mov        byte ptr [esi], cl
00484090  mov        esi, edi
00484092  cmp        byte ptr [esi], 0
00484095  jne        0x484089

// block 00484097
00484097  jmp        0x48407b

// block 00484099
00484099  mov        ecx, dword ptr [0x4adf68]
0048409f  mov        eax, dword ptr [ebp + 8]
004840a2  mov        dword ptr [eax], ecx
004840a4  mov        ecx, dword ptr [0x4adf6c]
004840aa  mov        dword ptr [eax + 4], ecx
004840ad  mov        ecx, dword ptr [0x4adf70]
004840b3  mov        dword ptr [ebp - 4], esi
004840b6  mov        dword ptr [eax + 8], ecx

// block 004840b9
004840b9  mov        eax, dword ptr [ebp - 8]
004840bc  xor        ecx, ecx
004840be  inc        ecx
004840bf  mov        dword ptr [eax], ecx
004840c1  mov        eax, dword ptr [ebp - 4]
004840c4  test       eax, eax
004840c6  je         0x4840ca

// block 004840c8
004840c8  mov        dword ptr [eax], ecx

// block 004840ca
004840ca  mov        eax, dword ptr [ebx + 0xb4]
004840d0  mov        esi, dword ptr [0x49815c]
004840d6  test       eax, eax
004840d8  je         0x4840dd

// block 004840da
004840da  push       eax
004840db  call       esi

// block 004840dd
004840dd  mov        eax, dword ptr [ebx + 0xb0]
004840e3  test       eax, eax
004840e5  je         0x484106

// block 004840e7
004840e7  push       eax
004840e8  call       esi

// block 004840ea
004840ea  test       eax, eax
004840ec  jne        0x484106

// block 004840ee
004840ee  push       dword ptr [ebx + 0xb0]
004840f4  call       0x46c941

// block 004840f9
004840f9  push       dword ptr [ebx + 0xbc]
004840ff  call       0x46c941

// block 00484104
00484104  pop        ecx
00484105  pop        ecx

// block 00484106
00484106  mov        eax, dword ptr [ebp - 4]
00484109  mov        dword ptr [ebx + 0xb4], eax
0048410f  mov        eax, dword ptr [ebp - 8]
00484112  mov        dword ptr [ebx + 0xb0], eax
00484118  mov        eax, dword ptr [ebp + 8]
0048411b  mov        dword ptr [ebx + 0xbc], eax
00484121  xor        eax, eax

// block 00484123
00484123  pop        edi
00484124  pop        esi
00484125  pop        ebx
00484126  leave      
00484127  ret        
