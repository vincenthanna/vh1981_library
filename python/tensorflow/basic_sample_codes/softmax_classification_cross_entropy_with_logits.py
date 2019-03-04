import tensorflow as tf
import numpy as np

xy = np.loadtxt('data-04-zoo.csv', delimiter=',', dtype=np.float32)
x_data = xy[:, 0:-1]
y_data = xy[:, [-1]]

nb_classes = 7
X = tf.placeholder(tf.float32, shape=[None, 16])
Y = tf.placeholder(tf.int32, shape=[None, 1])
Y_one_hot = tf.one_hot(Y, nb_classes)
# tf.one_hot은 결과물의 차원을 증가시키므로 다시 줄여줘야한다.([[[...]], [[...]], ...] => [[...],[...],...]
Y_one_hot = tf.reshape(Y_one_hot, [-1, nb_classes])

W = tf.Variable(tf.random_normal([16, nb_classes], seed=1), name='variable')
b = tf.Variable(tf.random_normal([nb_classes], seed=1), name='bias')

# training :
logits = tf.matmul(X, W) + b
cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=logits, labels=Y_one_hot))
optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.1).minimize(cost)

# test :
hypothesis = tf.nn.softmax(logits)
prediction = tf.argmax(hypothesis, axis=1)
correct_pred = tf.equal(prediction, tf.argmax(Y_one_hot, axis=1))
accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    for step in range(2001):
        session.run(optimizer, feed_dict={X:x_data, Y:y_data})
        if step % 100 == 0:
            cv, acc = session.run([cost, accuracy], feed_dict={X:x_data, Y:y_data})
            print("Step: {:5} cost:{:.3f} accuracy:{:.2f}".format(step, cv, acc))

    pred = session.run(prediction, feed_dict={X:x_data})
    # y_data는 (N,1)이므로 zip()을 사용하려면 1차원으로 펴줘야 한다.
    for p, y in zip(pred, y_data.flatten()):
        print("[{}] prediction: {} type Y: {}".format(p==int(y), p, int(y)))
