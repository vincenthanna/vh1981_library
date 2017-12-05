from sklearn.cluster import AgglomerativeClustering
from sklearn.datasets import make_blobs
import matplotlib.pyplot as plt
import numpy as np

X, T = make_blobs(random_state = 1)
agg = AgglomerativeClustering(n_clusters=3)
assignment = agg.fit_predict(X)

x1 = X[:, 0]
x2 = X[:, 1]
ax = plt.gca()
y = assignment

def colors(i):
    return "#" + format(int(0xffffff / 12) * i, '06x')

def plt_draw_scatters(x1, x2, ax, y):
    '''    
    :param x1: x축 데이터들. 
    :param x2: y축 데이터들.
    :param ax: 그래프가 출력될 axe.
    :param y: marker로 표시될 데이터값. unique한 값으로 분류될수 있어야 한다.
    :return: None. 
    '''
    
    unique_y = np.unique(y) # marker로 표시될 값을 카테고리화 한다.
    markers = ['o', '^', 'v', 'D', 's', '*', 'p', 'h', 'H', '8', '<', '>']

    # 카테고리화된 y값마다 다른 marker로 차트에 뿌려준다.
    for i, (typeY) in enumerate(unique_y):        
        mask = (y == typeY) # 현재 카테고리와 같은거만 표시해야 하니 추출에 사용할 mask를 생성.
        color = colors(i % 10)
        ax.plot(x1[mask], x2[mask], markers[i%10], c=color)

    # 좌표계 min/max 재조정(padding offset도 넣는다.)
    xlim = ax.get_xlim()
    ylim = ax.get_ylim()
    x1pad = (x1.max() - x1.min()) / 8
    x2pad = (x2.max() - x2.min()) / 8
    ax.set_xlim(x1.min() - x1pad, x1.max() + x1pad)
    ax.set_ylim(x2.min() - x2pad, x2.max() + x2pad)

plt_draw_scatters(x1, x2, ax, y)

plt.show()
