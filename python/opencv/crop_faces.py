import cv2
import sys
import os
import uuid


print(cv2.__version__)

imageDirPath = sys.argv[1]
cascPath = sys.argv[2]

faceCascade = cv2.CascadeClassifier(cascPath)

files = [f for f in os.listdir(imageDirPath) if os.path.isfile(os.path.join(imageDirPath, f))]
index = 0

for f in files:
    try:
        imagePath = os.path.join(imageDirPath, f)
        image = cv2.imread(imagePath)
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

        faces = faceCascade.detectMultiScale(
            gray,
            scaleFactor=1.2,
            minNeighbors=5,
            minSize=(30, 30),
            flags = cv2.CASCADE_SCALE_IMAGE
        )

        print("Found {0} faces!".format(len(faces)))

        # Draw a rectangle around the faces

        for (x, y, w, h) in faces:
            cv2.rectangle(image, (x, y), (x+w, y+h), (0, 255, 0), 2)
            crop_img = image[y:y+h, x:x+w]

            # cv2.imshow("cropped" + str(index), crop_img)

            #cv2.waitKey(0)

            croppedImgsDir = os.path.join(imageDirPath, "cropped")
            if not os.path.exists(croppedImgsDir):
                os.makedirs(croppedImgsDir)

            cropFaceFileName = str(uuid.uuid4()) + ".jpeg"
            cropFaceFilePath = os.path.join(croppedImgsDir, cropFaceFileName)
            print(cropFaceFilePath)
            cv2.imwrite(cropFaceFilePath, crop_img)

            index += 1

    except:
        pass
#
#
#
# # Read the image
# image = cv2.imread(imagePath)
# gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
#
# faces = faceCascade.detectMultiScale(
#     gray,
#     scaleFactor=1.1,
#     minNeighbors=5,
#     minSize=(30, 30),
#     flags = cv2.CASCADE_SCALE_IMAGE
# )
#
# print("Found {0} faces!".format(len(faces)))
#
# # Draw a rectangle around the faces
# index = 0
# for (x, y, w, h) in faces:
#     cv2.rectangle(image, (x, y), (x+w, y+h), (0, 255, 0), 2)
#     crop_img = image[y:y+h, x:x+w]
#
#     cv2.imshow("cropped" + str(index), crop_img)
#
#     #cv2.waitKey(0)
#     cv2.imwrite("cropped" + str(index) + ".jpeg", crop_img)
#
#     index += 1


# cv2.imshow("Faces found", image)
# cv2.waitKey(0)