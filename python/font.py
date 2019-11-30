import PIL
from PIL import ImageFont
from PIL import Image
from PIL import ImageDraw
import numpy as np

# font = ImageFont.truetype("Arial-Bold.ttf",14)
font = ImageFont.truetype("font/arial.ttf",12)
f = open("array.txt", 'w')
f.write('{')
buf=0
for n in np.arange(17):
    img=Image.new("1", (16,16),(0))
    draw = ImageDraw.Draw(img)
    if n<9:
        draw.text((4, 1),str(n+1),(255),font=font)
    elif n<16:
        draw.text((1, 1),str(n+1),(255),font=font)
    else:
        draw.text((1, 1), str(n + 16), (255), font=font)
    img.save("img/"+str(n+1)+".bmp")
    f.write('{')
    for l in np.arange(16):
        for m in np.arange(16):
            if img.getpixel((int(m),int(l)))!=0:
                buf+=1*pow(2,m)
                print('🚒 ',end='')
            else:
                print('😀 ',end='')
        print('\n')
        f.write(str(buf))
        buf = 0
        if l<15:
            f.write(' ,')
    print('----------------')
    f.write('},\n')
f.write('}')
f.close()