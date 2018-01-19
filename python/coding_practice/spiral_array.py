'''
문제는 다음과 같다:

6 6

  0   1   2   3   4   5
 19  20  21  22  23   6
 18  31  32  33  24   7
 17  30  35  34  25   8
 16  29  28  27  26   9
 15  14  13  12  11  10
위처럼 6 6이라는 입력을 주면 6 X 6 매트릭스에 나선형 회전을 한 값을 출력해야 한다.

'''
def spiral_array(rowsize, colsize):
    vecoffset = [(0, 1), (1, 0), (0, -1), (-1, 0)]
    arr = [[-1 for j in range(colsize)] for i in range(rowsize)]
    vec = 0
    val = 0
    row, col = 0, 0
    arr[row][col] = val
    while val < rowsize * colsize - 1:
        r, c = row + vecoffset[vec][0], col + vecoffset[vec][1]
        if r >= 0 and r < rowsize and c >= 0 and c < colsize and arr[r][c] == -1:
            val += 1
            row, col = r, c
            arr[row][col] = val
        else:
            vec = (vec + 1) % 4

    return arr


evals = [{"row": 6, "col": 6,
          "ret": [[0, 1, 2, 3, 4, 5],
                  [19, 20, 21, 22, 23, 6],
                  [18, 31, 32, 33, 24, 7],
                  [17, 30, 35, 34, 25, 8],
                  [16, 29, 28, 27, 26, 9],
                  [15, 14, 13, 12, 11, 10]]},
         {"row": 3, "col": 3,
          "ret": [[0, 1, 2],
                  [7, 8, 3],
                  [6, 5, 4]]},
         {"row": 4, "col": 2,
          "ret": [[0, 1],
                  [7, 2],
                  [6, 3],
                  [5, 4]]},
        {"row": 4, "col": 4,
          "ret": [[0, 1, 2, 3],
                  [11, 12, 13, 4],
                  [10, 15, 14, 5],
                  [9, 8, 7, 6]]}
         ]

def printArr(row, col, arr):
    str = ""
    for i in range(row):
        for j in arr[i]:
            str += "{:02d} ".format(j)
        str += "\n"
    return str

for eval in evals:
    ret = spiral_array(eval["row"], eval["col"])
    assert(ret == eval["ret"])
    print(printArr(eval["row"], eval["col"], ret))







