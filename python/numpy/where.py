import numpy as np

a = np.arange(5, 10)
print("a = ", a)

# 아래 코드는 a < 8인 a 배열 인덱스를 리턴한다.
print(np.where(a < 8)) # [0,1,2]
print(a[np.where(a < 8)]) # [5, 6, 7] 인덱스 배열로 인덱싱하면 조건에 맞는 값들을 얻을 수 있다.

# 아래 코드에서 where는 조건을 충족하는 
a = np.arange(4, 13).reshape(3, 3)
print("a=", a)
print("np.where(a > 8) : ", np.where(a > 8))
print(a[np.where(a > 8)])
