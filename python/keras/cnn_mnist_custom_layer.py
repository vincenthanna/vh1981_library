import keras
from keras import models, layers
from keras import backend
from keras.utils.data_utils import get_file


def make_model(input_shape, num_classes, save_file_name=""):
    model = models.Sequential()
    '''
    Conv2D()
    padding = 'valid'(default):유효영역만 처리하므로 크기가 작아진다. 'same':출력크기가 동일해짐
    stride = (1,1)
    '''
    model.add(layers.Conv2D(filters=32, kernel_size=(3, 3), activation='relu', input_shape=input_shape))
    model.add(layers.Conv2D(filters=64, kernel_size=(3, 3), activation='relu'))
    model.add(layers.MaxPooling2D(pool_size=(2, 2)))
    model.add(layers.Dropout(0.25))
    model.add(layers.Flatten())
    model.add(layers.Dense(units=128, activation='relu', name="cut_here"))
    model.add(layers.Dropout(0.5))
    model.add(layers.Dense(num_classes, activation='softmax'))

    model.compile(loss=keras.losses.categorical_crossentropy, optimizer='rmsprop', metrics=['accuracy'])

    return model

from keras import datasets

class DATA():
    def __init__(self):
        num_classes = 10

        (x_train, y_train), (x_test, y_test) = datasets.mnist.load_data()
        rows, cols = x_train.shape[1:3] # (28, 28) 1부터 3-1까지

        print("rows :{}, cols :{}".format(rows, cols))

        # image_data_format에 따라 shape 재조정
        # 1은 channel임(mnist는 단색이므로)
        if backend.image_data_format() == 'channels_first':
            x_train = x_train.reshape(x_train.shape[0], 1, rows, cols)
            x_test = x_test.reshape(x_test.shape[0], 1, rows, cols)

            input_shape = (1, rows, cols)
        else:
            x_train = x_train.reshape(x_train.shape[0], rows, cols, 1)
            x_test = x_test.reshape(x_test.shape[0], rows, cols, 1)

            input_shape = (rows, cols, 1)

        x_train = x_train.astype('float32')
        x_test = x_test.astype('float32')
        x_train /= 255.0
        x_test /= 255.0

        y_train = keras.utils.to_categorical(y_train, num_classes)
        y_test = keras.utils.to_categorical(y_test, num_classes)

        self.input_shape = input_shape
        self.num_classes = num_classes
        self.x_train, self.y_train = x_train, y_train
        self.x_test, self.y_test = x_test, y_test


from utils.skeras import plot_acc, plot_loss
import matplotlib.pyplot as plt

from keras.models import model_from_json

def save_model(model, filename):
    # weight 저장:
    model.save_weights(filename + ".h5")

    # model 저장:
    model_json = model.to_json()
    with open(filename + ".json", mode="w") as file:
        file.write(model_json)

def load_model(filename):
    # model 로드:
    model_json = ""
    with open(filename + ".json", "r") as f:
        model_json = f.read()
    model = model_from_json(model_json)

    # weight 로드:
    model.load_weights(filename + ".h5")
    return model


def main():
    batch_size = 1024
    epochs = 1

    data = DATA()
    # model = make_model(input_shape=data.input_shape, num_classes=data.num_classes, save_file_name="cnn_mnist_model")
    #
    # history = model.fit(data.x_train, data.y_train, batch_size=batch_size, epochs=epochs, validation_split=0.2)
    #
    # score = model.evaluate(data.x_test, data.y_test)
    # print("Test Loss:", score[0])
    # print("Test Accuracy :", score[1])



    # model을 읽어온다.
    fname = "cnn_mnist_model"
    loaded_model = load_model(fname)

    # 원본 model 정보를 출력:
    print("source model : ")
    loaded_model.summary()
    print(loaded_model.get_layer('conv2d_1').get_weights()[1])

    # 'cut_here' 까지의 layer들로 모델을 생성
    new_model = models.Sequential()
    for layer in loaded_model.layers:
        print(layer.name)
        if layer.name != 'cut_here':
            new_model.add(layer)
        else:
            break
    new_model.add(loaded_model.get_layer('cut_here'))

    # 기존 layer들은 고정.
    for layer in new_model.layers:
        layer.trainable = False

    # 새 layer 추가.
    new_model.add(layers.Dropout(0.5, name='dropout_new_001'))
    new_model.add(layers.Dense(64, activation='relu'))
    new_model.add(layers.Dropout(0.5, name='dropout_new_002'))
    new_model.add(layers.Dense(10, activation='softmax'))

    print("new model : ")
    new_model.summary()

    # load한 모델은 compile해줘야 함.
    new_model.compile(loss=keras.losses.categorical_crossentropy, optimizer='rmsprop', metrics=['accuracy'])

    # 새 layer를 추가했으므로 추가된 것만 다시 training.
    history = new_model.fit(data.x_train, data.y_train, batch_size=batch_size, epochs=epochs, validation_split=0.2)

    print(new_model.get_layer('conv2d_1').get_weights()[1])

    score2 = new_model.evaluate(data.x_test, data.y_test)
    print("Loaded Model Test Loss:", score2[0])
    print("Loaded Model Test Accuracy :", score2[1])

if __name__ == '__main__':
    main()
