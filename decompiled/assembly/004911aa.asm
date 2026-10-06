
// block 004911aa
004911aa  mov        edi, edi
004911ac  push       ebp
004911ad  mov        ebp, esp
004911af  push       ecx
004911b0  push       ecx
004911b1  mov        cl, byte ptr [ebp + 8]
004911b4  test       cl, 1
004911b7  je         0x4911c3

// block 004911b9
004911b9  fld        xword ptr [0x4ae430]
004911bf  fistp      dword ptr [ebp + 8]
004911c2  wait       

// block 004911c3
004911c3  test       cl, 8
004911c6  je         0x4911d8

// block 004911c8
004911c8  wait       
004911c9  fnstsw     ax
004911cb  fld        xword ptr [0x4ae430]
004911d1  fstp       qword ptr [ebp - 8]
004911d4  wait       
004911d5  wait       
004911d6  fnstsw     ax

// block 004911d8
004911d8  test       cl, 0x10
004911db  je         0x4911e7

// block 004911dd
004911dd  fld        xword ptr [0x4ae43c]
004911e3  fstp       qword ptr [ebp - 8]
004911e6  wait       

// block 004911e7
004911e7  test       cl, 4
004911ea  je         0x4911f5

// block 004911ec
004911ec  fldz       
004911ee  fld1       
004911f0  fdivrp     st(1)
004911f2  fstp       st(0)
004911f4  wait       

// block 004911f5
004911f5  test       cl, 0x20
004911f8  je         0x491200

// block 004911fa
004911fa  fldpi      
004911fc  fstp       qword ptr [ebp - 8]
004911ff  wait       

// block 00491200
00491200  leave      
00491201  ret        
