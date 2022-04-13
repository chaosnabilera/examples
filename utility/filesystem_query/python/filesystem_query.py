import os
import shutil
import sys
import zipfile

from datetime import datetime

def print_summary(target_dir, report_path):
    if not os.path.isdir(target_dir):
        print(f'{target_dir} does not exist!')
        return

    record = []

    cnt = 0
    size_sum = 0
    ext_cnt = {}
    ext_sizesum = {}

    min_ctime, max_ctime = float('inf'),float('-inf')
    min_ctime_path, max_ctime_path = None, None
    min_mtime, max_mtime = float('inf'),float('-inf')
    min_mtime_path, max_mtime_path = None, None

    target_dir = os.path.abspath(target_dir)
    for rootdir, dirs, files in os.walk(target_dir):
        for fn in files:
            fullpath = os.path.join(rootdir, fn)
            filesize = os.path.getsize(fullpath)
            fileext = os.path.splitext(fullpath)[1]
            cur_ctime = os.path.getctime(fullpath)
            cur_mtime = os.path.getmtime(fullpath)

            cnt += 1
            size_sum += filesize

            if fileext not in ext_cnt:
                ext_cnt[fileext] = 0
            ext_cnt[fileext] += 1
            
            if fileext not in ext_sizesum:
                ext_sizesum[fileext] = 0
            ext_sizesum[fileext] += filesize

            if cur_ctime < min_ctime:
                min_ctime = cur_ctime
                min_ctime_path = fullpath
            if cur_ctime > max_ctime:
                max_ctime = cur_ctime
                max_ctime_path = fullpath
            if cur_mtime < min_mtime:
                min_mtime = cur_mtime
                min_mtime_path = fullpath
            if cur_mtime > max_mtime:
                max_mtime = cur_mtime
                max_mtime_path = fullpath

            fullpath = fullpath.replace(',','.')
            record.append((fullpath, filesize, fileext, str(datetime.fromtimestamp(cur_ctime)), str(datetime.fromtimestamp(cur_mtime))))

    if cnt == 0:
        print(f'No file exist in {target_dir}')

    print(f'File count : {cnt}')
    ss = size_sum
    if ss < 1024*1024*1024:
        ss = '{:.3f} MB'.format(ss/(1024*1024))
    else:
        ss = '{:.2f} GB'.format(ss/(1024*1024*1024))
    print(f'File size sum : {ss}')
    for ext in ext_cnt:
        ss = ext_sizesum[ext]
        if ss < 1024:
            ss = f'{ss} bytes'
        elif ss < 1024*1024:
            ss = '{:.2f} KB'.format(ss/1024)
        else:
            ss = '{:.2f} MB'.format(ss/(1024*1024))
        print(f'\t{ext} : {ext_cnt[ext]} files / {ss}')

    print(f'Min ctime: {str(datetime.fromtimestamp(min_ctime))} : {min_ctime_path}')
    print(f'Max ctime: {str(datetime.fromtimestamp(max_ctime))} : {max_ctime_path}')
    print(f'Min mtime: {str(datetime.fromtimestamp(min_mtime))} : {min_mtime_path}')
    print(f'Max mtime: {str(datetime.fromtimestamp(max_mtime))} : {max_mtime_path}')

    print(f'Writing csv to {report_path}')
    with open(report_path,'w',encoding='utf-8') as ofile:
        ofile.write('Path,Size,Extension,CTIME,MTIME\n')
        for r in record:
            ofile.write(','.join(map(str,r))+'\n')
    print('done')

def print_usage():
    print(f'Usage 1) print summary: {sys.argv[0]} summary <target_directory> <report filename>')
    print(f'Usage 2) filtered move: {sys.argv[0]} filter_move <src directory> <target directory>')

#
# move .avif files or zip files that have .avif files
#
def filtered_move(src_dir, dst_dir):
    if not os.path.isdir(src_dir):
        print(f'{src_dir} does not exist!')
        return

    src_dir, dst_dir = os.path.abspath(src_dir), os.path.abspath(dst_dir)
    if not os.path.isdir(dst_dir):
        os.makedirs(dst_dir)

    print(f'src:{src_dir}')
    print(f'dst:{dst_dir}')

    for rootdir, dirs, files in os.walk(src_dir):
        for fn in files:
            ext = os.path.splitext(fn)[1].lower()
            spath = os.path.join(rootdir, fn)

            if ext not in ['.avif','.zip']:
                continue

            if ext == '.zip':
                try:
                    contains_avif = False
                    zf = zipfile.ZipFile(spath)
                    for n in zf.namelist():
                        if n[-5:].lower() == '.avif':
                            # print(spath,n)
                            contains_avif = True
                            break
                    zf.close()
                    if not contains_avif:
                        continue
                except:
                    print(f'Error processing {spath}')
                    continue

            rpath = spath[len(src_dir):]
            while rpath[0] in ['/','\\']:
                rpath = rpath[1:]
            dpath = os.path.join(dst_dir, rpath)
            # print(f'spath:{spath}')
            # print(f'dpath:{dpath}')
            ddir = os.path.split(dpath)[0]
            if not os.path.isdir(ddir):
                os.makedirs(ddir)
            try:
                shutil.move(spath,dpath)
            except:
                print(f'Failed to move {spath}->{dpath}')

if __name__ == '__main__':
    if len(sys.argv) == 4:
        if sys.argv[1] == 'summary':
            print_summary(sys.argv[2], sys.argv[3])
        elif sys.argv[1] == 'filter_move':
            filtered_move(sys.argv[2], sys.argv[3])
    else:
        print_usage()