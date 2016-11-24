#/usr/bin/python

import sys
import logging
import tempfile
import argparse

def parse_args():
    """
    Handles CLI options using argparse
    """
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-p", "--path",
        help="Path to output file",
        action="store",
        type=str,
        default=tempfile.mkstemp()[1],
    )
    return parser.parse_args()

def get_logger(path):
    logger = logging.getLogger(__name__)
    file_handler = logging.FileHandler(path)
    std_handler = logging.StreamHandler(sys.stdout)
    formatter = logging.Formatter('%(asctime)s - %(message)s')
    file_handler.setFormatter(formatter)
    std_handler.setFormatter(formatter)
    logger.setLevel(logging.INFO)
    logger.addHandler(file_handler)
    logger.addHandler(std_handler)
    return logger

def listen(stream, logger):
    while True:
        line = stream.readline().strip()
        if line.isdigit():
            logger.info(line)
        if line == "---":
            logger.info("invalid")


if __name__ == "__main__":
    args = parse_args()
    logger = get_logger(args.path)
    try:
        sys.stdout.write("Logging to: %s \n" % args.path)
        listen(sys.stdin, logger)
    except KeyboardInterrupt:
        sys.exit(0)
