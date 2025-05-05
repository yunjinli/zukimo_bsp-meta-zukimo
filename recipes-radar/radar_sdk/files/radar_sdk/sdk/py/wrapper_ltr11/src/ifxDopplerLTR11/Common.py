# ===========================================================================
# Copyright (C) 2022 Infineon Technologies AG
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# ===========================================================================

import pathlib
import platform
from ctypes import *
import numpy as np
import typing
import functools

from .Error import *


def call_once(func):
    """Decorator that caches the return value of the decorated function

    The decorated function is called only the first time it is invoked and the
    return value is saved. Subsequent calls return the saved value of the first
    call, the decorated function is no longer called.

    The cached value of the first call is returned even if the input parameters
    change.
    """
    cache = {}

    def inner(*args, **kwargs):
        if func not in cache:
            cache[func] = func(*args, **kwargs)
        return cache[func]
    return inner


def handle_errors(dll: CDLL):
    """Check error code and raise exception on errors

    The decorator invokes the decorated function. After the function call
    check_rc is called to check for errors and raise an exception if an error
    occurred.

    Parameters:
        dll: Loaded shared SDK library
    """
    def decorator(function):
        @functools.wraps(function) # update attributes of decorator like docstrings
        def inner(*args, **kwargs):
            result = function(*args, **kwargs)
            check_rc(dll)
            return result
        return inner
    return decorator


def check_rc(dll: CDLL, error_code: typing.Optional[int] = None) -> None:
    """Raise exception on errors

    If error_code is not given (or None), the function checks if an error
    occurred inside the SDK by calling ifx_error_get_and_clear(). If an error
    occurred the corresponding Python exception is raised. If no error occurred,
    the function does nothing.

    If error_code is given the appropriate exception is raised if error_code is
    not IFX_OK.

    Parameters:
        dll: Loaded shared SDK library
        error_code: Error code
    """
    if error_code is None:
        error_code = dll.ifx_error_get_and_clear()

    if error_code:
        error_description = dll.ifx_error_to_string(error_code).decode("ascii")
        raise_exception_for_error_code(error_code, error_description)


class CubeReal(Structure):
    """Wrapper for real cube (ifx_Cube_R_t)"""
    _fields_ = (('d', POINTER(c_float)),
                ('rows', c_uint32),
                ('cols', c_uint32),
                ('slices', c_uint32),
                ('stride', c_uint32*3),
                ('owns_d', c_uint8, 1))

    @classmethod
    def from_numpy(cls, np_cube: np.ndarray):
        """Create a real cube from a numpy matrix"""
        rows, cols, slices = np_cube.shape

        # copy cube
        np_cube = np.array(np_cube, dtype=np.float32, order="C")

        # We do not copy data but create a view. It is crucial that the memory
        # of the cube np_cube is not released as long as this object
        # exists. For this reason we assign np_cube to this object. This way
        # the memory of np_cube is not released before our current object is
        # released.
        d = np_cube.ctypes.data_as(POINTER(c_float))
        stride = (1, cols, cols*slices)
        mat = CubeReal(d, rows, cols, slices, stride, 0)
        mat.np_cube = np_cube  # avoid that memory of np_cube is freed
        return mat

    def to_numpy(self) -> np.ndarray:
        """Convert cube to a 3D numpy array

        The method currently assumes that the matrix is contiguous and in C
        order (row-major order).
        """
        shape = (self.rows, self.cols, self.slices)
        data = np.ctypeslib.as_array(self.d, shape)
        return np.array(data, order="C", copy=True)

class Complex(Structure):
    """Wrapper for complex number (ifx_Complex_t)"""
    _fields_ = (('data', c_float*2),)

