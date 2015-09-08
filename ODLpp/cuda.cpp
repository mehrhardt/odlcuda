// Disable deprecated API
#include <numpy/numpyconfig.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <stdint.h>

// Thrust bug...
#define DEBUG 1
#include <boost/python.hpp>
#include <boost/python/slice.hpp>
#include <iostream>
#define _DEBUG 1
#include <thrust/device_vector.h>
#include <numpy/arrayobject.h>

#include <LCRUtils/python/numpy_utils.h>
#include <ODLpp/DeviceVector.h>
#include <ODLpp/TypeMacro.h>
#include <ODLpp/CudaVector.h>

using namespace boost::python;

// Externally (CUDA) compiled

// Transformations
extern void absImpl(const DeviceVector<float>& source,
                    DeviceVector<float>& target);
extern void negImpl(const DeviceVector<float>& source,
                    DeviceVector<float>& target);

// Reductions
extern float sumImpl(const DeviceVector<float>& v);

// Functions
extern void convImpl(const DeviceVector<float>& source,
                     const DeviceVector<float>& kernel,
                     DeviceVector<float>& target);

extern void forwardDifferenceImpl(const DeviceVector<float>& source,
                                  DeviceVector<float>& target);
extern void forwardDifferenceAdjointImpl(const DeviceVector<float>& source,
                                         DeviceVector<float>& target);

extern void forwardDifference2DImpl(const DeviceVector<float>& source,
                                    DeviceVector<float>& dx,
                                    DeviceVector<float>& dy, int cols,
                                    int rows);
extern void forwardDifference2DAdjointImpl(const DeviceVector<float>& dx,
                                           const DeviceVector<float>& dy,
                                           DeviceVector<float>& target,
                                           int cols, int rows);

extern void maxVectorVectorImpl(const DeviceVector<float>& v1,
                                const DeviceVector<float>& v2,
                                DeviceVector<float>& target);
extern void maxVectorScalarImpl(const DeviceVector<float>& v1, float scalar,
                                DeviceVector<float>& target);
extern void divideVectorVectorImpl(const DeviceVector<float>& dividend,
                                   const DeviceVector<float>& divisor,
                                   DeviceVector<float>& quotient);

extern void addScalarImpl(const DeviceVector<float>& v1, float scalar,
                          DeviceVector<float>& target);
extern void signImpl(const DeviceVector<float>& v1,
                     DeviceVector<float>& target);
extern void sqrtImpl(const DeviceVector<float>& v1,
                     DeviceVector<float>& target);

// Functions
void convolution(const CudaVectorImpl<float>& source,
                 const CudaVectorImpl<float>& kernel,
                 CudaVectorImpl<float>& target) {
    convImpl(source, kernel, target);
}

void forwardDifference(const CudaVectorImpl<float>& source,
                       CudaVectorImpl<float>& target) {
    forwardDifferenceImpl(source, target);
}

void forwardDifferenceAdjoint(const CudaVectorImpl<float>& source,
                              CudaVectorImpl<float>& target) {
    forwardDifferenceAdjointImpl(source, target);
}

void forwardDifference2D(const CudaVectorImpl<float>& source,
                         CudaVectorImpl<float>& dx, CudaVectorImpl<float>& dy,
                         int cols, int rows) {
    forwardDifference2DImpl(source, dx, dy, cols, rows);
}

void forwardDifference2DAdjoint(const CudaVectorImpl<float>& dx,
                                const CudaVectorImpl<float>& dy,
                                CudaVectorImpl<float>& target, int cols,
                                int rows) {
    forwardDifference2DAdjointImpl(dx, dy, target, cols, rows);
}

void maxVectorVector(const CudaVectorImpl<float>& v1,
                     const CudaVectorImpl<float>& v2,
                     CudaVectorImpl<float>& target) {
    maxVectorVectorImpl(v1, v2, target);
}

void maxVectorScalar(CudaVectorImpl<float>& source, float scalar,
                     CudaVectorImpl<float>& target) {
    maxVectorScalarImpl(source, scalar, target);
}

void divideVectorVector(const CudaVectorImpl<float>& dividend,
                        const CudaVectorImpl<float>& divisor,
                        CudaVectorImpl<float>& quotient) {
    divideVectorVectorImpl(dividend, divisor, quotient);
}

