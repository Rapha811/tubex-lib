/** 
 *  \file
 *  Tube binding
 * ----------------------------------------------------------------------------
 *  \date       2020
 *  \author     Simon Rohou, Benoît Desrochers
 *  \copyright  Copyright 2020 Simon Rohou
 *  \license    This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 */

#include "tubex_Fnc.h"
#include "tubex_Function.h"
#include "tubex_Tube.h"
#include "tubex_TubeVector.h"
#include "pyibex_export_Function_docs.h"
#include "pyibex_export_Fnc_docs.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>

namespace py = pybind11;
using namespace pybind11::literals;
using py::class_;
using py::init;

using namespace tubex;
using ibex::Interval;
using ibex::IntervalVector;


// class Tube;
// class TubeVector;




class pyFnc : public Fnc {
  // protected:
public:
  // pyFnc(int v): Fnc(v){}
  /* Inherit the constructors */
  using Fnc::Fnc;
  const Tube eval(const TubeVector &x) const override {
    PYBIND11_OVERLOAD_PURE(const Tube, Fnc, eval, x);
  }
  const ibex::Interval eval(const ibex::IntervalVector &x) const override {
    PYBIND11_OVERLOAD_PURE(const ibex::Interval, Fnc, eval, x);
  }
  const ibex::Interval eval(int slice_id,const TubeVector &x) const override {
    PYBIND11_OVERLOAD_PURE(const ibex::Interval, Fnc, eval, slice_id, x);
  }
  const ibex::Interval eval(const ibex::Interval &t,const TubeVector &x) const override {
    PYBIND11_OVERLOAD_PURE(const ibex::Interval, Fnc, eval, t, x);
  }
  const TubeVector eval_vector(const TubeVector &x) const override {
    PYBIND11_OVERLOAD_PURE(const TubeVector, Fnc, eval_vector, x);
  }
  const ibex::IntervalVector eval_vector(const ibex::IntervalVector &x) const override {
    PYBIND11_OVERLOAD_PURE(const ibex::IntervalVector, Fnc, eval_vector, x);
  }
  const ibex::IntervalVector eval_vector(int slice_id,const TubeVector &x) const override {
    PYBIND11_OVERLOAD_PURE(const ibex::IntervalVector, Fnc, eval_vector, slice_id, x);
  }
  const ibex::IntervalVector eval_vector(const ibex::Interval &t,const TubeVector &x) const override {
    PYBIND11_OVERLOAD_PURE(const ibex::IntervalVector, Fnc, eval_vector, t, x);
  }
  /* Trampoline (need one for each virtual function) */
  // void contract(IntervalVector& box) override {
  //   // py::gil_scoped_acquire acquire;
  //   PYBIND11_OVERLOAD_PURE(
  //     void,       /* return type */
  //     Fnc,        /* Parent class */
  //     contract,   /* Name of function */
  //     box         /* Argument(s) */
  //   );
    // py::gil_scoped_release release;
  // }
};

