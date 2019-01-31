from distutils.core import setup, Extension
setup(name='siphash', version='1.0', ext_modules=[Extension('siphash', ['siphash.c'])])