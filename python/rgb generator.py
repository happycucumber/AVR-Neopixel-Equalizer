import PIL
from PIL import ImageFont
from PIL import Image
from PIL import ImageDraw
import numpy as np

f = open("animation.txt", 'w')
f.write('{')
for n in np.arange(9):
    temp=Image.open('img/'+str(n+1)+'.png')
    img=temp.convert('RGB')
    pix=np.array(img)
    f.write('{')

    for l in np.arange(16):
        for m in np.arange(16):
            f.write('{')
            f.write(str(pix[l][m][0]))
            f.write(',')
            f.write(str(pix[l][m][1]))
            f.write(',')
            f.write(str(pix[l][m][2]))
            f.write('}')
            if m!=15:
                f.write(',')
        if l!=15:
            f.write(',')
        else:
            f.write('},')
        f.write('\n')
    f.write('\n')
f.write('}')
f.close()

