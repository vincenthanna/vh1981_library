from keras import layers, models

num_classes = 10

# 분산 방식 모델링을 포함하는 함수형 구현
def ann_models_func(nin, nh, nout):
    x = layers.Input(shape=(nin,))
    h = layers.Activation('relu')(layers.Dense(nh)(nin))
    y = layers.Activation('softmax')(layers.Dense(nout)(h))
    model = models.Model(x, y)
    model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])

    return model


# 연쇄 방식 모델링을 포함하는 함수형 구현
def ann_seq_func(nin, nh, nout):
    model = models.Sequential()
    model.add(layers.Dense(nh, activation='relu', input_shape=(nin, )))
    model.add(layers.Dense(nout, activation='softmax'))
    model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])

    return model


# 분산 방식 모델링을 포함하는 객체지향형 구현
class ann_models_class(models.Model):
    def __init__(self, nin, nh, nout):
        # prepare network layers & activation functions
        hidden = layers.Dense(nh)
        output = layers.Dense(nout)
        relu = layers.Activation('relu')
        softmax = layers.Activation('softmax')

        # connect network elements
        x = layers.Input(shape=(nin,))
        h = relu(hidden(x))
        y = softmax(output(h))

        super().__init__(x, y)
        self.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])


# 연쇄 방식 모델링을 사용하는 객체지향형 구현
class ann_seq_class(models.Sequential):
    def __init__(self, nin, nh, nout):
        super().__init__()
        self.add(layers.Dense(nh, activation='relu', input_shape=(nin, )))
        self.add(layers.Dense(nout, activation='softmax'))
        self.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])


# 분류 ann에 사용할 데이터 불러 오기
import numpy as np
from keras import datasets
from keras.utils import np_utils


def get_data():
    # numpy array 타입으로 데이터를 받음
    (X_train, y_train), (X_test, y_test) = datasets.mnist.load_data()

    print("X_train.shape", X_train.shape)
    print("y_train.shape", y_train.shape)

    '''
    one-hot 처리를 한다.
    num_classes를 지정하지 않아도 되는데 없으면 자동으로 계산해 준다.
        num_classes = np.max(y) + 1
    식을 보면 지정해 주는 것이 좋을 듯 싶다.
    '''
    Y_train = np_utils.to_categorical(y_train, num_classes=num_classes)
    Y_test = np_utils.to_categorical(y_test, num_classes=num_classes)

    print("to_categorical(y_train).shape", Y_train.shape)

    # (?, 28, 28)을 (?, 784)로 변경한다.
    L, W, H = X_train.shape
    X_train = X_train.reshape(-1, W * H)
    X_test = X_test.reshape(-1, W * H)

    # 값을 0~1사이로 변경한다.
    X_train = X_train / 255.0
    X_test = X_test / 255.0

    return (X_train, Y_train), (X_test, Y_test)

import matplotlib.pyplot as plt


def plot_loss(history):
    # summarize history for loss
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('Model Loss')
    plt.ylabel('Loss')
    plt.xlabel('epoch')
    plt.legend(['train', ['test']], loc=0)

def plot_acc(history):
    # summarize history for accuracy
    plt.plot(history.history['acc'])
    plt.plot(history.history['val_acc'])
    plt.title('Model Accuracy')
    plt.ylabel('Accuracy')
    plt.xlabel('Epoch')
    plt.legend(['Train', 'Test'], loc=0)

def main():
    nin = 784
    nh = 100
    num_classes = 10
    nout = num_classes

    model = ann_seq_class(nin, nh, nout)
    (X_train, Y_train), (X_test, Y_test) = get_data() # (?, 784), (?, 10)

    # training
    history = model.fit(X_train, Y_train, epochs=15, batch_size=100, validation_split=0.2)
    performance_test = model.evaluate(X_test, Y_test, batch_size=100)
    print("Test Loss and Accuracy ->", performance_test)

    plot_loss(history)
    plt.show()
    plot_acc(history)
    plt.show()

# run code
if __name__ == '__main__':
    main()