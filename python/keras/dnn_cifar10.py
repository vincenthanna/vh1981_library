import numpy as np
from keras import datasets
from keras.utils import np_utils

def load_data():
    (X_train, y_train), (X_test, y_test) = datasets.cifar10.load_data()
    print("X_train.shape=", X_train.shape, " y_train.shape=", y_train.shape)
    Y_train = np_utils.to_categorical(y_train)
    Y_test = np_utils.to_categorical(y_test)

    L, W, H, C = X_train.shape
    X_train = X_train.reshape(-1, W*H*C)
    X_test = X_test.reshape(-1, W*H*C)

    X_train = X_train / 255.0
    X_test = X_test / 255.0

    return (X_train, Y_train), (X_test, Y_test)


from keras import layers, models

class DNN(models.Sequential):
    def __init__(self, nin, nh_1, pd_1, nout):
        super().__init__()

        self.add(layers.Dense(nh_1[0], activation='relu', input_shape=(nin,), name="hidden-1")) # hidden 100 with activation func.
        self.add(layers.Dropout(pd_1[0])) # add dropout

        self.add(layers.Dense(nh_1[1], activation='relu', name='hidden-2')) #
        self.add(layers.Dropout(pd_1[1]))

        self.add(layers.Dense(nout, activation='softmax'))

        self.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])


# 학습 효과 분석
from utils.skeras import plot_acc, plot_loss
import matplotlib.pyplot as plt

# 학습 및 성능 평가
def main():
    nh_1 = [100, 50]
    pd_1 = [0.0, 0.0]
    num_classes = 10
    nout = num_classes

    (X_train, y_train), (X_test, y_test) = load_data()
    model = DNN(X_train.shape[1], nh_1, pd_1, nout)
    history = model.fit(X_train, y_train, epochs=10, batch_size=100, validation_split=0.2)
    performance_test = model.evaluate(X_test, y_test, batch_size=100)
    print("Test Loss and Accuracy ->", performance_test)
    plot_acc(history)
    plt.show()
    plot_loss(history)
    plt.show()


if __name__ == '__main__':
    main()