from PIL import Image
import multiprocessing
import io
import os
import sys
import traceback

def convert_single(srcdir, dstdir, input_queue):
	while True:
		try:
			src_fn = input_queue.get(timeout = 1)
		except:
			break

		try:
			dst_fn = os.path.split(src_fn)[1]
			dst_fn = os.path.splitext(dst_fn)[0] + '.webp'
			dst_fn = os.path.join(dstdir, dst_fn)

			im = Image.open(src_fn).convert('RGB')
			im.save(dst_fn, quality=50, method=6)
			im.close()

			print(f'{src_fn} -> {dst_fn}')
		except:
			print(traceback.format_exc())
			print(f'Failed to convert {src_fn}')

def convert_multiprocess(srcdir, dstdir):
	if not os.path.isdir(srcdir):
		print(f'{srcdir} does not exist!')
		return

	srcdir = os.path.abspath(srcdir)
	dstdir = os.path.abspath(dstdir)

	input_queue = multiprocessing.Queue()
	num_process = os.cpu_count()

	worker_procs = [multiprocessing.Process(target=convert_single, args=(srcdir, dstdir, input_queue)) for i in range(num_process)]

	for rootdir, dirs, files in os.walk(srcdir):
		for fn in files:
			fnext = os.path.splitext(fn)[1].lower()
			if fnext == '.png':
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