typedef const char* cc_ptr;
void export_Function(py::module& m){

  py::class_<Fnc, pyFnc> fnc(m, "Fnc");
  fnc
    .def(py::init<int,int,bool>(),
        DOCS_FNC_FNC_INT_INT_BOOL, "n"_a, "m"_a, "is_intertemporal"_a)
    .def("nb_vars", &Fnc::nb_vars,DOCS_FNC_NB_VARS)
    .def("image_dim", &Fnc::image_dim,DOCS_FNC_IMAGE_DIM)
    .def("is_intertemporal", &Fnc::is_intertemporal,DOCS_FNC_IS_INTERTEMPORAL)
    // .def("eval", (const Tube (Fnc::*)(const TubeVector &) const)&Fnc::eval,
    //     DOCS_FNC_EVAL_TUBEVECTOR, "x"_a)
    // .def("eval", (const ibex::Interval (Fnc::*)(const ibex::IntervalVector &) const)&Fnc::eval,
    //     DOCS_FNC_EVAL_INTERVALVECTOR, "x"_a)
    // .def("eval", (const ibex::Interval (Fnc::*)(int,const TubeVector &) const)&Fnc::eval,
    //     DOCS_FNC_EVAL_INT_TUBEVECTOR, "slice_id"_a, "x"_a)
    // .def("eval", (const ibex::Interval (Fnc::*)(const ibex::Interval &,const TubeVector &) const)&Fnc::eval,
    //     DOCS_FNC_EVAL_INTERVAL_TUBEVECTOR, "t"_a, "x"_a)
    // .def("eval_vector", (const TubeVector (Fnc::*)(const TubeVector &) const)&Fnc::eval_vector,
    //     DOCS_FNC_EVAL_VECTOR_TUBEVECTOR, "x"_a)
    // .def("eval_vector", (const ibex::IntervalVector (Fnc::*)(const ibex::IntervalVector &) const)&Fnc::eval_vector,
    //     DOCS_FNC_EVAL_VECTOR_INTERVALVECTOR, "x"_a)
    // .def("eval_vector", (const ibex::IntervalVector (Fnc::*)(int,const TubeVector &) const)&Fnc::eval_vector,
    //     DOCS_FNC_EVAL_VECTOR_INT_TUBEVECTOR, "slice_id"_a, "x"_a)
    // .def("eval_vector", (const ibex::IntervalVector (Fnc::*)(const ibex::Interval &,const TubeVector &) const)&Fnc::eval_vector,
    //     DOCS_FNC_EVAL_VECTOR_INTERVAL_TUBEVECTOR, "t"_a, "x"_a)
  ;

  py::class_<Function, Fnc> function(m, "Function");
  function
      .def(py::init<cc_ptr>(),DOCS_FUNCTION_FUNCTION_CHAR, "y"_a)
      .def(py::init<cc_ptr,cc_ptr>(),
          DOCS_FUNCTION_FUNCTION_CHAR_CHAR, "x1"_a, "y"_a)
      .def(py::init<cc_ptr,cc_ptr,cc_ptr>(),
          DOCS_FUNCTION_FUNCTION_CHAR_CHAR_CHAR, "x1"_a, "x2"_a, "y"_a)
      .def(py::init<cc_ptr,cc_ptr,cc_ptr,cc_ptr>(),
          DOCS_FUNCTION_FUNCTION_CHAR_CHAR_CHAR_CHAR, "x1"_a, "x2"_a, "x3"_a, "y"_a)
      .def(py::init<cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr>(),
          DOCS_FUNCTION_FUNCTION_CHAR_CHAR_CHAR_CHAR_CHAR, "x1"_a, "x2"_a, "x3"_a, "x4"_a, "y"_a)
      .def(py::init<cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr>(),
          DOCS_FUNCTION_FUNCTION_CHAR_CHAR_CHAR_CHAR_CHAR_CHAR, "x1"_a, "x2"_a, "x3"_a, "x4"_a, "x5"_a, "y"_a)
      .def(py::init<cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr>(),
          DOCS_FUNCTION_FUNCTION_CHAR_CHAR_CHAR_CHAR_CHAR_CHAR_CHAR, "x1"_a, "x2"_a, "x3"_a, "x4"_a, "x5"_a, "x6"_a, "y"_a)
      .def(py::init<cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr>(),
          DOCS_FUNCTION_FUNCTION_CHAR_CHAR_CHAR_CHAR_CHAR_CHAR_CHAR_CHAR, "x1"_a, "x2"_a, "x3"_a, "x4"_a, "x5"_a, "x6"_a, "x7"_a, "y"_a)
      .def(py::init<cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr,cc_ptr>(),
          DOCS_FUNCTION_FUNCTION_CHAR_CHAR_CHAR_CHAR_CHAR_CHAR_CHAR_CHAR_CHAR, "x1"_a, "x2"_a, "x3"_a, "x4"_a, "x5"_a, "x6"_a, "x7"_a, "x8"_a, "y"_a)
      // .def(py::init<int,cc_ptr *,cc_ptr>(),
          // DOCS_FUNCTION_FUNCTION_INT_CHAR_CHAR, "n"_a, "x"_a, "y"_a)
      .def(py::init<const tubex::Function &>(),
          DOCS_FUNCTION_FUNCTION_FUNCTION, "f"_a)

      .def("ibex_function", &Function::ibex_function,DOCS_FUNCTION_IBEX_FUNCTION)
      .def("eval", (const Tube (Function::*)(const TubeVector &) const)&Function::eval,
          DOCS_FUNCTION_EVAL_TUBEVECTOR, "x"_a)
      .def("eval", (const ibex::Interval (Function::*)(const ibex::Interval &) const)&Function::eval,
          DOCS_FUNCTION_EVAL_INTERVAL, "t"_a)
      .def("eval", (const ibex::Interval (Function::*)(const ibex::IntervalVector &) const)&Function::eval,
          DOCS_FUNCTION_EVAL_INTERVALVECTOR, "x"_a)
      .def("eval", (const ibex::Interval (Function::*)(int,const TubeVector &) const)&Function::eval,
          DOCS_FUNCTION_EVAL_INT_TUBEVECTOR, "slice_id"_a, "x"_a)
      .def("eval", (const ibex::Interval (Function::*)(const ibex::Interval &,const TubeVector &) const)&Function::eval,
          DOCS_FUNCTION_EVAL_INTERVAL_TUBEVECTOR, "t"_a, "x"_a)
      .def("traj_eval", &Function::traj_eval,
          DOCS_FUNCTION_TRAJ_EVAL_TRAJECTORYVECTOR, "x"_a)
      .def("eval_vector", (const TubeVector (Function::*)(const TubeVector &) const)&Function::eval_vector,
          DOCS_FUNCTION_EVAL_VECTOR_TUBEVECTOR, "x"_a)
      .def("eval_vector", (const ibex::IntervalVector (Function::*)(const ibex::Interval &) const)&Function::eval_vector,
          DOCS_FUNCTION_EVAL_VECTOR_INTERVAL, "t"_a)
      .def("eval_vector", (const ibex::IntervalVector (Function::*)(const ibex::IntervalVector &) const)&Function::eval_vector,
          DOCS_FUNCTION_EVAL_VECTOR_INTERVALVECTOR, "x"_a)
      .def("eval_vector", (const ibex::IntervalVector (Function::*)(int,const TubeVector &) const)&Function::eval_vector,
          DOCS_FUNCTION_EVAL_VECTOR_INT_TUBEVECTOR, "slice_id"_a, "x"_a)
      .def("eval_vector", (const ibex::IntervalVector (Function::*)(const ibex::Interval &,const TubeVector &) const)&Function::eval_vector,
          DOCS_FUNCTION_EVAL_VECTOR_INTERVAL_TUBEVECTOR, "t"_a, "x"_a)
      .def("traj_eval_vector", &Function::traj_eval_vector,
          DOCS_FUNCTION_TRAJ_EVAL_VECTOR_TRAJECTORYVECTOR, "x"_a)
      .def("diff", &Function::diff,DOCS_FUNCTION_DIFF)

      .def("__getitem__", [](Function& s, size_t index){
              if (index >= s.nb_vars()){
                  throw py::index_error();
              }
                return s[static_cast<int>(index)];
          }, DOCS_FUNCTION_OPERATOR_INDEX_INT, py::return_value_policy::reference_internal)
    ;
}
