
// block 004969f1
004969f1  mov        edi, edi
004969f3  push       ebp
004969f4  mov        ebp, esp
004969f6  push       ecx
004969f7  push       ecx
004969f8  fld        qword ptr [ebp + 8]
004969fb  frndint    
004969fd  fstp       qword ptr [ebp - 8]
00496a00  fld        qword ptr [ebp - 8]
00496a03  leave      
00496a04  ret        
