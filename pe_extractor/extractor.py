import os
import sys


def dump_output(path, data):
    f = open(path, 'wb')

    if data is None:
        f.write(b'\0')
    else:
        f.write(data)

    f.close()


def get_path_list(directory):
    paths = []
    if not os.path.exists(directory):
        raise FileNotFoundError('No such directory: {}'.format(directory))

    for root, dirs, files in os.walk(directory):
        for file in files:
            paths.append(os.path.join(root, file))

    return paths


def pad_or_trunc(data, size):
    if data is None:
        return b'~' * size

    if len(data) < size:
        return data + b'~' * (size - len(data))
    else:
        return data[:size]


def dump_file(path, **feature_dict):
    result = {}

    try:
        pe = pefile.PE(path, fast_load=True)
    except FileNotFoundError as e:
        print("%s: %s" % (path, e))
        return result
    except pefile.PEFormatError as e:
        print("%s: %s" % (path, e))
        return result

    pe.parse_data_directories(directories=[
        pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_IMPORT'],
        pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_EXPORT'],
        pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_RESOURCE']])

    for name, size in feature_dict.items():
        if name == 'imports':
            data = combine_imports(extract_imports(pe), size)
        elif name == 'exports':
            data = extract_exports(pe, size)
        elif name == 'entry_point':
            data = extract_entry_point(pe, size)
        elif name == 'code_entropy':
            data = extract_code_entropy(pe, size)
        elif name == 'entropies':
            data = extract_entropies(pe, size)
        elif name == 'icon':
            data = get_main_icon(pe, path, size)
        elif name == 'resources':
            data = extract_resources(pe, path, size)
        elif name == 'security':
            data = extract_security_attrs(pe, path, size)
        elif name == 'header':
            data = extract_header(pe, path, size)
        elif name == 'size':
            data = extract_size(pe, path, size)
        else:
            raise AttributeError("Unkown option: %s" % (name))

        data = pad_or_trunc(data, size)

        result[name] = (data, size)

    return result


def dump_files(directory, output, **feature_dict):
    paths = get_path_list(directory)

    total = len(paths)
    for i, path in enumerate(paths):
        sys.stdout.write("\r{}: {}/{}".format(directory, i + 1, total))
        sys.stdout.flush()

        output_path = os.path.join(output, path)
        if not os.path.exists(output_path):
            os.makedirs(output_path)

        features = dump_file(path, **feature_dict)
        for name, data in features.items():
            dump_output(os.path.join(output, path, name), data[0])


def extract_features(directories, output='out', **feature_dict):
    for d in directories:
        dump_files(d, output, **feature_dict)
