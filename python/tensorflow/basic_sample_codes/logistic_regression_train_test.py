import tensorflow as tf
import numpy as np

x_data = [[1,2,1], [1,3,2], [1,3,4], [1,5,5], [1,7,5], [1,2,5], [1,6,6], [1,7,7]]
y_data = [[2],[2],[2],[1],[1],[1],[0],[0]]

x_test = [[2,1,1], [3,1,2], [3,3,4]]
y_test = [[2],[2],[2]]

nb_classes = 3
X = tf.placeholder(tf.float32, shape=[None, 3])
Y = tf.placeholder(tf.int32, shape=[None, 1])
Y_one_hot = tf.one_hot(Y, nb_classes)
Y_one_hot = tf.reshape(Y_one_hot, [-1, nb_classes])

W = tf.Variable(tf.random_normal([3, nb_classes]), name='variable')
b = tf.Variable(tf.random_normal([nb_classes]), name='bias')

# model building:
'''
# 수동 방법:
hypothesis = tf.nn.softmax(tf.matmul(X,W) + b)
cost = tf.reduce_mean(-tf.reduce_sum(Y_one_hot * tf.log(hypothesis), axis=1))
'''
# softmax_cross_entropy_with_logits() 사용:
logits = tf.matmul(X, W) + b
hypothesis = tf.nn.softmax(logits) # 나중에 디버깅용
cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=logits, labels=Y_one_hot))
optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.1).minimize(cost)

prediction = tf.argmax(hypothesis, axis=1)
is_correct = tf.equal(prediction, tf.argmax(Y_one_hot, axis=1))
accuracy = tf.reduce_mean(tf.cast(is_correct, tf.float32))

with tf.Session() as session:
    # training 단계:
    session.run(tf.global_variables_initializer())

    for step in range(201):
        cv, wv, _ = session.run([cost, W, optimizer], feed_dict={X:x_data, Y:y_data})
        print("Step:", step, "cost:", cv, "w:", wv)

    # test 단계:
    pred = session.run(prediction, feed_dict={X:x_test})
    print("accuracy : ", session.run(accuracy, feed_dict={X:x_test, Y:y_test}))
