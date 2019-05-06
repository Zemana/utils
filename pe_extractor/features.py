import os
try:
    import pefile
except ModuleNotFoundError:
    !pip install pefile
    import pefile


def extract_imports(pe):
    imports = {}

    if not hasattr(pe, "DIRECTORY_ENTRY_IMPORT"):
        return imports

    for entry in pe.DIRECTORY_ENTRY_IMPORT:

        entries = []
        for imp in entry.imports:
            if imp.name is None:
                entries.append(str(imp.ordinal))
            else:
                entries.append(imp.name.decode().lower())

        dll = entry.dll
        libname = dll.decode().lower() if isinstance(dll, bytes) else dll.lower

        imports[libname] = entries

    return imports


def combine_imports(imports, size):
    imports = [lib + f for lib, funcs in imports.items() for f in funcs]
    return ''.join(imports).encode('latin-1')


def extract_exports(pe, size):
    exports = []

    if not hasattr(pe, "DIRECTORY_ENTRY_EXPORT"):
        return b''

    for sym in pe.DIRECTORY_ENTRY_EXPORT.symbols:
        if sym.name is None:
            exports.append(str(sym.ordinal))
        else:
            exports.append(sym.name.decode())

    return ''.join(exports).encode('latin-1')


def extract_entry_point(pe, size):
    exports = []

    ep = pe.OPTIONAL_HEADER.AddressOfEntryPoint
    return pe.get_memory_mapped_image()[ep:ep+size]


def extract_code_entropy(pe, size):
    ep = pe.OPTIONAL_HEADER.AddressOfEntryPoint

    for section in pe.sections:
        if section.contains_rva(ep):
            # Found code section
            return str(section.get_entropy()).encode('latin-1')

    return b''


def extract_entropies(pe, size):
    entropies = ''
    for section in pe.sections:
            entropies += str(section.get_entropy())[:5]

    return entropies.encode('latin-1')


def get_resource_offset(pe):
    resource_attribute = pe.OPTIONAL_HEADER.DATA_DIRECTORY[2]

    rva = resource_attribute.VirtualAddress
    size = resource_attribute.Size

    if rva == 0 or size == 0:
        raise AttributeError("Could not get rva: %d or size: %d" % (rva, size))

    for sec in pe.sections:
        va = sec.VirtualAddress
        if va <= rva and va + sec.Misc_VirtualSize > rva:
            return (rva - va + sec.PointerToRawData, size)

    raise AttributeError("Could not calculate offset")


def extract_resources(pe, path, size):
    try:
        attr_offset, attr_size = get_resource_offset(pe)
    except AttributeError as e:
        return None

    if size > attr_size:
        size = attr_size

    f = open(path, 'rb')
    f.seek(attr_offset)
    data = f.read(size)
    f.close()

    return data


def extract_security_attrs(pe, path, size):
    security_attribute = pe.OPTIONAL_HEADER.DATA_DIRECTORY[4]

    attr_offset = security_attribute.VirtualAddress
    attr_size = security_attribute.Size

    if size > attr_size:
        size = attr_size

    f = open(path, 'rb')
    f.seek(attr_offset)
    data = f.read(size)
    f.close()

    return data


def extract_header(pe, path, size):
    file_size = os.path.getsize(path)
    if size > file_size:
        size = file_size

    f = open(path, 'rb')
    data = f.read(size)
    f.close()

    return data


def extract_size(pe, path, size):
    return '{:0{}X}'.format(os.path.getsize(path), size).encode('latin-1')
