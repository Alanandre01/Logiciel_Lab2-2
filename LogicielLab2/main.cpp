// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include <new>
#include <windows.h>
#include <dshow.h>
#include <string>
#include <Python.h>
#include "playback.h"
#include "video.h"

#pragma comment(lib, "strmiids.lib")
using namespace std;

DShowPlayer* g_pPlayer = NULL;

//static PyObject* random(PyObject* self, PyObject* args) {
//    /* initialize random seed: */
//    srand(time(NULL));
//    int random = rand() % 10;
//
//    PyObject* python_val = Py_BuildValue("i", random);
//    return python_val;
//}
//
//static PyObject* add(PyObject* self, PyObject* args) {
//    int x = 0, y = 0, s = 0;
//
//    PyArg_ParseTuple(args, "ii", &x, &y);
//    s = x + y;
//    PyObject* python_val = Py_BuildValue("i", s);
//    return python_val;
//}

void OnChar(wchar_t c)
{
    switch (c)
    {

    case L'p':
    case L'P':
        if (g_pPlayer->State() == STATE_RUNNING)
        {
            g_pPlayer->Pause();
        }
        else
        {
            g_pPlayer->Play();
        }
        break;

    case L's':
    case L'S':
        if (g_pPlayer->State() == STATE_RUNNING)
        {
            g_pPlayer->SetRate(2.0);
        }
        break;

    case L'r':
    case L'R':
        if (g_pPlayer->State() == STATE_RUNNING || g_pPlayer->State() == STATE_PAUSED)
        {
            REFERENCE_TIME rtNow = 0;
            g_pPlayer->SetPositions(&rtNow);

        }
        break;

    /*case L's':
    case L'S':
        if (g_pPlayer->State() == STATE_RUNNING || g_pPlayer->State() == STATE_PAUSED)
        {
            g_pPlayer->Stop();
        }
        break;*/


    case L'q':
    case L'Q':
        if (g_pPlayer->State() == STATE_RUNNING || g_pPlayer->State() == STATE_PAUSED)
        {
            exit(0);
        }
        break;
    }
}

static PyObject* initModule(PyObject* self, PyObject* args)
{
    g_pPlayer = new DShowPlayer(PyUnicode_AsUTF8(args));
    g_pPlayer->Play();

    PyObject* python_val = Py_BuildValue("");
    return python_val;
}

static PyObject* inputChar(PyObject* self, PyObject* args)
{
    OnChar(*PyUnicode_AsUTF8(args));
    
    PyObject* python_val = Py_BuildValue("");
    return python_val;
}

static PyMethodDef methods[] = {
    { "initModule", (PyCFunction)initModule, METH_O, nullptr },
    { "inputChar", (PyCFunction)inputChar, METH_O, nullptr },
    //{ "random", (PyCFunction)random, METH_NOARGS, nullptr },
    //{ "add", (PyCFunction)add, METH_VARARGS, nullptr },
    { nullptr, nullptr, 0, nullptr }
};


static PyModuleDef LogicielLab2 = {
    PyModuleDef_HEAD_INIT,
    "LogicielLab2", /* name of module */
    NULL, /* module documentation, may be NULL */
    0,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    methods
};

PyMODINIT_FUNC PyInit_LogicielLab2() {
    return PyModule_Create(&LogicielLab2);
}