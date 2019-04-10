import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn import model_selection
from keras import models, layers
import seaborn as sns

from utils import skeras

#코드 실행 및 결과 보기
def main():
    machine = Machine()
    machine.run(epochs=400)

def rnn_model(shape):
    print("rnn_model shape=", shape) # (12, 1)
    m_x = layers.Input(shape=shape) # X.shape[1:]
    m_h = layers.LSTM(units=10)(m_x)
    m_y = layers.Dense(1)(m_h)
    m = models.Model(m_x, m_y)

    m.compile(optimizer='adam', loss='mean_squared_error')
    m.summary()
    return m

class Dataset:
    def __init__(self, fname='international-airline-passengers.csv', D=12):
        data_dn = load_data(fname=fname)
        X, y = get_Xy(data_dn, D=D)
        print(X.shape, y.shape) # (?, 12, 1), (131,)
        X_train, X_test, y_train, y_test = model_selection.train_test_split(X, y, test_size=0.2, random_state=42)

        self.X, self.y = X, y
        self.X_train, self.X_test, self.y_train, self.y_test = X_train, X_test, y_train, y_test


def load_data(fname='international-airline-passengers.csv'):
    '''
    :param fname: 읽어들일 파일명
    :return:
    '''

    """
    skipfooter : 끝에서부터 무시할 줄 수.
    실제 데이터를 보면 마지막 몇줄은 데이터가 아니다.
    engine : 사용할 parser engine. C 엔진은 빠르고, python엔진은 feature-complete(기능이 모두 있음?)
    usecols : 필요한 열만 불러올 수 있다. [1]은 두번째 열만 불러온다. callable(lambda 같은 것)도 넘길 수 있다.
    """
    dataset = pd.read_csv(fname, usecols=[1], engine='python', skipfooter=3) # csv파일로부터 데이터를 읽는다.
    print("dataset.shape=", dataset.shape) # (144, 1)
    data = dataset.values.reshape(-1) # (144,)로 변경
    print("data.shape=", data.shape)

    # 읽어들인 데이터를 그래프로 그려본다.
    plt.plot(data)
    plt.xlabel('Time')
    plt.ylabel('#passengers')
    plt.title('Original Data')
    plt.show()

    # normalized data (https://en.wikipedia.org/wiki/Standard_score)
    data_dn = (data - np.mean(data)) / np.std(data) / 5 # 5로 나누는 것은 뭔가?
    plt.plot(data_dn)
    plt.xlabel('Time')
    plt.ylabel('#passengers(normalized)')
    plt.title('Normalized data by $E[]$ and $5\sigma$')
    plt.show()
    return data_dn


def get_Xy(data, D=12):
    '''
    :param data:
    :param D: RNN에 한번에 입력할 input 데이터 수
    :return:
    '''
    # make X and y
    X_l = []
    y_l = []
    N = len(data) # data.shape = (144,)
    assert N > D, "N should be larger than D, where N is len(data)"
    '''     
    ii부터 ii+D-1 까지의 값을 입력(X) 로 하고 출력값은 바로 다음값을 사용한다.
    ===> 0~n-1까지를 입력으로 해서 n번째의 값을 예측하는 many-to-one rnn 모델
    '''
    for ii in range(N-D-1): # -1한 것은 예측값 때문에
        X_l.append(data[ii:ii+D]) #ii부터 D개까지 (ii ~ ii+D-1)
        y_l.append(data[ii+D]) #다음 값이 Y가 된다.

    X = np.array(X_l)
    print("X.shape=", X.shape) #(131,12)
    X = X.reshape(X.shape[0], X.shape[1], 1) #입력 shape가 (12, 1)이므로 (?, 12, 1)로 변경
    print("X.shape 2=", X.shape)
    y = np.array(y_l)
    print("X.shape=", X.shape, "y.shape=", y.shape)
    return X, y


class Machine():
    def __init__(self):
        self.data = Dataset()
        shape = self.data.X.shape[1:] # 입력 shape에서 데이터 수를 제외한다.
        print("input shape=", shape) # (12, 1)
        self.model = rnn_model(shape)

    def run(self, epochs=400):
        d = self.data
        X_train, X_test = d.X_train, d.X_test
        y_train, y_test = d.y_train, d.y_test
        X, y = d.X, d.y #전체 데이터
        m = self.model

        # train:
        h = m.fit(X_train, y_train, epochs=epochs, validation_data=[X_test, y_test], verbose=0)

        skeras.plot_loss(h)
        plt.title('history of training')
        plt.show()

        yp = m.predict(X_test)
        print("yp : ", yp) # (27, 1)
        print("loss:", m.evaluate(X_test, y_test))
        plt.plot(y_test, label='Original')
        plt.plot(yp, label='prediction')
        plt.legend(loc=0)
        plt.title('Validation Results')
        plt.show()

        yp = m.predict(X_test).reshape(-1) # 1차원 배열로 변경
        print("loss:", m.evaluate(X_test, y_test))
        #print(yp.shape, y_test.shape)

        df = pd.DataFrame()
        df['Sample'] = list(range(len(y_test))) * 2
        df['Normalized #Passengers'] = np.concatenate([y_test, yp], axis=0)
        df['Type'] = ['Original'] * len(y_test) + ['Prediction'] * len(yp)

        plt.figure(figsize=(7, 5))
        sns.barplot(x="Sample", y="Normalized #Passengers", hue="Type", data=df)
        plt.ylabel("Normalized #Passengers")
        plt.show()

        yp = m.predict(X)

        plt.plot(y, label="Original")
        plt.plot(yp, label="Prediction")
        plt.legend(loc=0)
        plt.title("All Results")
        plt.show()



if __name__ == '__main__':
    main()





