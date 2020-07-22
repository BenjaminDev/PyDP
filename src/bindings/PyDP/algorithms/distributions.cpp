// Provides bindings for distributions

#include "pybind11/pybind11.h"

#include "algorithms/distributions.h"

using namespace std;
namespace py=pybind11;
namespace dpi = differential_privacy::internal;

void declareLaplaceDistribution(py::module &m) {
    py::class_<dpi::LaplaceDistribution> laplace_dist(m, "LaplaceDistribution");
    laplace_dist.attr("__module__") = "pydp";
    laplace_dist.def(py::init<double, double>(), py::arg("epsilon") = 0., py::arg("sensitivity"))
        .def("get_uniform_double",
             &dpi::LaplaceDistribution::GetUniformDouble,
             "Returns a uniform random integer of in range [0, 2^53).")
        .def("sample",
             py::overload_cast<double>(&dpi::LaplaceDistribution::Sample),
             py::arg("scale") = 1.0,
             "Samples the Laplacian with distribution Lap(scale*b)")
        .def("get_diversity",
             &dpi::LaplaceDistribution::GetDiversity,
             "Returns the parameter defining this distribution, often labeled b.");

    laplace_dist.attr("__doc__") = "Allows sampling from a secure laplace distribution.";
}

void declareGaussianDistribution(py::module& m) {
  py::class_<dpi::GaussianDistribution> gauss_dist(m, "GaussianDistribution");
  gauss_dist.attr("__module__") = "pydp";
  gauss_dist.def(py::init<double>())
      .def("sample", (double (dpi::GaussianDistribution::*)()) &
                         dpi::GaussianDistribution::Sample)
      .def("sample", (double (dpi::GaussianDistribution::*)(double)) &
                         dpi::GaussianDistribution::Sample)
      .def("stddev", &dpi::GaussianDistribution::Stddev);
}

void init_algorithms_distributions(py::module& m) {
  declareLaplaceDistribution(m);
  declareGaussianDistribution(m);
}
