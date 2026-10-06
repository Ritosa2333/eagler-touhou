
// block 004daee1
004daee1  push       esi
004daee2  out        0x22, al
004daee4  pop        ecx
004daee5  clc        
004daee6  fadd       qword ptr [esi + 0x78]
004daee9  sahf       

// block 004daeea
004daeea  ljmp       0x3e77:0x7be382a9
