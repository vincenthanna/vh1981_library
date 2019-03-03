'''
pyplot tutorial

아래 내용에 내용 추가중:
https://matplotlib.org/tutorials/introductory/pyplot.html
'''

import numpy as np
import matplotlib.pyplot as plt

'''
라인 그래프
'''
def pyplot_line():

    X = [1,2,3,4,5]
    Y = [2,3,4,5,6]

    plt.plot(X, Y)
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.show()

'''
점 그래프
'''
def pyplot_dot():
    X = [1, 2, 3, 4, 5]
    Y = [2, 3, 4, 5, 6]

    plt.plot(X, Y, 'ro')
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.show()

'''
numpy로 여러 값을 생성해서 넣는 예
'''
def pyplot_numpy_range():
    X = np.arange(0.0, 10.0, 0.1)
    Y = []
    for x in X:
        Y.append(x * x)

    plt.plot(X, Y)
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.show()

'''
subplot, bar, scatter 예제
'''
def pyplot_category():

    names = ["a", "b", "c"]
    values = [1.0, 10.0, 100.0]

    '''
    figure는 화면 하나를 말한다.
    subplot은 figure에 속한 그래프 하나를 말한다.
    '''
    plt.figure(1, figsize=(9, 5))

    plt.subplot(1, 3, 1)
    x = np.arange(len(values))
    print(x)
    plt.xticks(x, names)
    '''
    x, y 모두 숫자값이 와야 한다.
    x label에 문자열 값을 매기는 것은 xticks를 사용한다.
    '''
    plt.bar(x, values)

    plt.subplot(1, 3, 2)
    plt.xticks(x, names)
    plt.scatter(x, values)

    plt.subplot(1, 3, 3)
    plt.xticks(x, names)
    plt.plot(x, values)
    plt.suptitle('categorical plotting')
    plt.show()

def pyplot_line_property():
    X = [1,2,3,4,5]
    Y = [1,4,9,16,25]

    '''
    plot()은 그래프화 된 데이터 객체(Line2D)를 리턴한다.
    Line2D 값을 받아서 속성 변경에 사용할 수 있다.
    '''
    line, = plt.plot(X, Y)
    line.set_antialiased(False)
    plt.setp(line, 'color', 'r', 'linewidth', 2.0)
    plt.setp(line, 'aa', True)
    plt.setp(line, 'label', 'label string')
    #plt.setp(line, 'ls', 'steps') # 계단형 그래프가 된다.


    plt.xlabel("X")
    plt.ylabel("Y")
    plt.show()

def pyplot_multiline():
    X = [0, 1, 2, 3, 4, 5]
    Y1 = [0, 1, 2, 3, 4, 5]
    Y2 = [0, 1, 4, 9, 16, 25]

    plt.plot(X, Y1, X, Y2)

    plt.xlabel("X")
    plt.ylabel("Y")
    plt.show()

def pyplot_labels():
    mu, sigma = 100, 15
    x = mu + sigma * np.random.rand(10000)
    print(len(x))
    '''
    matplotlib.pyplot.hist(x, bins=None, range=None, density=None, weights=None, cumulative=False, 
        bottom=None, histtype='bar', align='mid', orientation='vertical', rwidth=None, 
        log=False, color=None, label=None, stacked=False, normed=None, *, data=None, **kwargs)
    bins: 막대 그래프 수
    density: True이면 y값이 전체 수에 대한 비율로 변경되고 return값 첫번째도 비율값 목록으로 으로 리턴된다.
        (normalized 된 값이 된다.) 
    facecolor : 그래프 색
    
    '''
    n, bins, patches = plt.hist(x, bins=10, density=True, facecolor='r', alpha=0.75)
    print(n)
    plt.show()


pyplot_line()
pyplot_dot()
pyplot_numpy_range()
pyplot_category()
pyplot_line_property()
pyplot_multiline()
pyplot_labels()