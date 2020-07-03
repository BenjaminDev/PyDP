// Provides bindings for algorithms count

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include <iostream>
#include "algorithms/count.h"

using namespace std;


namespace py = pybind11;
namespace dp = differential_privacy;

//this could/should be implemeted once and tested 
double CheckPrivacyBudget(double privacy_budget_fraction, double privacy_budget_) {
    if (privacy_budget_fraction < 0.0){
        std::cout << "Requested budget " << privacy_budget_fraction
        << " should be positive.";
        return false;
    }
    if (privacy_budget_fraction > privacy_budget_){

      std::cout  << "Requested budget " << privacy_budget_fraction
        << " exceeds remaining budget of " << privacy_budget_;
      return false;  
    }
    return true;
  }

template <typename T>
void declareCount(py::module& m, string const& suffix) {
  using count_builder = typename dp::Count<T>::Builder;

  py::class_<dp::Count<T>> count(m, ("Count" + suffix).c_str());
  count.attr("__module__") = "pydp";
  count.def(py::init([]() { 
    
    return count_builder().Build().ValueOrDie(); }))
      .def(py::init([](double epsilon) {
        return count_builder().SetEpsilon(epsilon).Build().ValueOrDie();
      }))
      .def("add_entry", &dp::Count<T>::AddEntry)
      .def("add_entries",
           [](dp::Count<T>& obj, std::vector<T>& v) {
             return obj.AddEntries(v.begin(), v.end());
           })
      // TODO: port ConfidenceInterval and Summary
      //.def("noise_confidence_interval", &dp::Count<T>::NoiseConfidenceInterval)
      //.def("serialize", &dp::Count<T>::Serialize)
      //.def("merge", &dp::Count<T>::Merge)
      .def("memory_used", &dp::Count<T>::MemoryUsed)
      .def("result",
           [](dp::Count<T>& obj, std::vector<T>& v) {
             return dp::GetValue<T>(obj.Result(v.begin(), v.end()).ValueOrDie());
           })
      .def("partial_result",
           [](dp::Count<T>& obj) {
            //  base::StatusOr<Output> resultf = sum_obj.ValueOrDie()->Result(a.begin(), a.end());
             return dp::GetValue<T>(obj.PartialResult().ValueOrDie());
           })

      .def("partial_result", [](dp::Count<T>& obj, double privacy_budget) {
          
          if (CheckPrivacyBudget(privacy_budget, obj.RemainingPrivacyBudget())){

              dp::base::StatusOr<dp::Output> resultf = obj.PartialResult(privacy_budget).ValueOrDie();
              if (resultf.ok()) {
                std::cout<<"updated"<<std::endl;
                return dp::GetValue<T>(resultf.ValueOrDie());
              } else {
                throw std::runtime_error(resultf.status().error_message());
              }
          }else{
            throw std::runtime_error("no more budget");
          }


        // return dp::GetValue<T>(obj.PartialResult(privacy_budget).ValueOrDie());
      });
}

void init_algorithms_count(py::module& m) {
  declareCount<int>(m, "Int");
  declareCount<double>(m, "Double");
}
