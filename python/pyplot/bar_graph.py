import numpy as np
import matplotlib.pyplot as plt

features = ["A", "B", "C", "D", "E", "F", "G"]
datas = [10, 4, 8, 7, 18, 8, 3]
def plot_bar_h_example(features, data):
    n_features = len(features)
    plt.barh(range(n_features), data, align='center')
    plt.yticks(np.arange(n_features), features)
    plt.ylim(-1, n_features)

plot_bar_h_example(features, datas)

plt.show()