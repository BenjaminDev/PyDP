#ifndef PYDP_LIB_ALGORITHM_H_
#define PYDP_LIB_ALGORITHM_H_

#include "algorithms/algorithm.h"
#include "algorithms/order-statistics.h"
#include "base/statusor.h"

namespace dp = differential_privacy;
namespace py = pybind11;

namespace differential_privacy {
namespace python {

template <typename T, class OrderStat>
class ContinuousBuilder {
 public:
  std::unique_ptr<OrderStat> build(double epsilon,
                                   std::optional<int> l0_sensitivity = std::nullopt,
                                   std::optional<int> linf_sensitivity = std::nullopt) {
    auto builder = typename OrderStat::Builder();
      
    builder.SetEpsilon(epsilon);

    if (l0_sensitivity.has_value())
      builder.SetMaxPartitionsContributed(l0_sensitivity.value());
    if (linf_sensitivity.has_value())
      builder.SetMaxContributionsPerPartition(linf_sensitivity.value());

    base::StatusOr<std::unique_ptr<OrderStat>> obj = builder.Build();
    if (!obj.ok()) {
      throw std::runtime_error(obj.status().error_message());
    }

    return obj.ValueOrDie();
  }

  std::map<std::type_index, std::string> type_to_name = {{typeid(double), "Double"},
                                                         {typeid(int), "Int"}};
  std::map<std::type_index, std::string> algorithm_to_name = {
      {typeid(dp::continuous::Min<T>), "Min"},
      {typeid(dp::continuous::Max<T>), "Max"}};

  std::string get_algorithm_name() {
    // Set the suffix string
    return (algorithm_to_name[typeid(OrderStat)] + type_to_name[typeid(T)]);
  }

  void declare(py::module& m) {
    py::class_<OrderStat> pyself(m, get_algorithm_name().c_str());

    pyself.attr("__module__") = "pydp";


    // No bounds constructor
    // continuous::Min<int64_t>::Builder()
            //   .SetEpsilon(config->epsilon)
            //   .SetMaxPartitionsContributed(l0_sensitivity)
            //   .SetMaxContributionsPerPartition(linf_sensitivity)
            //   .Build();
    
    pyself.def(py::init([this](double epsilon, int l0_sensitivity,
                               int linf_sensitivity) {
                 return this->build(epsilon,  l0_sensitivity,
                                    linf_sensitivity);
               }),
               py::arg("epsilon"), py::arg("l0_sensitivity") = 1,
               py::arg("linf_sensitivity") = 1);

    // Getters
    pyself.def_property_readonly("epsilon", &OrderStat::GetEpsilon);

    // pyself.def("privacy_budget_left", &Algorithm::RemainingPrivacyBudget);

    // pyself.def("consume_privacy_budget", &Algorithm::ConsumePrivacyBudget);

    // pyself.def("memory_used", &Algorithm::MemoryUsed);

    // // Input data
    // pyself.def("add_entries", [](Algorithm& pythis, std::vector<T>& v) {
    //   pythis.AddEntries(v.begin(), v.end());
    // });

    // pyself.def("add_entry", &Algorithm::AddEntry);

    // Compute results
    pyself.def("result", [](OrderStat& pythis, std::vector<T>& v) {
      auto result = pythis.Result(v.begin(), v.end());
      // py::print(typeid(pythis));
      if (!result.ok()) {
        throw std::runtime_error(result.status().error_message());
      }

      return dp::GetValue<T>(result.ValueOrDie());
    });

    // pyself.def("partial_result", [](Algorithm& pythis) {
    //   auto result = pythis.PartialResult();

    //   if (!result.ok()) {
    //     throw std::runtime_error(result.status().error_message());
    //   }

    //   return dp::GetValue<double>(result.ValueOrDie());
    // });

    // pyself.def("partial_result", [](Algorithm& pythis, double privacy_budget) {
    //   if (privacy_budget > pythis.RemainingPrivacyBudget()) {
    //     throw std::runtime_error("Privacy budget requeted exceeds set privacy budget");
    //   }

    //   auto result = pythis.PartialResult(privacy_budget);

    //   if (!result.ok()) {
    //     throw std::runtime_error(result.status().error_message());
    //   }

    //   return dp::GetValue<double>(result.ValueOrDie());
    // });

    // pyself.def("partial_result", [](Algorithm& pythis, double privacy_budget,
    //                                 double noise_interval_level) {
    //   if (privacy_budget > pythis.RemainingPrivacyBudget()) {
    //     throw std::runtime_error("Privacy budget requeted exceeds set privacy budget");
    //   }

    //   auto result = pythis.PartialResult(privacy_budget, noise_interval_level);

    //   if (!result.ok()) {
    //     throw std::runtime_error(result.status().error_message());
    //   }

    //   return dp::GetValue<double>(result.ValueOrDie());
    // });

    // Other methods
    // pyself.def("consume_privacy_budget", &Algorithm::ConsumePrivacyBudget);

    // pyself.def("reset", &Algorithm::Reset);

    // pyself.def("serialize", &Algorithm::Serialize);

    // pyself.def("merge", &Algorithm::Merge);

    // pyself.def("noise_confidence_interval", &Algorithm::NoiseConfidenceInterval);
  }
};

}  // namespace python
}  // namespace differential_privacy

#endif  // PYDP_LIB_ALGORITHM_H_
