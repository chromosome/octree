from distutils.core import setup, Extension

octree_module = Extension('octree',
							 # include_dirs=['.'],
							 sources=['py_octree.cpp'],
							 extra_compile_args=['-std=c++17'],
							 language='c++'
							)

setup(name='octree', 
	  version='0.1.0', 
	  description='Quad Tree Data Structure',
	  author='Danilo Bonometti',
	  author_email='danilobonometti@udec.cl',
	  ext_modules=[octree_module])
