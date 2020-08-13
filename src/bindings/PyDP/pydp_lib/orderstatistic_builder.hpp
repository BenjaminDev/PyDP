#ifndef PYDP_LIB_ORDERSTAT_H_
#define PYDP_LIB_ORDERSTAT_H_
#include "algorithms/algorithm.h"
#include "algorithms/order-statistics.h"
#include "base/statusor.h"
#include "pybind11/stl.h"

namespace dp = differential_privacy;
namespace py = pybind11;

namespace differential_privacy {
namespace python {

template <typename T, class OrderStat>
class ContinuousBuilder {
 public:
  std::unique_ptr<OrderStat> build(double epsilon,
                                   std::optional<T> lower_bound = std::nullopt,
                                   std::optional<T> upper_bound = std::nullopt,
                                   std::optional<int> l0_sensitivity = std::nullopt,
                                   std::optional<int> linf_sensitivity = std::nullopt) {
    auto builder = typename OrderStat::Builder();

    builder.SetEpsilon(epsilon);
    // TODO: Validate: Is this construction logic valid in all cases?
    if (l0_sensitivity.has_value())
      builder.SetMaxPartitionsContributed(l0_sensitivity.value());
    if (linf_sensitivity.has_value())
      builder.SetMaxContributionsPerPartition(linf_sensitivity.value());

    if (lower_bound.has_value()) builder.SetLower(lower_bound.value());
    if (upper_bound.has_value()) builder.SetUpper(upper_bound.value());

    base::StatusOr<std::unique_ptr<OrderStat>> obj = builder.Build();
    if (!obj.ok()) {
      throw std::runtime_error(obj.status().error_message());
    }

    return std::move(obj.ValueOrDie());
  }

  std::map<std::type_index, std::string> type_to_name = {{typeid(double), "Double"},
                                                         {typeid(int64_t), "Int"}};
  std::map<std::type_index, std::string> algorithm_to_name = {
      {typeid(dp::continuous::Min<T>), "Min"},
      {typeid(dp::continuous::Max<T>), "Max"},
      {typeid(dp::continuous::Median<T>), "Median"},
      {typeid(dp::continuous::Percentile<T>), "Percentile"}};

  std::string get_algorithm_name() {
    // Set the suffix string
    return (algorithm_to_name[typeid(OrderStat)] + type_to_name[typeid(T)]);
  }

  void declare(py::module& m) {
    py::class_<OrderStat> pyself(m, get_algorithm_name().c_str());

    pyself.attr("__module__") = "pydp";

    pyself.def(py::init([this](double epsilon, T lower_bound, T upper_bound,
                               int l0_sensitivity, int linf_sensitivity) {
                 return this->build(epsilon, lower_bound, upper_bound, l0_sensitivity,
                                    linf_sensitivity);
               }),
               py::return_value_policy::reference, py::arg("epsilon"),
               py::arg("lower_bound"), py::arg("upper_bound"),
               py::arg("l0_sensitivity") = 1, py::arg("linf_sensitivity") = 1);

    pyself.def(
        py::init([this](double epsilon, int l0_sensitivity, int linf_sensitivity) {
          return this->build(epsilon, std::nullopt /*lower_bound*/,
                             std::nullopt /*upper_bound*/, l0_sensitivity,
                             linf_sensitivity);
        }),
        py::return_value_policy::reference, py::arg("epsilon"),
        py::arg("l0_sensitivity") = 1, py::arg("linf_sensitivity") = 1);
    // TODO: Should we base class this and there seems to be a lot of overlap.
    // I want to verbosely implement another use case as I get a deeper understanding I
    // think we can abstract better. Getters
    pyself.def_property_readonly("epsilon", &OrderStat::GetEpsilon);

    pyself.def("privacy_budget_left", &OrderStat::RemainingPrivacyBudget);

    pyself.def("memory_used", &OrderStat::MemoryUsed);

    // // Input data
    pyself.def("add_entries", [](OrderStat& pythis, std::vector<T>& v) {
      pythis.AddEntries(v.begin(), v.end());
    });

    pyself.def("add_entry", &OrderStat::AddEntry);

    // Compute results
    pyself.def("result", [](OrderStat& pythis, std::vector<T>& v) {
      auto result = pythis.Result(v.begin(), v.end());
      if (!result.ok()) {
        throw std::runtime_error(result.status().error_message());
      }

      return dp::GetValue<T>(result.ValueOrDie());
    });

    pyself.def("partial_result", [](OrderStat& pythis) {
      if (pythis.RemainingPrivacyBudget() <= 0.0) {
        throw std::runtime_error("Privacy budget requested exceeds set privacy budget");
      }
      auto result = pythis.PartialResult();

      if (!result.ok()) {
        throw std::runtime_error(result.status().error_message());
      }

      return dp::GetValue<T>(result.ValueOrDie());
    });

    pyself.def("partial_result", [](OrderStat& pythis, double privacy_budget) {
      if (privacy_budget > pythis.RemainingPrivacyBudget()) {
        throw std::runtime_error("Privacy budget requested exceeds set privacy budget");
      }

      auto result = pythis.PartialResult(privacy_budget);

      if (!result.ok()) {
        throw std::runtime_error(result.status().error_message());
      }

      return dp::GetValue<T>(result.ValueOrDie());
    });

    pyself.def("reset", &OrderStat::Reset);

    pyself.def("serialize", &OrderStat::Serialize);

    pyself.def("merge", &OrderStat::Merge);

    pyself.def("noise_confidence_interval", &OrderStat::NoiseConfidenceInterval);
  }
};

}  // namespace python
}  // namespace differential_privacy

#endif  // PYDP_LIB_ORDERSTAT_H_
