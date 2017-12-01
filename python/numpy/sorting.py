import numpy as np

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