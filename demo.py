
import os
os.add_dll_directory(r'C:\Program Files\mingw64\bin')
os.add_dll_directory(r'C:\Users\cesar\AppData\Local\Programs\Python\Python314')

import matrix1_api_c as mx

m = mx.Matrix_F(3, 3)
m[0, 0] = 1.0
m[1, 1] = 5.0
m[2, 2] = 9.0
print(m)

t    = m.transpose()
ref  = m.row_echelon()
rref = m.row_echelon(True)
scaled = m.mul_scalar(3.0)

print(t)
print(rref)

m[2, 2] = 8.0
print("m con [2,2]=8:")
print(m)

# ── m5 = 5 * m2 + m3 * m4 ───────────────────────────────
m2 = mx.Matrix_I("m2.txt")   # leer desde archivo
m3 = mx.Matrix_I("m3.txt")
m4 = mx.Matrix_I("m4.txt")

m5 = m2.mul_scalar(5) + m3 * m4   # 5*m2 + m3*m4
print("m5 = 5*m2 + m3*m4:")
print(m5)

m6 = m5 - m2
print("m6 = m5 - m2:")
print(m6)