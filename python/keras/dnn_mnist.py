nin = 784
nh_1 = [100, 50]
num_classes = 10
nout = num_classes

from keras import layers, models

class DNN(models.Sequential):
    def __init__(self, nin, nh_1, nout):
        super().__init__()
        self.add(layers.Dense(nh_1[0], activation='relu', input_shape=(nin, ), name='hidden_1'))
        self.add(layers.Dense(nh_1[1], activation='relu', name='hidden_2'))
        self.add(layers.Dense(nout, activation='softmax'))
        self.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])


# 데이터 준비
import numpy as np
from keras import datasets
from keras.utils import np_utils
(X_train, y_train), (X_test, y_test) = datasets.mnist.load_data()
y_train = np_utils.to_categorical(y_train)
y_test = np_utils.to_categorical(y_test)
L, W, H = X_train.shape
X_train = X_train.reshape(-1, W * H)
X_test = X_test.reshape(-1, W * H)
X_train = X_train / 255.0
X_test = X_test / 255.0

model = DNN(nin, nh_1, nout)
history = model.fit(X_train, y_train, epochs=10, batch_size=100, validation_split=0.2)
performance_test = model.evaluate(X_test, y_test, batch_size=100)
print("Test Loss And Accuracy ->", performance_test)

from utils.skeras import plot_loss
from utils.skeras import plot_acc

#plot_loss(history)
plot_acc(history)

import matplotlib.pyplot as plt
plt.show()