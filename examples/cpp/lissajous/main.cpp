/* ============================================================================
 *  tube-lib - Arithmetic on tubes: Lissajous example
 *
 *  Example from the paper "Guaranteed Computation of Robots Trajectories"
 *  Simon Rohou, Luc Jaulin, Lyudmila Mihaylova, Fabrice Le Bars, Sandor M. Veres
 * ============================================================================
 *  Copyright : Copyright 2016 Simon Rohou
 *  License   : This program can be distributed under the terms of
 *              the Apache License, Version 2.0. See the file LICENSE.
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2016
 * ---------------------------------------------------------------------------- */

#include "Tube.h"
#include "VibesFigure.h"
#include "VibesFigure_Tube.h"

using namespace std;
using namespace ibex;

void displayLissajousMap(const Tube& x, const Tube& y, int fig_x, int fig_y);

int main(int argc, char *argv[])
{
  /* =========== PARAMETERS =========== */

    Interval domain(0,6);
    double timestep = 0.001;

  /* =========== INITIALIZATION =========== */

    // Creating tubes over the [0,6] domain with some timestep:
    Tube xddot(domain, timestep, Function("t", "-10*cos(t)+[-0.001,0.001]"));
    Tube xdot(domain,timestep), x(domain, timestep), yddot(domain, timestep), ydot(domain, timestep), y(domain, timestep);

    // Initial conditions:
    Interval xdot0 = 0., ydot0 = 10.;
    Interval x0 = Interval(10.).inflate(0.2), y0 = Interval(0.).inflate(0.2);

  /* =========== CONSTRAINT NETWORK =========== */

    bool fixpoint;
    do
    {
      double vol_x = x.volume(), vol_y = y.volume();

      // Contractors
      xdot.ctcFwdBwd(xddot, xdot0);
      ydot.ctcFwdBwd(yddot, ydot0);
      x.ctcFwdBwd(xdot, x0);
      y.ctcFwdBwd(ydot, y0);

      // Further constraints that can be considered
      if(false)
      {
        y.ctcIn(ydot, y[M_PI/2.], 3.*M_PI/2.);
        y.ctcIn(ydot, y[3.*M_PI/2.], M_PI/2.);
        x.ctcIn(xdot, x[M_PI/2.], 3.*M_PI/2.);
        x.ctcIn(xdot, x[3.*M_PI/2.], M_PI/2.);
        y.ctcPeriodic(M_PI);
      }

      yddot &= -0.4 * xddot * xdot;

      cout << "contraction step..." << endl;
      // Fixpoint is detected when tubes are not contracted anymore
      fixpoint = vol_x == x.volume() && vol_y == y.volume();

    } while(!fixpoint);

  /* =========== GRAPHICS =========== */

    vibes::beginDrawing(); vibes::axisAuto();
    map<Tube*,VibesFigure_Tube*> map_graphics;
    displayTube(map_graphics, &x, "Tube [x](·)", 100, 100);
    displayTube(map_graphics, &y, "Tube [y](·)", 150, 150);
    displayLissajousMap(x, y, 1200, 200);

  /* =========== END =========== */

    // Deleting pointers to graphical tools
    for(auto it = map_graphics.begin(); it != map_graphics.end(); ++it)
      delete it->second;
    vibes::endDrawing();

  // Checking if this example is still working:
  return (fabs(x.volume() - 2.84844) < 1e-2
       && fabs(y.volume() - 5.3149) < 1e-2) ? EXIT_SUCCESS : EXIT_FAILURE;
}

void displayLissajousMap(const Tube& x, const Tube& y, int fig_x, int fig_y)
{
  const string fig_name = "Map (top view): [x](·)x[y](·)";
  const int slices_number_to_display = 500;

  vibes::newFigure(fig_name);
  vibes::setFigureProperties(
            vibesParams("figure", fig_name, "x", fig_x, "y", fig_y, "width", 900, "height", 600));
  vibes::axisLimits(-12, 12, -8, 8);

  // Robot's tubes projection
  int startpoint;
  for(int i = 0 ; i < x.size() ; i += max((int)(x.size() / slices_number_to_display), 1))
    startpoint = i;

  for(int i = startpoint ; i >= 0; i -= max((int)(x.size() / slices_number_to_display), 1))
  {
    Interval intv_x = x[i];
    Interval intv_y = y[i];
    if(!intv_x.is_unbounded() && !intv_y.is_unbounded())
      vibes::drawBox(intv_x.lb(), intv_x.ub(), intv_y.lb(), intv_y.ub(),
                     "lightGray[white]", vibesParams("figure", fig_name));
  }
}