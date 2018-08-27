import unittest
import google_image_crawler
import os

class OutcomesTest(unittest.TestCase):

    def test_download_image(self):
        working = False
        url="http://thezam.co.kr/file_data/thezam00/2016/11/22/3622057597602174cee198e05e192ec7.jpeg"
        savedir = "."
        real_url, file_name = google_image_crawler.download_image(url, ".")
        print(real_url, file_name)
        if os._exists(file_name):
            stat = os.stat(file_name)
            if stat.st_size > 0:
                working = True
            else:
                print("size = 0")
        else:
            print("no exists")

        self.assertTrue(working)

    def test_fail(self):
        self.assertTrue(True)

    def test_error(self):
        pass

if __name__ == '__main__':
    unittest.main()