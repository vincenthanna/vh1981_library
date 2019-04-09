from keras.preprocessing import sequence
from keras.datasets import imdb
from keras import layers, models

class Data:
    def __init__(self, max_features=20000, maxlen=80):
        (x_train, y_train), (X_test, y_test) = imdb.load_data(num_words=max_features)
        print("x_train:\n", x_train)
        print("y_train:\n", y_train)
        x_train = sequence.pad_sequences(x_train, maxlen=maxlen)  # 전부 같은 길이로 맞춰준다.
        x_test = sequence.pad_sequences(x_test, maxlen=maxlen)
        self.x_train, self.y_train = x_train, y_train
        self.x_test, self.y_test = x_test, y_test


class RNN_LSTM(models.Model):
    def __init__(self, max_features, maxlen):
        x = layers.Input((maxlen, ))
        """
        Embedding : Turns positive integers (indexes) into dense vectors of fixed size.
            eg. [[4], [20]] -> [[0.25, 0.1], [0.6, -0.2]]
        """
        h = layers.Embedding(input_dim=max_features, output_dim=128)(x)
        '''
        regular dropout : 입력 혹은 출력에 적용되는 dropout(xt, yt)
        recurrent dropout : recurrent unit (ht) 마다 적용되는 dropout
        '''
        h = layers.LSTM(128, dropout=0.2, recurrent_dropout=0.2)(h)

