
// block 004967ef
004967ef  mov        edi, edi
004967f1  push       ebp
004967f2  mov        ebp, esp
004967f4  push       ecx
004967f5  push       ecx
004967f6  cmp        dword ptr [0x4b37a0], 0
004967fd  fld        qword ptr [ebp + 0xc]
00496800  fadd       qword ptr [ebp + 0x14]
00496803  fstp       qword ptr [ebp - 8]
00496806  jne        0x496831

// block 00496808
00496808  push       dword ptr [ebp + 0x1c]
0049680b  fld        qword ptr [ebp - 8]
0049680e  sub        esp, 0x18
00496811  fstp       qword ptr [esp + 0x10]
00496815  fld        qword ptr [ebp + 0x14]
00496818  fstp       qword ptr [esp + 8]
0049681c  fld        qword ptr [ebp + 0xc]
0049681f  fstp       qword ptr [esp]
00496822  push       dword ptr [ebp + 8]
00496825  push       1
00496827  call       0x4966fa

// block 0049682c
0049682c  add        esp, 0x24
0049682f  leave      
00496830  ret        

// block 00496831
00496831  call       0x46e96f

// block 00496836
00496836  push       0xffff
0049683b  push       dword ptr [ebp + 0x1c]
0049683e  mov        dword ptr [eax], 0x21
00496844  call       0x491181

// block 00496849
00496849  fld        qword ptr [ebp - 8]
0049684c  pop        ecx
0049684d  pop        ecx
0049684e  leave      
0049684f  ret        
