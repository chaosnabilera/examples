# using pip install pillow-avif-plugin
import pillow_avif
from PIL import Image

if __name__ == '__main__':
    im = Image.open('aa.avif').convert('RGB')
    im.save('aa.webp', quality=50, method=6)