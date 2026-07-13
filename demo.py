
import os
os.add_dll_directory(r'C:\Program Files\mingw64\bin')
os.add_dll_directory(r'C:\Users\cesar\AppData\Local\Programs\Python\Python314')

import matrix1_api_c as mx

m = mx.Matrix_F(3, 3)
m[0, 0] = 1.0
m[1, 1] = 5.2
m[2, 2] = 9.4
print(m)

t    = m.transpose()
scaled = m.mul_scalar(3.0)

print(t)
print(scaled)

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

ref  = m5.row_echelon()
print("ref = m5.row_echelon():")
print(ref)
rref = m6.row_echelon(True)
print("rref = m6.row_echelon(True):")
print(rref)

m7 = 5 * m2       # __rmul__
print("m7 = 5*m2:")
print(m7)
m8 = m * 5.1       # __mul__ con escalar
print("m8 = m*5.1:")
print(m8)   
m9 = m8 * m      # __mul__ con matriz
print("m9 = m8*m:")
print(m9)

print("Determinante de m7:", m7.det())
print("Determinante de m8:", m8.det())