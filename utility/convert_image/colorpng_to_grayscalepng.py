import datetime
import logging
import multiprocessing
import os
import sys
import traceback

from PIL import Image

def create_logger():
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)
    formatter = logging.Formatter('[%(asctime)s] %(message)s')
    stream_handler = logging.StreamHandler()
    stream_handler.setFormatter(formatter)
    logger.addHandler(stream_handler)
    fname = datetime.datetime.today().strftime("%Y%m%d%H%M%S")
    file_handler = logging.FileHandler(f'{fname}.log')
    file_handler.setFormatter(formatter)
    logger.addHandler(file_handler)
    return logger

def convert_single(src_path, dst_path):
    print(f'{src_path} -> {dst_path}')
    im = Image.open(src_path).convert("L")
    im.save(dst_path)

def single_task(srcdir, dstdir, input_queue, log_queue):
    while True:
        try:
            src_path = input_queue.get(timeout = 1)
        except:
            break

        src_relpath = src_path[len(srcdir):]
        while src_relpath[0] in ['/','\\']:
            src_relpath = src_relpath[1:]

        dst_path = os.path.join(dstdir, src_relpath)
        dst_fdir = os.path.split(dst_path)[0]

        if not os.path.isdir(dst_fdir):
            try:
                os.makedirs(dst_fdir)
            except:
                pass # other process may make it as well

        try:
            convert_single(src_path, dst_path)
        except:
            print(traceback.format_exc())
            print(f'Failed to convert {src_path}')

def run_multiprocess_task(srcdir, dstdir):
    log = create_logger()
    log_queue = multiprocessing.Queue()
    input_queue = multiprocessing.Queue()
    num_process = os.cpu_count()

    worker_procs = [multiprocessing.Process(
        target = single_task,
        args = (srcdir, dstdir, input_queue, log_queue),
    ) for i in range(num_process)]

    for rootdir, dirs, files in os.walk(srcdir):
        for fn in files:
            fnext = os.path.splitext(fn)[1].lower()
            if fnext == '.png':
                input_queue.put(os.path.join(rootdir, fn))

    for wp in worker_procs:
        wp.start()

    while True:
        alive = False
        for wp in worker_procs:
            if wp.is_alive():
                alive = True
                break
        if not alive:
            break
        try:
            to_log = log_queue.get(timeout = 1)
            log.info(to_log)
        except:
            continue

def print_usage():
    print(f'{sys.argv[0]} <source dir> <destination dir>')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print_usage()
        sys.exit(1)
    
    srcdir = os.path.abspath(sys.argv[1])
    dstdir = os.path.abspath(sys.argv[2])
    
    if not os.path.isdir(srcdir):
        print(f'{srcdir} does not exist')
        sys.exit(1)

    if not os.path.isdir(dstdir):
        print(f'{dstdir} does not exist. creating one')
        os.makedirs(f'{dstdir}')

    run_multiprocess_task(srcdir, dstdir)