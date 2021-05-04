from PIL import Image
from numpy import asarray

image = Image.open('flower.jpg')
# convert image to numpy array
data = asarray(image)
print((data))

new_im = Image.fromarray(data).save('saved.jpg')
