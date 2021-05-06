import sys
import cv2 as cv

img = cv.imread(cv.samples.findFile("scene.jpg"))
print(img)
if img is None:
    sys.exit("Could not read the image.")
cv.imshow("Display window", img)


# image = Image.open('scene.jpg')
# # convert image to numpy array
# data = asarray(image)
# numpy.savetxt("outimg.txt", data, fmt="%s")   
# # print((data))

# new_im = Image.fromarray(data).save('saved.jpg')
