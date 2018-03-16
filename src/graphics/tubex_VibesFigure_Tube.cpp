/* ============================================================================
 *  tubex-lib - VibesFigure_Tube class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2015
 * ---------------------------------------------------------------------------- */

#include "ibex.h"
#include "vibes.h"
#include "tubex_VibesFigure_Tube.h"

// a real value to display unbounded slices
#include <limits>
#define BOUNDED_INFINITY numeric_limits<float>::max()

using namespace std;
using namespace ibex;

namespace tubex
{
  // Static items

    vector<VibesFigure_Tube*> VibesFigure_Tube::v_vibesfig_tube;

    void VibesFigure_Tube::draw(const string& fig_name, int x, int y)
    {
      draw(fig_name, NULL, NULL, x, y);
    }

    void VibesFigure_Tube::draw(const string& fig_name, const Tube *tube, int x, int y)
    {
      draw(fig_name, tube, NULL, x, y);
    }

    void VibesFigure_Tube::draw(const string& fig_name, const Trajectory *traj, int x, int y)
    {
      draw(fig_name, NULL, traj, x, y);
    }
    
    void VibesFigure_Tube::draw(const string& fig_name, const Tube *tube, const Trajectory *traj, int x, int y)
    {
      if(VibesFigure_Tube::v_vibesfig_tube.size() == 0)
      {
        vibes::beginDrawing();
        vibes::axisAuto();
      }

      VibesFigure_Tube *figtube = NULL;
      for(int i = 0 ; i < v_vibesfig_tube.size() ; i++)
      {
        if(v_vibesfig_tube[i]->name() == fig_name)
        {
          figtube = v_vibesfig_tube[i];
          break;
        }
      }

      if(figtube == NULL)
      {
        figtube = new VibesFigure_Tube(fig_name);
        v_vibesfig_tube.push_back(figtube);
      }

      figtube->setProperties(x, y, 700, 350);
      if(tube != NULL) figtube->addTube(tube);
      if(traj != NULL) figtube->addTrajectory(traj);
      figtube->show();
    }

    void VibesFigure_Tube::endDrawing()
    {
      for(int i = 0 ; i < v_vibesfig_tube.size() ; i++)
        delete v_vibesfig_tube[i];
      vibes::endDrawing();
    }

  // Non-static items

    VibesFigure_Tube::VibesFigure_Tube(const string& fig_name, const Tube *tube, const Trajectory *traj) : VibesFigure(fig_name)
    {
      if(tube != NULL) addTube(tube);
      if(traj != NULL) addTrajectory(traj);
      m_view_box = IntervalVector(2, Interval::EMPTY_SET);
    }

    VibesFigure_Tube::~VibesFigure_Tube()
    {
      typename map<const Tube*,FigTubeParams>::const_iterator it;
      for(it = m_map_tubes.begin(); it != m_map_tubes.end(); it++)
        if(it->second.tube_copy != NULL)
          delete it->second.tube_copy;
    }

    void VibesFigure_Tube::addTube(const Tube *tube, const string& color_frgrnd, const string& color_bckgrnd)
    {
      if(m_map_tubes.find(tube) != m_map_tubes.end())
        cout << "Warning VibesFigure_Tube::addTube(): tube already added" << endl;

      else
        m_map_tubes[tube];

      setTubeColor(tube, color_frgrnd, color_bckgrnd);
      setTubeColor(tube, TubeColorType::SLICES, DEFAULT_SLICES_COLOR);
    }

    void VibesFigure_Tube::setTubeColor(const Tube *tube, const string& color_frgrnd, const string& color_bckgrnd)
    {
      if(m_map_tubes.find(tube) == m_map_tubes.end())
        cout << "Warning VibesFigure_Tube::setTubeColor(): unknown tube" << endl;

      setTubeColor(tube, TubeColorType::FOREGROUND, color_frgrnd);
      setTubeColor(tube, TubeColorType::BACKGROUND, color_bckgrnd);
    }

    void VibesFigure_Tube::setTubeColor(const Tube *tube, int color_type, const string& color)
    {
      if(m_map_tubes.find(tube) == m_map_tubes.end())
        cout << "Warning VibesFigure_Tube::setTubeColor(): unknown tube" << endl;

      m_map_tubes[tube].m_colors[color_type] = color;
    }
    