void addScalar(const CudaVectorImpl<float>& source, float scalar,
               CudaVectorImpl<float>& target) {
    addScalarImpl(source, scalar, target);
}

void signVector(const CudaVectorImpl<float>& source,
                CudaVectorImpl<float>& target) {
    signImpl(source, target);
}

void sqrtVector(const CudaVectorImpl<float>& source,
                CudaVectorImpl<float>& target) {
    sqrtImpl(source, target);
}

void absVector(CudaVectorImpl<float>& source, CudaVectorImpl<float>& target) {
    absImpl(source, target);
}

float sumVector(const CudaVectorImpl<float>& source) { return sumImpl(source); }

template <typename T>
void instantiateCudaVector(const std::string& name) {
    class_<CudaVectorImpl<T>>(name.c_str(), "Documentation", init<size_t>())
        .def(init<size_t, T>())
        .def("from_pointer", &fromPointer<T>)
        .staticmethod("from_pointer")
        .def("copy", &CudaVectorImpl<T>::copy)
        .def(self_ns::str(self_ns::self))
        .def("__repr__", &repr<T>)
        .def("data_ptr", &CudaVectorImpl<T>::dataPtr)
        .add_property("dtype", &dtype<T>)
        .add_property("shape", &shape<T>)
        .add_property("size", &CudaVectorImpl<T>::size)
        .def("__len__", &CudaVectorImpl<T>::size)
        .def("equals", &CudaVectorImpl<T>::allEqual)
        .def("__getitem__", &CudaVectorImpl<T>::getItem)
        .def("__setitem__", &CudaVectorImpl<T>::setItem)
        .def("copy_device_to_host", &copyDeviceToHost<T>)
        .def("get_to_host", &getSliceToHost<T>)
        .def("getslice", &getSliceView<T>)
        .def("setslice", &setSlice<T>)
        .def("lincomb", &CudaVectorImpl<T>::linComb)
        .def("inner", &CudaVectorImpl<T>::inner)
        .def("dist", &CudaVectorImpl<T>::dist)
        .def("norm", &CudaVectorImpl<T>::norm)
        .def("multiply", &CudaVectorImpl<T>::multiply)
        .def("fill", &CudaVectorImpl<T>::fill);
}

// Expose classes and methods to Python
BOOST_PYTHON_MODULE(odlpp_cuda) {
    auto result = _import_array();  // Import numpy
    if (result != 0) {
        PyErr_Print();
        throw std::invalid_argument("numpy.core.multiarray failed to import");
    }
    boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

    def("conv", convolution);
    def("forward_diff", forwardDifference);
    def("forward_diff_adj", forwardDifferenceAdjoint);
    def("forward_diff_2d", forwardDifference2D);
    def("forward_diff_2d_adj", forwardDifference2DAdjoint);
    def("max_vector_vector", maxVectorVector);
    def("max_vector_scalar", maxVectorScalar);
    def("divide_vector_vector", divideVectorVector);
    def("add_scalar", addScalar);
    def("sign", signVector);
    def("sqrt", sqrtVector);
    def("abs", absVector);
    def("sum", sumVector);

    // Instatiate according to numpy

    // boolean
    // instantiateCudaVector<long>("CudaVectorInt");
    // instantiateCudaVector<int>("CudaVectorIntc");
    // instantiateCudaVector<size_t>("CudaVectorIntp");
    instantiateCudaVector<int8_t>("CudaVectorInt8");
    instantiateCudaVector<int16_t>("CudaVectorInt16");
    instantiateCudaVector<int32_t>("CudaVectorInt32");
    instantiateCudaVector<int64_t>("CudaVectorInt64");
    instantiateCudaVector<uint8_t>("CudaVectorUInt8");
    instantiateCudaVector<uint16_t>("CudaVectorUInt16");
    instantiateCudaVector<uint32_t>("CudaVectorUInt32");
    instantiateCudaVector<uint64_t>("CudaVectorUInt64");
    // instantiateCudaVector<double>("CudaVectorFloat");
    // Half precision
    instantiateCudaVector<float>("CudaVectorFloat32");
    instantiateCudaVector<double>("CudaVectorFloat64");
    // Complex
}
