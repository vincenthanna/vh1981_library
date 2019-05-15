from keras import backend as K
from keras.engine.topology import Layer
import numpy as np

from keras import initializers
init_he = initializers.he_normal(seed=None)
init_zeros = initializers.zeros()
class SFC(Layer):
    def __init__(self, No, **kwargs):
        self.No = No # 출력 벡터의 길이
        super().__init__(**kwargs)

    def build(self, input_shape):
        '''
        :param input_shape: 입력 shape
        :return:

        layer에 사용될 parameter들을 생성해 놓는다.

        X * W :
            (N ,in) * (in, out) = (N, out)
            in은 shape[1] 이고 (열의 길이), out은 self.No
        '''
        self.w = self.add_weight("w", (input_shape[1], self.No), initializer=init_he)
        self.b = self.add_weight("b", (self.No,), initializer=init_zeros)
        super().build(input_shape)

    def call(self, x):
        return K.dot(x, self.w) + self.b

    def compute_output_shape(self, input_shape):
        return (input_shape[0], self.No)

x = np.array([0,1,2,3,4])
y = x * 2 + 1


import keras
model = keras.models.Sequential()
model.add(SFC(1, input_shape=(1,)))
model.compile(optimizer='SGD', loss='mse')

model.fit(x[:2], y[:2], epochs=10000, verbose=0)
print(model.predict(x[2:]))


