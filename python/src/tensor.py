from __future__ import annotations
from typing import Optional, Union, Sequence, List, Tuple, Any

import pytensor as _ts
import numpy as np

DataT = Union[_ts.MatrixF, _ts.MatrixI, _ts.VectorF, _ts.VectorI]
ArrayT = Union[DataT, np.array, List[List[float]]]
ScalarT = int


def _is_instance_of_tensor(o: DataT) -> bool:
    types = [_ts.MatrixF, _ts.MatrixI, _ts.VectorF, _ts.VectorI]
    for t in types:
        if isinstance(o, t):
            return True
    return False


def _map_dim_and_type_to_tensor(dim: int, t: Any) -> DataT:
    if dim == 1:
        if t == np.float32 or t is float:
            return _ts.VectorF
        if t == np.int32 or t is int:
            return _ts.VectorI
        else:
            raise ValueError(f"Type {t} is not supported!")
    elif dim == 2:
        if t == np.float32 or t is float:
            return _ts.MatrixF
        if t == np.int32 or t is int:
            return _ts.MatrixI
        else:
            raise ValueError(f"Type {t} is not supported!")
    else:
        raise ValueError(f"Dim {dim} is not supported!")


def _numpy_downcast(array: np.array) -> np.array:
    dtype = array.dtype
    if dtype in [np.int32, np.float32]:
        return array
    elif dtype == np.int64:
        return array.astype(np.int32)
    elif dtype == np.float64:
        return array.astype(np.float32)
    else:
        raise ValueError(f"Array type {dtype} is not supported!")


def _check_shape(shape: Sequence[int]):
    if len(shape) not in [1, 2]:
        msg = f"Tensor with dims other that 2D are not supported yet! Note that {len(shape)=}"
        raise ValueError(msg)


class Tensor:
    def __init__(self,
                 array: Optional[Union[ArrayT, ScalarT]] = None,
                 shape: Optional[Sequence[int]] = None):

        self._data: DataT
        self._shape: Sequence[int]

        if array is None and shape is None:
            raise ValueError("either array or dims should be passed to initialize object")

        array32: np.array

        if _is_instance_of_tensor(array):
            self._data = array
        elif isinstance(array, np.ndarray):
            array32 = _numpy_downcast(array)
            self._data = _map_dim_and_type_to_tensor(array32.ndim, array32.dtype)(array32)
        elif isinstance(array, List):
            array32 = _numpy_downcast(np.array(array))
            self._data = _map_dim_and_type_to_tensor(array32.ndim, array32.dtype)(array32)
        elif isinstance(array, ScalarT):
            array32 = _numpy_downcast(np.array([array]))
            self._data = _map_dim_and_type_to_tensor(array32.ndim, array32.dtype)(array32)
        elif array is not None:
            raise ValueError(f"Array type {type(array)} is not supported~")

        if shape:
            _check_shape(shape)
            self._data = _map_dim_and_type_to_tensor(len(shape), float)(*shape)

        self._shape = tuple(self._data.shape())

    @property
    def shape(self):
        return self._shape

    @property
    def dim(self):
        return len(self.shape)

    @property
    def data(self):
        return self._data

    @property
    def T(self):
        return Tensor(_ts.transpose(self._data))

    @property
    def numpy(self):
        return np.array(self._data)

    def __getitem__(self, item: Union[Tuple[int, int], int]) -> float:
        if isinstance(item, tuple):
            if len(item) != self.dim:
                raise IndexError(f"Index {item} is not compatible with Tensor with shape {self.shape}")
            return self._data[item]
        elif isinstance(item, int):
            return self._data[item]
        else:
            raise ValueError(f"Unsupported index: {item}")

    def __add__(self, other: Tensor) -> Tensor:
        return Tensor(_ts.add(self._data, other._data))

    def __mul__(self, other: Union[float, Tensor]) -> Tensor:
        if isinstance(other, float):
            other = Tensor(np.full(self.shape, other))
        return Tensor(_ts.multiply(self._data, other._data))

    def __rmul__(self, other: Union[float, Tensor]) -> Tensor:
        return self * other

    def __matmul__(self, other: Tensor) -> Tensor:
        return Tensor(_ts.dot(self._data, other._data))

    def __str__(self) -> str:
        return f"Tensor({self._shape[0]}, {self._shape[1]})"


def log(tensor: Tensor) -> Tensor:
    return Tensor(_ts.log(tensor.data))


def pow(tensor: Tensor, p: int) -> Tensor:
    return Tensor(_ts.pow(tensor.data, p))


def exp(tensor: Tensor) -> Tensor:
    return Tensor(_ts.exp(tensor.data))


def sum(tensor: Tensor, axis: int) -> Tensor:
    return Tensor(_ts.sum(tensor.data, axis))