class CubeComplex(Structure):
    """Wrapper for complex cube (ifx_Cube_C_t)"""
    _fields_ = (('d', POINTER(Complex)),
                ('rows', c_uint32),
                ('cols', c_uint32),
                ('slices', c_uint32),
                ('stride', c_uint32*3),
                ('owns_d', c_uint8))

    @classmethod
    def from_numpy(cls, np_cube : np.ndarray):
        """Create a complex cube from a numpy matrix"""
        rows, cols, slices = np_cube.shape

        # copy cube
        np_cube = np.array(np_cube, dtype=np.complex64, order="C")

        # We do not copy data but create a view. It is crucial that the memory
        # of the cube np_cube is not released as long as this object
        # exists. For this reason we assign np_cube to this object. This way
        # the memory of np_cube is not released before our current object is
        # released.
        d = np_cube.ctypes.data_as(POINTER(Complex))
        stride = (1, cols, cols*slices)
        cube = CubeComplex(d, rows, cols, slices, stride, 0)
        cube.np_cube = np_cube # avoid that memory of np_cube is freed
        return cube

    def to_numpy(self) -> np.ndarray:
        """Convert cube to a 3D numpy array

        The method currently assumes that the matrix is contiguous and in C
        order (row-major order).
        """
        shape = (self.rows, self.cols, self.slices)
        real_elems = 2*shape[0]*shape[1]*shape[2]

        float_ptr = cast(self.d, POINTER(c_float))
        array = np.ctypeslib.as_array(float_ptr, (real_elems,))
        real = array[0::2].reshape(shape) # real parts
        imag = array[1::2].reshape(shape) # imaginary parts
        return real + 1j*imag

class MatrixReal(Structure):
    """Wrapper for real matrices (ifx_Cube_R_t)"""
    _fields_ = (('d', POINTER(c_float)),
                ('rows', c_uint32),
                ('cols', c_uint32),
                ('stride', c_uint32*2),
                ('owns_d', c_uint8, 1))

    @classmethod
    def from_numpy(cls, np_mat: np.ndarray):
        """Create a real matrix from a numpy matrix"""
        rows, cols = np_mat.shape

        # copy matrix
        np_mat = np.array(np_mat, dtype=np.float32, order="C")

        # We do not copy data but create a view. It is crucial that the memory
        # of the matrix np_mat is not released as long as this object
        # exists. For this reason we assign np_mat to this object. This way
        # the memory of np_mat is not released before our current object is
        # released.
        d = np_mat.ctypes.data_as(POINTER(c_float))

        stride = (1, cols)
        mat = MatrixReal(d, rows, cols, stride, 0)
        mat.np_mat = np_mat  # avoid that memory of np_mat is freed
        return mat

    def to_numpy(self) -> np.ndarray:
        """Convert matrix to a 2D numpy array

        The method currently assumes that the matrix is contiguous and in C
        order (row-major order).
        """
        shape = (self.rows, self.cols)
        data = np.ctypeslib.as_array(self.d, shape)
        return np.array(data, order="C", copy=True)


class VectorComplex(Structure):
    """Wrapper for the ifx_Vector_C_t structure"""
    _fields_ = (('d', POINTER(Complex)),
                ('len', c_uint32),
                ('stride', c_uint32, 31),
                ('owns_d', c_uint32, 1))

    @classmethod
    def from_numpy(cls, np_vector: np.ndarray):
        """Create a complex vector structure from a numpy vector"""
        len = np_vector.size
        np_vector = np.array(np_vector, dtype=np.complex64, order="C")
        data = np_vector.ctypes.data_as(POINTER(Complex))

        vector = VectorComplex(data, len, 31, 1)
        vector.np_vector = np_vector  # avoid that memory of np_vector is freed

        return vector

    def to_numpy(self) -> np.ndarray:
        """Convert vector structure to a numpy array"""
        data_array_size = self.len * 2
        data_array = np.ctypeslib.as_array(cast(
            self.d, POINTER(c_float)), (data_array_size,))

        return data_array[0::2].reshape(self.len) + 1j*data_array[1::2].reshape(self.len)


def find_library(libname: str):
    """Find path to shared library

    The functions searches for the shared library in the directory of the
    current script.

    Parameters:
        libname: Name of SDK library without prefix "lib" and without suffix (e.g., "radar_sdk")
    """
    system = platform.system()
    if system == "Windows":
        dll_name = f"{libname}.dll"
    elif system == "Linux":
        dll_name = f"lib{libname}.so"
    elif system == "Darwin":
        dll_name = f"lib{libname}.dylib"
    else:
        raise RuntimeError(f"System '{system}' not supported")

    script_dir = pathlib.Path(__file__).resolve().parent
    dll_path = script_dir / dll_name

    if not dll_path.is_file():
        raise FileNotFoundError(f"Cannot find file {dll_path}")

    return str(dll_path)


