import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn import model_selection
from keras import models, layers

from utils import skeras

#코드 실행 및 결과 보기
def main():
    machine = Machine()
    machine.run(epochs=400)

def rnn_model(shape):
    m_x = layers.Input(shape=shape) # X.shape[1:]
    m_h = layers.LSTM(units=10)
    m_y = layers.Dense(1)(m_h)
    m = models.Model(m_x, m_y)

    m.compile(optimizer='adam', loss='mean_squared_error')
    m.summary()
    return m

class Dataset:
    def __init__(self, fname='international-airline-passengers.csv', D=12):
        data_dn = load_data(fname=fname)
        X, y = get_Xy(data_dn, D=D)
        X_train, X_test, y_train, y_test = model_selection.train_test_split(X, y, test_size=0.2, random_state=42)

        self.X, self.y = X, y
        self.X_train, self.X_test, self.y_train, self.y_test = X_train, X_test, y_train, y_test


def load_data(fname='international-airline-passengers.csv'):
    dataset = pd.read_csv(fname, usecols=[1], engine='python', skipfooter=3)
    data = dataset.values.reshape(-1)
    plt.plot(data)
    plt.xlabel('Time')
    plt.ylabel('#passengers')
    plt.title('Original Data')

    # normalized data
    data_dn = (data - np.mean(data)) / np.std(data) / 5 # 5로 나누는 것은 뭔가?
    plt.plot(data_dn)
    plt.xlabel('Time')
    plt.ylabel('#passengers(normalized)')
    plt.title('Normalized data by $E[]$ and $5\sigma$')
    plt.show()
    return data_dn


def get_Xy(data, D=12):
    # make X and y
    X_1 = []
    y_1 = []
    N - len(data)






class Machine():
    def __init__(self):
        self.data = Dataset()
        shape = self.data.X.shape[1:] # 입력 shape에서 데이터 수를 제외한다.
        self.model = rnn_model(shape)

    def run(self, epochs=400):