    void VibesFigure_Tube::removeTube(const Tube *tube)
    {
      if(m_map_tubes.find(tube) == m_map_tubes.end())
        cout << "Warning VibesFigure_Tube::removeTube(): unable to remove" << endl;

      if(m_map_tubes[tube].tube_copy != NULL)
        delete m_map_tubes[tube].tube_copy;
      m_map_tubes.erase(tube);
    }

    void VibesFigure_Tube::addTrajectory(const Trajectory *traj, const string& color)
    {
      if(m_map_trajs.find(traj) != m_map_trajs.end())
        cout << "Warning VibesFigure_Tube::addTrajectory(): trajectory already added" << endl;

      else
        m_map_trajs[traj].color = color;
    }
    
    void VibesFigure_Tube::setTrajectoryColor(const Trajectory *traj, const string& color)
    {
      if(m_map_trajs.find(traj) == m_map_trajs.end())
        cout << "Warning VibesFigure_Tube::setTrajectoryColor(): unknown trajectory" << endl;

      m_map_trajs[traj].color = color;
    }
    
    void VibesFigure_Tube::removeTrajectory(const Trajectory *traj)
    {
      if(m_map_trajs.find(traj) == m_map_trajs.end())
        cout << "Warning VibesFigure_Tube::removeTrajectory(): unable to remove" << endl;

      m_map_trajs.erase(traj);
    }
    
    void VibesFigure_Tube::show()
    {
      show(false);
    }
    
    void VibesFigure_Tube::show(bool detail_slices)
    {
      typename map<const Tube*,FigTubeParams>::const_iterator it_tubes;
      for(it_tubes = m_map_tubes.begin(); it_tubes != m_map_tubes.end(); it_tubes++)
        m_view_box |= drawTube(it_tubes->first);

      // Trajectories are drawn on top of the tubes
      typename map<const Trajectory*,FigTrajParams>::const_iterator it_trajs;
      for(it_trajs = m_map_trajs.begin(); it_trajs != m_map_trajs.end(); it_trajs++)
        m_view_box |= drawTrajectory(it_trajs->first);

      axisLimits(m_view_box);

      // The following is used as a calibration of the SVG file
      vibes::clearGroup(m_name, "transparent_box");
      vibes::drawBox(m_view_box, vibesParams("figure", m_name, "group", "transparent_box"));
    }

    const IntervalVector VibesFigure_Tube::drawTube(const Tube *tube, bool detail_slices)
    {
      IntervalVector viewbox(2, Interval::EMPTY_SET);

      // Computing viewbox

        double image_lb, image_ub;

        if(!tube->image().is_unbounded())
        {
          image_lb = tube->image().lb();
          image_ub = tube->image().ub();
        }

        else // some slices can be [-oo,+oo], maybe not all of them
        {
          image_lb = NAN;
          image_ub = NAN;

          for(int i = 0 ; i < tube->nbSlices() ; i++)
          {
            Interval slice = (*tube)[i];
            if(!slice.is_unbounded())
            {
              image_lb = std::isnan(image_lb) || image_lb > slice.lb() ? slice.lb() : image_lb;
              image_ub = std::isnan(image_ub) || image_ub < slice.ub() ? slice.ub() : image_ub;
            }
          }
        }

        viewbox[0] = tube->domain();
        viewbox[1] = Interval(image_lb, image_ub);

      // Displaying tube

        ostringstream o;
        o << "tube_" << tube->name();
        string group_name = o.str();
        string group_name_bckgrnd = o.str() + "_old";

        // Two display modes available:
        // - one in which each slice is shown
        // - one in which only the polygon envelope of the tube is shown

        // First, displaying background.
        // The background is the previous version of the tube (before contraction).
        // Always displayed as a polygon.
        {
          if(m_map_tubes[tube].tube_copy == NULL)
          {
            // If a copy of the tube has not been done,
            // we make one and no display is done.
          }

          else
          {
            // Otherwise, the copy is displayed and then updated
            // with the current version of the tube.

            vector<double> v_x, v_y;
            computePolygonEnvelope(*m_map_tubes[tube].tube_copy, v_x, v_y);
            vibes::clearGroup(m_name, group_name_bckgrnd);
            vibes::newGroup(group_name_bckgrnd, m_map_tubes[tube].m_colors[TubeColorType::BACKGROUND], vibesParams("figure", m_name));
            vibes::Params params_background = vibesParams("figure", m_name, "group", group_name_bckgrnd);
            vibes::drawPolygon(v_x, v_y, params_background);

            delete m_map_tubes[tube].tube_copy;
          }

          m_map_tubes[tube].tube_copy = new Tube(*tube);
        }

        // Second, the foreground: actual values of the tube.
        // Can be either displayed slice by slice or with a polygon envelope.

        vibes::Params params_foreground = vibesParams("figure", m_name, "group", group_name);
        vibes::clearGroup(m_name, group_name);

        if(detail_slices)
        {
          vibes::newGroup(group_name, m_map_tubes[tube].m_colors[TubeColorType::SLICES], vibesParams("figure", m_name));
          for(int i = 0 ; i < tube->nbSlices() ; i++)
            drawSlice(tube->sliceBox(i), params_foreground);
        }

        else
        {
          vector<double> v_x, v_y;
          computePolygonEnvelope(*tube, v_x, v_y);
          vibes::newGroup(group_name, m_map_tubes[tube].m_colors[TubeColorType::FOREGROUND], vibesParams("figure", m_name));
          vibes::drawPolygon(v_x, v_y, params_foreground);
        }

      return viewbox;
    }

