import numpy as np

a = np.arange(5, 10)
print("a = ", a)

# 아래 코드는 a < 8인 a 배열 인덱스를 리턴한다.
print(np.where(a < 8)) # [0,1,2]
print(a[np.where(a < 8)]) # [5, 6, 7] 인덱스 배열로 인덱싱하면 조건에 맞는 값들을 얻을 수 있다.

# 아래 코드에서 where는 조건을 충족하는 인덱스 배열을 리턴한다.
a = np.arange(4, 13).reshape(3, 3)
print("a=", a)
print("np.where(a > 8) : ", np.where(a > 8)) # 조건을 충족하는 값의 행/열 인덱스 목록을 전달해 준다.
print(a[np.where(a > 8)]) # 이 코드로 where 조건을 충족하는 것을 얻을 수 있다.

a = np.array([[1,2,3],
            [4,5,6],
            [2,3,4],
            [1,4,5],
            [3,4,7],
            [7,8,2],
            [9,8,1]])
            
'''
정렬 : 
numpy.argsort(배열의 정렬 기준축을 받아 정렬된 배열의 인덱스 배열을 리턴한다.
'''
indexes = np.argsort(a[:, 1])
for i in indexes:
    print("index=", i, "value=", a[i][1])

'''
[::-1] 현재의 배열을 뒤집에서 돌려준다.
'''
print("normal : ", indexes)
print("reversed: ", indexes[::-1]) #지금 배열을 거꾸로 뒤집어서 돌려준다.