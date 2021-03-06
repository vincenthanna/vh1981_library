import unittest
import tensorflow as tf
from face_model import face_recognition_model
import numpy as np
#from main import triplet_loss
import keras.backend as K

from keras import backend as K
K.set_image_data_format('channels_last')

class basicTest(unittest.TestCase):

    """이름에 test문구가 들어가 있어야 실행을 해준다."""
    def test_001(self):
        model = face_recognition_model(input_shape=(96, 96, 3))
        print("model output_shape=", model.output_shape)
        print("model.count_params()=", model.count_params())
        model.summary()

    def aa_test_expand_dims(self):
        with tf.Session() as test:

            x = tf.random_normal([10, 4]).eval(session=test)
            print("x.shape:", x.shape)
            a = tf.expand_dims(x, axis=1).eval(session=test)
            print("a.shape:", a.shape)
            print(a)

            b = tf.expand_dims(x, axis=0).eval(session=test)
            print("b.shape:", b.shape)
            print(b)

            print("a-b.shape", (a-b).shape)
            print((a-b))


            a = np.array([1,2])
            b = np.array([3,4])
            print(a, b)
            c = tf.expand_dims(a, axis=1).eval(session=test)
            print(c)
            d = tf.expand_dims(a, axis=0).eval(session=test)
            print(d)
            print("a-b:")
            print(c-d)

            a = tf.random_normal([10, 10, 4]).eval(session=test)
            print(a.shape)
            a = tf.sqrt(tf.reduce_sum(a, axis=-1))
            print(a.shape)

    def __test_222(self):
        with tf.Session() as test:
            a = np.array([
                [True, False],
                [False, True]
            ])
            print(a.shape)

            b = np.random.rand(2,2)
            print("b.shape:", b.shape)
            print(b)

            print("bbb:", tf.boolean_mask(b, a).eval(session=test))
            bb = tf.map_fn(lambda x : tf.reduce_min(tf.boolean_mask(x[0], x[1])), (tf.cast(b, dtype=tf.float32), a), dtype=tf.float32)
            print(bb.eval(session=test))

            k = np.random.rand(3, 2)
            print(tf.reduce_max(k, axis=0).eval(session=test).shape)
            print(tf.reduce_max(k, axis=1).eval(session=test).shape)
            print(tf.reduce_max(k, axis=-1).eval(session=test).shape)

    def test_333(self):
        with tf.Session() as test:
            K.set_session(test)
            a = K.eye(3)
            print(K.eval(a))



if __name__ == '__main__':
    unittest.main(warnings='ignore')

