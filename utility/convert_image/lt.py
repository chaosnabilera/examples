import datetime
import logging

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

if __name__ == '__main__':
    log = create_logger()
    for i in range(10):
        log.info('ha'*(i+1))