import unittest
import tensorflow as tf
from face_model import face_recognition_model
import numpy as np
from dataset import prepare_data, get_batch
import keras.backend as K
import matplotlib.pyplot as plt

K.set_image_data_format('channels_last')

class basicTest(unittest.TestCase):
    """이름에 test문구가 들어가 있어야 실행을 해준다."""

    def test_001(self):
        labels, imgpaths = prepare_data()

        batch_size = 32
        labels_batch = get_batch(labels, batch_size, 0)
        images_batch = get_batch(imgpaths, batch_size, 0, isImgPath=True)
        print("labels_batch.shape=", labels_batch.shape)
        print("images_batch.shape=", images_batch.shape)

        for i in range(images_batch.shape[0]):
            img = images_batch[i]
            plt.imshow(img)
            plt.show()


if __name__ == '__main__':
    unittest.main(warnings='ignore')

