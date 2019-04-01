import tensorflow as tf
import numpy as np

h = [1, 0, 0, 0, 0]
i = [0, 1, 0, 0, 0]
e = [0, 0, 1, 0, 0]
l = [0, 0, 0, 1, 0]
o = [0, 0, 0, 0, 1]

hidden_size = 5 #y 출력 크기
input_dim = 5 #입력 x의 길이
batch_size = 1 #batch 데이터 입력 크기
seq_len = 6 #한번에 입력되는 x의 갯수

nb_classes = 5
idx2char = ['h', 'i', 'e', 'l', 'o']

x_data = [[0, 1, 0, 2, 3, 3]]
y_data = [[1, 0, 2, 3, 3, 4]]

x_one_hot_tensor = tf.one_hot(x_data, nb_classes)
print("x_one_hot_tensor.shape:", x_one_hot_tensor.shape)

X = tf.placeholder(tf.float32, [None, seq_len, input_dim])
Y = tf.placeholder(tf.int32, [None, seq_len])

print("X.shape=", X.shape)

rnn_cell = tf.contrib.rnn.BasicLSTMCell(num_units=hidden_size)
initial_state = rnn_cell.zero_state(batch_size, tf.float32)

outputs, _states = tf.nn.dynamic_rnn(rnn_cell, X, initial_state=initial_state, dtype=tf.float32)
print("outputs.shape:", outputs.shape)
prediction = tf.argmax(outputs, axis=2, output_type=tf.int32)
correct = tf.equal(tf.reshape(prediction, [-1]), tf.reshape(Y, [-1]))
accuracy = tf.reduce_mean(tf.cast(correct, dtype=tf.float32))

weights = tf.ones([batch_size, seq_len])

sequence_loss = tf.contrib.seq2seq.sequence_loss(logits=outputs, targets=Y, weights=weights)

print("sequence_loss.shape:", sequence_loss.shape)

loss = tf.reduce_mean(sequence_loss)

print("loss.shape:", loss.shape)

train = tf.train.AdamOptimizer(learning_rate=0.1).minimize(loss)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())
    '''
    중요! :
    feed_dict 에는 Tensor object를 넣을 수 없다.
    입력에 tensor 연산을 한다면 feed하기 전에 먼저 실행해서 value로 변경해야 한다.
    '''
    x_one_hot = session.run(x_one_hot_tensor)
    for i in range(2000):
        l, _ = session.run([loss, train], feed_dict={X: x_one_hot, Y: y_data})
        result, acc = session.run([prediction, accuracy], feed_dict={X: x_one_hot, Y: y_data})
        if (i % 100) == 0:
            print(i, "loss:", l, "prediction:", result, "expected:", y_data, "accuracy:", acc)
            result_str = [idx2char[c] for c in np.squeeze(result)]
            print("\tprediction str: ", ''.join(result_str))
    print("final test:")
    result = session.run(prediction, feed_dict={X:x_one_hot})
    result_str = [idx2char[c] for c in np.squeeze(result)]
    print("\tprediction str: ", ''.join(result_str))
