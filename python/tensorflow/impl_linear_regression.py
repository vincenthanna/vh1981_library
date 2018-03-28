import tensorflow as tf

x_data = [1,2,3]
y_data = [1,2,3]

# weight(가중치)와 bias(편향) 변수를 초기화 한다.
W = tf.Variable(tf.random_uniform([1], -1.0, 1.0))
b = tf.Variable(tf.random_uniform([1], -1.0, 1.0))

# 값을 입력받을 placeholder를 생성한다.
X = tf.placeholder(tf.float32, name="X")
Y = tf.placeholder(tf.float32, name="Y")

# 선형관계를 위한 수식을 작성한다.
# W : 가중치
# b : 편향
hypothesis = W * X + b

# 손실값(예측값에서 실제 값을 뺀 뒤 제곱) 수식
# reduce_mean : Computes the mean(평균) of elements across dimensions of a tensor.
cost = tf.reduce_mean(tf.square(hypothesis - Y)) # 손실값은 예측값에서 실제값을 뺀 뒤 제곱하며, 비용은 모든 데이터에 대한 손실값의 평균을 내어 구한다.

# 경사하강법(gradient descent) 최적화 함수를 이용해 손실값을 최소화하는 연산그래프를 생성한다.
optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.1)
train_op = optimizer.minimize(cost)

with tf.Session() as session:
    session.run(tf.global_variables_initializer())
    '''
    최적화를 수행하는 그래프인 train_op를 충분히 학습이 이루어지도록 수행한다.(100번?)
    '''
    for step in range(100):
        session.run([train_op], feed_dict={X: x_data, Y: y_data}) # 수행할수록 train_op가 수행하는 W, b 값이 오차값이 줄어드는 방향으로 변경된다.

    print("cost_val_mean=", sum(session.run(hypothesis, feed_dict={X:x_data}) - y_data) / 3)
    print("X:5, Y:", session.run(hypothesis, feed_dict={X:5}))
    print("X:2.5, Y:", session.run(hypothesis, feed_dict={X:2.5}))
