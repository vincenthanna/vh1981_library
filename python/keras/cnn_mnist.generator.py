import keras
from keras import models, layers
from keras import backend
from keras.preprocessing.image import ImageDataGenerator
from random import randint


class CNN(models.Sequential):
    def __init__(self, input_shape, num_classes):
        super().__init__()
        '''
        Conv2D()
        padding = 'valid'(default):유효영역만 처리하므로 크기가 작아진다. 'same':출력크기가 동일해짐
        stride = (1,1)
        '''
        self.add(layers.Conv2D(filters=32, kernel_size=(3, 3), activation='relu', input_shape=input_shape))
        self.add(layers.Conv2D(filters=64, kernel_size=(3, 3), activation='relu'))
        self.add(layers.MaxPooling2D(pool_size=(2, 2)))
        self.add(layers.Dropout(0.25))
        self.add(layers.Flatten())
        self.add(layers.Dense(units=128, activation='relu'))
        self.add(layers.Dropout(0.5))
        self.add(layers.Dense(num_classes, activation='softmax'))

        self.compile(loss=keras.losses.categorical_crossentropy, optimizer='rmsprop', metrics=['accuracy'])


from keras import datasets


class DATA():
    def __init__(self):
        num_classes = 10

        (x_train, y_train), (x_test, y_test) = datasets.mnist.load_data()
        rows, cols = x_train.shape[1:3]  # (28, 28) 1부터 3-1까지

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
    batch_size = 128
    epochs = 10

    data = DATA()
    model = CNN(input_shape=data.input_shape, num_classes=data.num_classes)

    generator = ImageDataGenerator(
        rotation_range=15,
        rescale=1.0 / 255,
        shear_range=0.2,
        zoom_range=0.2)
    generator.fit(data.x_train, seed=randint(10, 100))

    history = model.fit_generator(
        generator.flow(data.x_train, data.y_train, batch_size=1000),
        epochs=10,
        steps_per_epoch=10,
        validation_data=(data.x_test, data.y_test)
    )

    score = model.evaluate(data.x_test, data.y_test)
    print("Test Loss:", score[0])
    print("Test Accuracy :", score[1])

    print(history.history)

    plot_acc(history)
    plt.show()
    plot_loss(history)
    plt.show()


if __name__ == '__main__':
    main()
