import numpy as np

a = np.array([0,1,2,3,4])
print(a) # [0 1 2 3 4] 1행 5열

a = a.reshape(-1, 1)
print(a) # [[0] [1] [2] [3] [4]] 5행 1열

a = np.array([[0, 1], [2, 3], [4, 5]])
a = a.reshape(2, 3)
print(a) # [[0 1 2] [3 4 5]]
a = a.reshape(-1, 2) # 열을 2로 변환
print(a) # [[0 1] [2 3] [4 5]]
a = a.reshape(-1, 1)
print(a) # [[0] [1] [2] [3] [4]]
