#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <python3.8/structmember.h>

#include "octree.h"

using namespace std;

typedef struct {
	PyObject_HEAD;
	PyObject* tree;
} octree;

static PyMemberDef octree_members[] = {
	{ "tree", T_OBJECT, offsetof(octree, tree), 0, "Octree" },
	{ NULL }
};

static void octree_dealloc(octree* self) {
	Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject* octree_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
	double x;
	double y;
	double z;

	if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z))
		return NULL;

	octree* self;

	self = (octree*)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->tree = (PyObject*) new dsa::octree(x, y, z);
	}

	return (PyObject*) self;
}

static int octree_init(octree* self, PyObject* args, PyObject* kwargs) {
	return 0;
}

// static PyObject* octree_dims(octree* self) {
// 	PyObject* dims = PyTuple_New(2);

// 	auto [lat, lon] = ((dsa::octree*)(self->tree))->dimensions();

// 	PyTuple_SET_ITEM(dims, 0, PyFloat_FromDouble(lat));
// 	PyTuple_SET_ITEM(dims, 1, PyFloat_FromDouble(lon));

// 	return dims;
// }

static PyObject* octree_insert(octree* self, PyObject* args, PyObject* kwargs) {
	double x;
	double y;
	double z;

	if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z))
		return NULL;

	dsa::point_t p = {x, y, z};

	long inserted = ((dsa::octree*)(self->tree))->insert(p);

	return PyBool_FromLong(inserted);
}

// static PyObject* octree_remove(octree* self, PyObject* args, PyObject* kwargs) {
// 	double lat;
// 	double lon;

// 	if (!PyArg_ParseTuple(args, "dd", &lat, &lon))
// 		return NULL;

// 	dsa::point_t p = {lat, lon};

// 	long removed = ((dsa::octree*)(self->tree))->remove(p);

// 	return PyBool_FromLong(removed);
// }

static PyObject* octree_find(octree* self, PyObject* args, PyObject* kwargs) {
	double x;
	double y;
	double z;

	if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z))
		return NULL;

	vector<dsa::octant::side_e> path;
	dsa::point_t p = {x, y, z};

	long found = ((dsa::octree*)(self->tree))->find(p);

	return PyBool_FromLong(found);
}

static PyObject* octree_track(octree* self, PyObject* args, PyObject* kwargs) {
	double x;
	double y;
	double z;

	if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z))
		return NULL;

	vector<dsa::octant::side_e> path;
	dsa::point_t p = {x, y, z};

	path = ((dsa::octree*)(self->tree))->track(p);

	PyObject* tracks = PyList_New(path.size());

	for (size_t i = 0; i < path.size(); ++i) {
		string t = dsa::oct_map[path[i]];
		PyList_SET_ITEM(tracks, i, PyUnicode_FromString(t.c_str()));
	}

	return tracks;
}

// static PyObject* octree_region_search(octree* self, PyObject* args, PyObject* kwargs) {
// 	double lat;
// 	double lon;
// 	int dx;
// 	int dy;

// 	if (!PyArg_ParseTuple(args, "ddii", &lat, &lon, &dx, &dy))
// 		return NULL;

// 	dsa::quad_t q = {{lat, lon}, {dx, dy}};

// 	long points = ((dsa::octree*)(self->tree))->get_total_cities(q);

// 	return PyLong_FromLong(points);
// }

static PyMethodDef octree_methods[] = {
	// { "dims",   (PyCFunction) octree_dims,   METH_VARARGS, "dimensions of the tree" },
	{ "insert", (PyCFunction) octree_insert, METH_VARARGS, "inserts a point"        },
	// { "remove", (PyCFunction) octree_remove, METH_VARARGS, "removes a point"        },
	{ "find",   (PyCFunction) octree_find,   METH_VARARGS, "finds a point"          },
	{ "track",  (PyCFunction) octree_track,  METH_VARARGS, "tracks a point"         },
	// { "region_search",  (PyCFunction) octree_region_search,  METH_VARARGS, "query area points"},
	{NULL}
};

static PyTypeObject octree_type = {
	PyVarObject_HEAD_INIT(NULL, 0) 
	.tp_name = "octree.octree",								/* tp_name */
	.tp_basicsize = sizeof(octree),							/* tp_basicsize */
	.tp_dealloc = (destructor) octree_dealloc,				/* tp_dealloc */
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/* tp_flags */
	.tp_doc = "octree object",								/* tp_doc */
	.tp_methods = octree_methods,							/* tp_methods */
	.tp_members = octree_members,							/* tp_members */
	.tp_init = (initproc)octree_init,						/* tp_init */
	.tp_new = octree_new,									/* tp_new */
};

static struct PyModuleDef octree_module = {
	PyModuleDef_HEAD_INIT,
	"Octree",
	"Octree data structure",
	-1,
	octree_methods
};

PyMODINIT_FUNC PyInit_octree(void) {
	Py_Initialize();
	PyObject *m = PyModule_Create(&octree_module);

	if (PyType_Ready(&octree_type) < 0)
	return NULL;

	Py_INCREF(&octree_type);
	if (PyModule_AddObject(m, "octree", (PyObject *)&octree_type) < 0) {
		Py_DECREF(&octree_type);
		Py_DECREF(m);
		return NULL;
	}

	return m;
}
