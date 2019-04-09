from keras.preprocessing import sequence
from keras.datasets import imdb
from keras import layers, models

class Data:
    def __init__(self, max_features=20000, maxlen=80):
        '''
        :param max_features: 최대 단어수. 단어들을 빈도수대로 정렬하여 max_features 순위 밖은 무시한다.
        :param maxlen: 각각의 최대 길이(적으면 늘려주고 넘으면 잘라서 맞춘다)
        '''
        (x_train, y_train), (x_test, y_test) = imdb.load_data(num_words=max_features)

        print("len(x_train[0]) :", len(x_train[0]))
        print("x_train[0] :", x_train[0])

        print("x_train.shape :", x_train.shape)
        print("y_train.shape :", y_train)
        x_train = sequence.pad_sequences(x_train, maxlen=maxlen)  # 전부 같은 길이로 맞춰준다.
        x_test = sequence.pad_sequences(x_test, maxlen=maxlen)
        self.x_train, self.y_train = x_train, y_train
        self.x_test, self.y_test = x_test, y_test
        print("len(x_train[0]) :", len(x_train[0]))
        print("x_train[0] :", x_train[0])
        print("len(x_train[1]) :", len(x_train[1]))
        print("x_train[1] :", x_train[1])


class RNN_LSTM(models.Model):
    def __init__(self, max_features, maxlen):
        x = layers.Input((maxlen, ))
        """
        Embedding : Turns positive integers (indexes) into dense vectors of fixed size.
            eg. [[4], [20]] -> [[0.25, 0.1], [0.6, -0.2]]
        """
        h = layers.Embedding(input_dim=max_features, output_dim=128)(x) # h.shape = (?, 80, 128)

        print("h:\n", h)

        '''
        regular dropout : 입력 혹은 출력에 적용되는 dropout(xt, yt)
        recurrent dropout : recurrent unit (ht) 마다 적용되는 dropout
        '''
        h = layers.LSTM(128, dropout=0.2, recurrent_dropout=0.2)(h)
        y = layers.Dense(1, activation='sigmoid')(h)
        super().__init__(x, y)

        #try using different optimizers and different optimizer configs
        self.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])


# 학습 및 성능 평가
class Machine:
    def __init__(self, max_features=20000, maxlen=80):
        self.data = Data(max_features, maxlen)
        self.model = RNN_LSTM(max_features, maxlen)

    def run(self, epochs=3, batch_size=32):
        data = self.data
        model = self.model
        print("Training Stage")
        print("====================")
        model.fit(data.x_train, data.y_train, batch_size=batch_size, epochs=epochs, validation_data=(data.x_test, data.y_test))

        score, acc = model.evaluate(x=data.x_test, y=data.y_test, batch_size=batch_size)

        print("Test Performance: accuracy={0}, loss={1}".format(acc, score))

def main():
    m = Machine()
    m.run()

if __name__ == '__main__':
    main()



