import tensorflow as tf

xd = [1,2,3]
yd = [1,2,3]

W = tf.Variable(tf.random_uniform([1], -1.0, 1.0))
b = tf.Variable(tf.random_uniform([1], -1.0, 1.0))

X = tf.placeholder(tf.float32, name="X")
Y = tf.placeholder(tf.float32, name="Y")

h = W * X + b

cost = tf.reduce_mean(tf.square(h - Y))
optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.1)
train_op = optimizer.minimize(cost)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())

    for step in range(100):
        _, cost_val = session.run([train_op, cost], feed_dict={X:xd, Y:yd})

        print(step, cost_val, session.run(W), session.run(b))


    print("\n======test======")
    print("X: 5, Y: ", session.run(h, feed_dict={X:5}))
    print("X: 2.5, Y:", session.run(h, feed_dict={X:2.5}))
