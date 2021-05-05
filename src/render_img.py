from PIL import Image
from numpy import asarray
import numpy

image = Image.open('flower.jpg')
# convert image to numpy array
data = asarray(image)
numpy.savetxt("outimg.txt", data, fmt="%s", newline="----------")   
# print((data))

new_im = Image.fromarray(data).save('saved.jpg')
