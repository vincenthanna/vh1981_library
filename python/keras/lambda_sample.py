import numpy as np

def Lambda_with_lambda():
    from keras.layers import Lambda, Input
    from keras.models import Model

    x = Input((1, ))
    y = Lambda(lambda x: x + 1)(x)
    m = Model(x, y)

    yp = m.predict([1])
    print(yp)

Lambda_with_lambda()


from keras import backend as K

def add_decorate(x):
    '''
    편차 제곱을 x 마지막 차원에 일일이 붙여준다.
    '''
    # axis = -1은 마지막 차원을 지정한다.
    m = K.mean(x, axis=-1, keepdims=True)
    print("x.shape=", x.shape,"m.shape=", m.shape)
    d = K.square(x - m)
    return K.concatenate([x, d], axis=-1)

def add_decorate_shape(input_shape):
    shape = list(input_shape)
    assert len(shape) == 2
    shape[1] *= 2
    print("shape:", shape)
    return tuple(shape)

def Lambda_with_shape():
    from keras.layers import Lambda, Input
    from keras.models import Model

    #in_sample = lambda n_batch: np.random.rand(n_batch, 10)
    in_sample = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    print(in_sample.shape)
    in_sample = in_sample.reshape((-1, 10))

    x = Input((10, ))
    y = Lambda(add_decorate, output_shape=add_decorate_shape, input_shape=(10,))(x)
    m = Model(x, y)

    yp = m.predict(in_sample)
    #print(yp)

Lambda_with_shape()


def Lambda_with_tf():
    from keras.layers import Lambda, Input
    from keras.models import Model
    import tensorflow as tf

    def add_decorate_tf(x):
        '''
        편차 제곱을 x 마지막 차원에 일일이 붙여준다.
        '''
        # axis = -1은 마지막 차원을 지정한다.
        m = tf.reduce_mean(x, axis=-1, keep_dims=True)
        print("x.shape=", x.shape, "m.shape=", m.shape)
        #d = K.square(x - m)
        d = tf.sqrt(x - m)
        #return K.concatenate([x, d], axis=-1)
        return tf.concat([x, d], axis=-1)

    def add_decorate_shape_tf(input_shape):
        shape = list(input_shape)
        assert len(shape) == 2
        shape[1] *= 2
        print("shape:", shape)
        return tuple(shape)

    #in_sample = lambda n_batch: np.random.rand(n_batch, 10)
    in_sample = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    print(in_sample.shape)
    in_sample = in_sample.reshape((-1, 10))

    x = Input((10, ))
    y = Lambda(add_decorate, output_shape=add_decorate_shape, input_shape=(10,))(x)
    m = Model(x, y)

    yp = m.predict(in_sample)
    print(yp)

Lambda_with_tf()