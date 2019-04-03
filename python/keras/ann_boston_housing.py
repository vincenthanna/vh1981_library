from keras import layers, models

class ANN(models.Model):
    def __init__(self, nin, nh, nout):
        # layer, activation function 준비
        hidden = layers.Dense(nh)
        output = layers.Dense(nout)
        relu = layers.Activation('relu')

        # connect network elements
        x = layers.Input(shape=(nin,))
        h = relu(hidden(x))
        y = output(h)

        super().__init__(x, y)

        self.compile(loss='mse', optimizer='sgd')


# 학습과 평가용 데이터 불러오기
from keras import datasets
from sklearn import preprocessing

def load_data():
    (X_train, y_train), (X_test, y_test) = datasets.boston_housing.load_data()
    scaler = preprocessing.MinMaxScaler()
    X_train = scaler.fit_transform(X_train)
    X_test = scaler.fit_transform(X_test)
    return (X_train, y_train), (X_test, y_test)


import matplotlib.pyplot as plt
from utils.skeras import plot_loss

def main():
    nin = 13
    nh = 5
    nout = 1

    model = ANN(nin, nh, nout)
    (X_train, y_train), (X_test, y_test) = load_data()

    history = model.fit(X_train, y_train, epochs=100, batch_size=100, validation_split=0.2, verbose=2)

    performance_test = model.evaluate(X_test, y_test, batch_size=100)
    print("\nTest Loss -> {:.2f}".format(performance_test))

    plot_loss(history)
    plt.show()


if __name__ == '__main__':
    main()
