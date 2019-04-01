import numpy as np
import matplotlib.pyplot as plt

def sample_data_check(imgs, labels):
    '''
    데이터가 제대로 읽혀졌는지 테스트하는 함수
    :param imgs: [num, 28*28] 형태의 데이터
    :param labels: 이미지 label(one-hot)
    :return: none.
    '''
    print("sample_data_check:", imgs.shape, labels.shape)
    num_imgs = imgs.shape[0]
    imgs = imgs.reshape(-1, 28, 28)
    print("sample_data_check:", imgs.shape, labels.shape)
    cols = rows = int(math.sqrt(num_imgs))
    for i in range(0, rows * cols):
        plt.subplot(rows, cols, i + 1)
        plt.xlabel("")
        plt.title(np.argmax(labels[i]))
        plt.imshow(imgs[i])

    plt.show()