    void VibesFigure_Tube::computePolygonEnvelope(const Tube& tube, vector<double>& v_x, vector<double>& v_y)
    {
      if(tube.isEmpty()) cout << "Tube graphics: warning, tube " << tube.name() << " is empty" << endl;

      for(int i = 0 ; i < tube.nbSlices() ; i++)
      {
        IntervalVector slice_box = tube.sliceBox(i);
        slice_box[1] &= Interval(-BOUNDED_INFINITY,BOUNDED_INFINITY);
        v_x.push_back(slice_box[0].lb()); v_x.push_back(slice_box[0].ub());
        v_y.push_back(slice_box[1].ub()); v_y.push_back(slice_box[1].ub());
      }

      for(int i = tube.nbSlices() - 1 ; i >= 0 ; i--)
      {
        IntervalVector slice_box = tube.sliceBox(i);
        slice_box[1] &= Interval(-BOUNDED_INFINITY,BOUNDED_INFINITY);
        v_x.push_back(slice_box[0].ub()); v_x.push_back(slice_box[0].lb());
        v_y.push_back(slice_box[1].lb()); v_y.push_back(slice_box[1].lb());
      }
    }

    void VibesFigure_Tube::drawSlice(const IntervalVector& slice, const vibes::Params& params) const
    {
      if(slice[1].is_empty())
        return; // no display

      IntervalVector boundedSlice(slice);
      boundedSlice[1] &= Interval(-BOUNDED_INFINITY,BOUNDED_INFINITY);
      vibes::drawBox(boundedSlice, params);
    }
    
    const IntervalVector VibesFigure_Tube::drawTrajectory(const Trajectory *traj, float points_size)
    {
      IntervalVector viewbox(2, Interval::EMPTY_SET);

      std::ostringstream o;
      o << "traj_" << traj->name();
      vibes::newGroup(o.str(), m_map_trajs[traj].color, vibesParams("figure", m_name));

      if(traj->domain().is_unbounded() || traj->domain().is_empty())
        return viewbox;

      // Two display modes available:
      // - one by Function
      // - one by map<double,double>

      vector<double> v_x, v_y;

      if(traj->getFunction() == NULL)
      {
        typename map<double,double>::const_iterator it_scalar_values;
        for(it_scalar_values = traj->getMap().begin(); it_scalar_values != traj->getMap().end(); it_scalar_values++)
        {
          if(points_size != 0.)
            vibes::drawPoint(it_scalar_values->first, it_scalar_values->second,
                             points_size, vibesParams("figure", m_name, "group", o.str()));

          else
          {
            v_x.push_back(it_scalar_values->first);
            v_y.push_back(it_scalar_values->second);
          }

          viewbox[0] |= it_scalar_values->first;
          viewbox[1] |= it_scalar_values->second;
        }
      }

      else
      {
        for(double t = traj->domain().lb() ; t <= traj->domain().ub() ; t+=traj->domain().diam()/TRAJ_NB_DISPLAYED_POINTS)
        {
          if(points_size != 0.)
            vibes::drawPoint(t, (*traj)[t], points_size, vibesParams("figure", m_name, "group", o.str()));

          else
          {
            v_x.push_back(t);
            v_y.push_back((*traj)[t]);
          }

          viewbox[0] |= t;
          viewbox[1] |= (*traj)[t];
        }
      }

      if(v_x.size() != 0)
        vibes::drawLine(v_x, v_y, vibesParams("figure", m_name, "group", o.str()));

      return viewbox;
    }
}