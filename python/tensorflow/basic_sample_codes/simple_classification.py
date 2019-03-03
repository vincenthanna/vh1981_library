import tensorflow as tf
import numpy as np

x_data = np.array([
    [0, 0], [1, 0], [1, 1], [0, 0], [0, 0], [0, 1]
])

y_data = np.array([
    [1,0,0],
    [0,1,0],
    [0,0,1],
    [1,0,0],
    [1,0,0],
    [0,0,1]
])

print(x_data.shape)
print(y_data.shape)

X = tf.placeholder(tf.float32)
Y = tf.placeholder(tf.float32)

W1 = tf.Variable(tf.random_uniform([2, 10], -1, 1))
W2 = tf.Variable(tf.random_uniform([10, 3], -1, 1))

b1 = tf.Variable(tf.zeros([10]))
b2 = tf.Variable(tf.zeros([3]))

L1 = tf.nn.relu(tf.add(tf.matmul(X, W1), b1))

model = tf.add(tf.matmul(L1, W2), b2)

cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=Y, logits=model))
optimizer = tf.train.AdamOptimizer(learning_rate=0.01)
train_optimizer = optimizer.minimize(cost)



with tf.Session() as session:
    init = tf.global_variables_initializer()
    session.run(init)

    for step in range(100):
        session.run(train_optimizer, feed_dict={X:x_data, Y:y_data})

        if (step + 1) % 10 == 0:
            print(step + 1, session.run(cost, feed_dict={X: x_data, Y: y_data}))

    pred = tf.argmax(model, 1)
    target = tf.argmax(Y, 1)
    print("예측:", session.run(pred, feed_dict={X: x_data}))
    print("실제:", session.run(target, feed_dict={Y: y_data})) #이걸 궂이 이럴 필요있나?

    correct = tf.equal(pred, target)
    accuracy = tf.reduce_mean(tf.cast(correct, tf.float32))
    print("정확도: %.2f" % session.run(accuracy * 100, feed_dict={X:x_data, Y:y_data}))



