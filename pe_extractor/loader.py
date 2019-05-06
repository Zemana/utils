import os
import sys
import random
import numpy as np


def get_dir_list(directory):
    paths = []
    if not os.path.exists(directory):
        raise FileNotFoundError('No such directory: {}'.format(directory))

    for root, dirs, files in os.walk(directory):
        for d in dirs:
            paths.append(os.path.join(root, d))

    return paths


def read_data(path, size):
    if not os.path.exists(path):
        return '~' * 10

    f = open(path, 'rb')
    data = f.read(size)
    f.close()

    if len(data) != size:
        raise AttributeError("Not enough data: path: %s expected:%s got:%s" %
                             (path, size, len(data)))

    return data.decode('latin-1')


def load_file(path, **feature_dict):
    data_list = []

    for name, size in feature_dict.items():
        fpath = os.path.join(path, name)

        data_list.append(read_data(fpath, size))

    d = ''.join(data_list)

    return np.frombuffer(d.encode('latin-1'), dtype=np.uint8)


def load_files(paths, **feature_dict):
    files = {}

    total = len(paths)
    for i, path in enumerate(paths):
        sys.stdout.write("\r{}/{}".format(i + 1, total))
        sys.stdout.flush()

        files[path] = load_file(path, **feature_dict)

    return files


def load_features(directories, output='out', **feature_dict):
    if len(directories) != 4:
        raise AttributeError("The format: train/malware, train/clean, " +
                             "test/malware, test/clean")

    x_train, y_train, x_test, y_test = [], [], [], []

    directories = [os.path.join(output, d) for d in directories]

    paths_train = get_dir_list(directories[0]) + get_dir_list(directories[1])
    paths_test = get_dir_list(directories[2]) + get_dir_list(directories[3])

    random.shuffle(paths_train)
    random.shuffle(paths_test)

    files = load_files(paths_train, **feature_dict)
    for path, data in files.items():
        label = 1 if path.startswith(directories[0]) else 0
        x_train.append(data)
        y_train.append(label)

    files = load_files(paths_test, **feature_dict)
    for path, data in files.items():
        label = 1 if path.startswith(directories[2]) else 0
        x_test.append(data)
        y_test.append(label)

    x_train = np.asarray(x_train, dtype=np.uint8)
    y_train = np.asarray(y_train, dtype=np.uint8)
    x_test = np.asarray(x_test, dtype=np.uint8)
    y_test = np.asarray(y_test, dtype=np.uint8)

    return x_train, y_train, x_test, y_test
