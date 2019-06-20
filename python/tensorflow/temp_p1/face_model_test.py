import unittest
import tensorflow as tf
from face_model import face_recognition_model
from main import triplet_loss_func
import numpy as np

from keras import backend as K
K.set_image_data_format('channels_last')

class basicTest(unittest.TestCase):

    """이름에 test문구가 들어가 있어야 실행을 해준다."""
    def test_001(self):
        model = face_recognition_model(input_shape=(96, 96, 3))
        print("model output_shape=", model.output_shape)
        print("model.count_params()=", model.count_params())
        model.summary()

    def test_triplet_loss(self):
        with tf.Session() as test:
            tf.set_random_seed(1)
            y_pred = (tf.random_normal([3, 128], mean=6, stddev=0.1, seed=1),
                      tf.random_normal([3, 128], mean=1, stddev=1, seed=1),
                      tf.random_normal([3, 128], mean=3, stddev=4, seed=1))
            loss = triplet_loss_func(y_pred)

            self.assertTrue(round(loss.eval(), 3), 528.143)

    def test_expand_dims(self):
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


if __name__ == '__main__':
    unittest.main(warnings='ignore')

