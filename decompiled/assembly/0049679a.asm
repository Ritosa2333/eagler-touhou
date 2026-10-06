
// block 0049679a
0049679a  mov        edi, edi
0049679c  push       ebp
0049679d  mov        ebp, esp
0049679f  cmp        dword ptr [0x4b37a0], 0
004967a6  jne        0x4967d0

// block 004967a8
004967a8  push       dword ptr [ebp + 0x14]
004967ab  fld        qword ptr [ebp + 0xc]
004967ae  sub        esp, 0x18
004967b1  fstp       qword ptr [esp + 0x10]
004967b5  fldz       
004967b7  fstp       qword ptr [esp + 8]
004967bb  fld        qword ptr [ebp + 0xc]
004967be  fstp       qword ptr [esp]
004967c1  push       dword ptr [ebp + 8]
004967c4  push       1
004967c6  call       0x4966fa

// block 004967cb
004967cb  add        esp, 0x24
004967ce  pop        ebp
004967cf  ret        

// block 004967d0
004967d0  call       0x46e96f

// block 004967d5
004967d5  push       0xffff
004967da  push       dword ptr [ebp + 0x14]
004967dd  mov        dword ptr [eax], 0x21
004967e3  call       0x491181

// block 004967e8
004967e8  fld        qword ptr [ebp + 0xc]
004967eb  pop        ecx
004967ec  pop        ecx
004967ed  pop        ebp
004967ee  ret        
