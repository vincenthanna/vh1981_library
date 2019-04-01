import tensorflow as tf
import numpy as np

'''
many-to-one 구조의 RNN

Open        High        Low         Volume  Close
828.659973	833.450012	828.349976	1247700	831.659973
X는 Open, High, Low, Volume, Close
Y는 Close

(batch_size, seq_length, data_dim)
@batch_size 입력 데이터의 1회 처리 수
@seq_length sequence 길이
@data_dim 단일 입력 데이터의 길이. 위에서처럼 5가 된다.

'''


def MinMaxScaler(data):
    """ Min Max Normalization
    Parameters
    ----------
    data : numpy.ndarray
        input data to be normalized
        shape: [Batch size, dimension]
    Returns
    ----------
    data : numpy.ndarry
        normalized data
        shape: [Batch size, dimension]
    References
    ----------
    .. [1] http://sebastianraschka.com/Articles/2014_about_feature_scaling.html
    """
    numerator = data - np.min(data, 0)
    denominator = np.max(data, 0) - np.min(data, 0)
    # noise term prevents the zero division
    return numerator / (denominator + 1e-7)

timesteps = seq_length = 7
data_dim = 5
output_dim = 1
hidden_dim = 5

# Open, High, Low, Close, Volume
xy = np.loadtxt('data-02-stock_daily.csv', delimiter=',')
xy = xy[::-1] #reverse order 거꾸로 1칸 간격
print("xy:", xy)
xy = MinMaxScaler(xy)
x = xy
y = xy[:, [-1]]

dataX = []
dataY = []

for i in range(0, len(y) - seq_length):
    _x = x[i:i + seq_length]
    _y = y[i + seq_length] # next Close price
    # print(_x, "--->", _y)
    # print("_x.shape:", np.array(_x).shape)
    dataX.append(_x)
    dataY.append(_y)

# data split : train/test
train_size = int(len(dataY) * 0.7)
test_size = len(dataY) - train_size
trainX, testX = np.array(dataX[0:train_size]), np.array(dataX[train_size:])
trainY, testY = np.array(dataY[0:train_size]), np.array(dataY[train_size:])

# input placeholder
X = tf.placeholder(tf.float32, [None, seq_length, data_dim])
Y = tf.placeholder(tf.float32, [None, 1])
print("X.shape=", X.shape)
print("Y.shape=", Y.shape)

# make model
cell = tf.contrib.rnn.BasicLSTMCell(num_units=hidden_dim)
outputs, _states = tf.nn.dynamic_rnn(cell, X, dtype=tf.float32)
Y_pred = tf.contrib.layers.fully_connected(outputs[:, -1], output_dim, activation_fn=None) # classification이 아니므로 activation_fn을 지정하지 않음

# cost/loss
loss = tf.reduce_sum(tf.square(Y_pred - Y))
optimizer = tf.train.AdamOptimizer(0.01)
train = optimizer.minimize(loss)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    for i in range(1000):
        _, l = session.run([train, loss], feed_dict={X:trainX, Y:trainY})
        out = session.run(outputs, feed_dict={X:trainX})
        #print("out.shape", out.shape)  # (-1, 7, 5) 가장 앞은 데이터 수
        print(i, l)

    testPred = session.run(Y_pred, feed_dict={X: testX})

    import matplotlib.pyplot as plt
    plt.plot(testY)
    plt.plot(testPred)
    plt.show()

