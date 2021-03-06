// GMTL is (C) Copyright 2001-2010 by Allen Bierbaum
// Distributed under the GNU Lesser General Public License 2.1 with an
// addendum covering inlined code. (See accompanying files LICENSE and
// LICENSE.addendum or http://www.gnu.org/copyleft/lesser.txt)

// This file was originally part of PyJuggler.

// PyJuggler is (C) Copyright 2002, 2003 by Patrick Hartling
// Distributed under the GNU Lesser General Public License 2.1.  (See
// accompanying file COPYING.txt or http://www.gnu.org/copyleft/lesser.txt)

// Includes ====================================================================
#include <boost/python.hpp>
#include <gmtl/Vec.h>
#include <gmtl-pickle.h>

// Using =======================================================================
using namespace boost::python;

// Module ======================================================================
void _Export_Vec_float_4()
{
    scope* gmtl_Vec_float_4_scope = new scope(
    class_< gmtl::Vec<float,4>, bases< gmtl::VecBase<float,4> > >("Vec4f", init<  >())
        .def(init< const gmtl::Vec<float,4> & >())
        .def(init< const gmtl::VecBase<float,4> & >())
        .def(init< const float &, const float &, const float &, const float & >())
        .def_pickle(gmtlPickle::Vec4_pickle<float>())
    );

    enum_< gmtl::Vec<float,4>::Params >("Params")
        .value("Size", gmtl::Vec<float,4>::Size)
    ;

    delete gmtl_Vec_float_4_scope;

}
