import unittest
from face_model import faceRecoModel

from keras import backend as K
K.set_image_data_format('channels_first')

class basicTest(unittest.TestCase):

    """이름에 test문구가 들어가 있어야 실행을 해준다."""
    def test_001(self):
        model = faceRecoModel(input_shape=(3, 96, 96))
        print("model output_shape=", model.output_shape)

    def test_002(self):
        pass

if __name__ == '__main__':
    unittest.main(warnings='ignore')

