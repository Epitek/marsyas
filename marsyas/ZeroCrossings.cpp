/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/** 
    \class ZeroCrossings
    \brief Time-domain ZeroCrossings 

    Basically counts the number of times the input 
signal crosses the zero line. 
*/

#include "ZeroCrossings.h"

using namespace std;
using namespace Marsyas;

ZeroCrossings::ZeroCrossings(string name):MarSystem("ZeroCrossings",name)
{
  //type_ = "ZeroCrossings";
  //name_ = name;
}


ZeroCrossings::~ZeroCrossings()
{
}


MarSystem* 
ZeroCrossings::clone() const 
{
  return new ZeroCrossings(*this);
}

void
ZeroCrossings::localUpdate()
{
  MRSDIAG("ZeroCrossings.cpp - ZeroCrossings:localUpdate");
  
  setctrl("mrs_natural/onSamples", (mrs_natural)1);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte").toReal() / getctrl("mrs_natural/inSamples").toNatural());

  setctrl("mrs_string/onObsNames", "ZeroCrossings,");  
}



 
void 
ZeroCrossings::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  zcrs_ = 1.0;
  
  for (o=0; o < inObservations_; o++)
    for (t = 1; t < inSamples_; t++)
      {
	if (((in(o, t-1) > 0) && (in(o,t) < 0)) ||
	    ((in(o, t-1) < 0) && (in(o,t) > 0)))
	  {
	    zcrs_++;
	  }
	out(o,0) = zcrs_ / inSamples_;
      }
}







	
	