def load_library(libname: str) -> CDLL:
    """Load SDK library and specify prototypes of common functions

    Load the SDK library and declare prototypes of SDK functions used by all
    wrappers. If system is not supported or the shared library cannot be found,
    an exception is raised.

    Parameters:
        libname: Name of SDK library without suffix (e.g., radar_sdk)
    """
    dll = CDLL(find_library(libname))

    # version
    declare_prototype(dll, "ifx_sdk_get_version_string", None, c_char_p)
    declare_prototype(dll, "ifx_sdk_get_version_string_full", None, c_char_p)

    # vectors, matrices, cubes
    declare_prototype(dll, "ifx_mat_destroy_r", [POINTER(MatrixReal)], None)
    declare_prototype(dll, "ifx_cube_destroy_r", [POINTER(CubeReal)], None)

    # error
    declare_prototype(dll, "ifx_error_to_string", [c_int], c_char_p)
    declare_prototype(dll, "ifx_error_get_and_clear", None, c_int)

    # list
    declare_prototype(dll, "ifx_list_destroy", [c_void_p], None)
    declare_prototype(dll, "ifx_list_size", [c_void_p], c_size_t)
    declare_prototype(dll, "ifx_list_get", [c_void_p, c_size_t], c_void_p)

    # memory management
    declare_prototype(dll, "ifx_mem_free", [c_void_p], None)

    return dll


def get_sdk_version(dll: CDLL, full: bool = False) -> str:
    """Return SDK version string (excluding git tag from which it was built)

    If full is false, the major, minor, and patch number of the SDK version is
    returned as a string in the format "X.Y.Z". If full is true, the full
    version information including git tag is returned.
    """
    if full:
        return dll.ifx_sdk_get_version_string_full().decode("ascii")
    else:
        return dll.ifx_sdk_get_version_string().decode("ascii")


class ifxStructure(Structure):
    """Wrapper for C structs

    This is a wrapper around ctypes.Structure with more convenience like
    converting a structure to a dictionary.
    """

    def _initialize_fields(self, dictionary: dict) -> None:
        for (name, _) in self._fields_:
            setattr(self, name, dictionary[name])

    def _get_field(self, field_name: str, decode_byte_str: bool = False) -> object:
        """Get value of field

        Return string representation of member. This function might not work
        for more complex types like pointers or nested structures.

        Parameters:
            field_name: Name of field
            decode_byte_str: If true decode byte arrays into string assuming ASCII encoding
        """
        v = getattr(self, field_name)
        if decode_byte_str and type(v) == bytes:
            return v.decode("ascii")
        else:
            return v

    def _get_field_as_str(self, field_name: str) -> str:
        """Convert structure to dictionary

        Return string representation of field. This function might not work for
        more complex types like pointers or nested structures.

        Parameters:
            field_name: Name of field
        """
        return str(getattr(self, field_name))

    def to_dict(self, decode_byte_str: bool = False) -> dict:
        """Convert structure to dictionary

        For each member _get_field_as_str is called.

        Parameters:
            decode_byte_str: If true decode byte arrays into string assuming ASCII encoding
        """
        return {field_name: self._get_field(field_name, decode_byte_str) for field_name, _ in self._fields_}

    def __repr__(self) -> str:
        """Return string representation of object"""
        l = [f"{self.__class__.__name__}:"]
        for field_name, field_type in self._fields_:
            l.append(f"    {field_name}: {self._get_field_as_str(field_name)}")
        return "\n".join(l)


def declare_prototype(dll: CDLL, function_name: str, argtypes, restype) -> None:
    """Declare prototype

    Tell ctypes the argument types and the return type of the C function with
    name function_name.

    Parameters:
        dll: Loaded shared SDK library
        function_name: Name of SDK function
        argtypes: List of arguments or None if no arguments
        restype: Return type or None if function does not return a value
    """
    f = getattr(dll, function_name)
    f.restype = restype
    f.argtypes = argtypes
    setattr(dll, function_name, f)
