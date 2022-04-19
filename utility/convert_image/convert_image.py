# using pip install pillow-avif-plugin
import pillow_avif
from PIL import Image
import io
import multiprocessing
import os
import sys
import traceback
import zipfile

def convert_avif_byte_to_webp(avif_byte, webp_fn):
    im = Image.open(io.BytesIO(avif_byte)).convert('RGB')
    im.save(webp_fn, quality=50, method=6)
    im.close()

def convert_single(srcdir, dstdir, input_queue, tmpfilename):
    while True:
        try:
            src_fn = input_queue.get(timeout=1)
        except:
            break

        try:
            fnext = os.path.splitext(src_fn)[1].lower()
            dst_fn = src_fn[len(srcdir):]
            while dst_fn[0] in ['/','\\']:
                dst_fn = dst_fn[1:]
            dst_fn = os.path.join(dstdir, dst_fn)
            ddir = os.path.split(dst_fn)[0]
            if not os.path.isdir(ddir):
                try:
                    os.makedirs(ddir)
                except:
                    pass

            if fnext == '.avif':
                dst_fn = dst_fn[:-5]+'.webp'
                with open(src_fn,'rb') as ifile:
                    avif_byte = ifile.read()
                convert_avif_byte_to_webp(avif_byte, dst_fn)
            else:
                with zipfile.ZipFile(src_fn,'r') as izip:
                    izdata = {}
                    for nm in izip.namelist():
                        with izip.open(nm,'r') as izzf:
                            izzf_data = izzf.read()
                            izdata[nm] = izzf_data

                    with zipfile.ZipFile(dst_fn,'w',zipfile.ZIP_DEFLATED) as ozip:
                        for k in izdata:
                            if len(izdata[k]) == 0:
                                continue
                            
                            if k[-5:].lower() == '.avif':
                                convert_avif_byte_to_webp(izdata[k],tmpfilename)
                                with open(tmpfilename, 'rb') as tif:
                                    tdata = tif.read()
                                nn = os.path.split(k)[1]
                                nn = nn[:-5] + '.webp'
                                with ozip.open(nn,'w') as ozf:
                                    ozf.write(tdata)
                            else:
                                nn = os.path.split(k)[1]
                                try:
                                    with ozip.open(nn,'w') as ozf:
                                        ozf.write(izdata[k])
                                except:
                                    with ozip.open('meta_info.txt','w') as ozf:
                                        ozf.write(izdata[k])
        except:
            print(traceback.format_exc())
            print(f'Unable to handle {src_fn}')

def print_usage():
    print(f'Usage: {sys.argv[0]} <source directory> <target directory>')

def convert_multiprocess(srcdir, dstdir):
    if not os.path.isdir(srcdir):
        print(f'{srcdir} does not exist!')
        return

    srcdir = os.path.abspath(srcdir)
    dstdir = os.path.abspath(dstdir)

    input_queue = multiprocessing.Queue()
    num_process = os.cpu_count()-4

    worker_procs = [multiprocessing.Process(target=convert_single, args=(srcdir, dstdir, input_queue, f'___tmpfile{i}__.webp')) for i in range(num_process)]

    for rootdir, dirs, files in os.walk(srcdir):
        for fn in files:
            fnext = os.path.splitext(fn)[1].lower()
            if fnext in ['.avif','.zip']:
                src_fn = os.path.join(rootdir, fn)
                input_queue.put(src_fn)

    for wp in worker_procs:
        wp.start()
    for wp in worker_procs:
        wp.join()

if __name__ == '__main__':
    if len(sys.argv) == 3:
        convert_multiprocess(sys.argv[1], sys.argv[2])
    else:
        print_usage()