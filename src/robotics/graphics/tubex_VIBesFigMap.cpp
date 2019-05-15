/** 
 *  VIBesFigMap class
 * ----------------------------------------------------------------------------
 *  \date       2015
 *  \author     Simon Rohou
 *  \copyright  Copyright 2019 Simon Rohou
 *  \license    This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 */

#include <string>
#include <iomanip>
#include "tubex_VIBesFigMap.h"
#include "tubex_colors.h"
#include "tubex_Tube.h"
#include "tubex_Trajectory.h"

using namespace std;
using namespace ibex;

#define GRAY_DISPLAY_MODE 0

namespace tubex
{
  VIBesFigMap::VIBesFigMap(const string& fig_name)
    : VIBesFig(fig_name)
  {
    
  }

  VIBesFigMap::~VIBesFigMap()
  {
    typename map<const TubeVector*,FigMapTubeParams>::const_iterator it;
    for(it = m_map_tubes.begin(); it != m_map_tubes.end(); it++)
      if(it->second.tube_x_copy != NULL)
      {
        delete it->second.tube_x_copy;
        delete it->second.tube_y_copy;
      }
  }
  
  void VIBesFigMap::set_restricted_tdomain(const Interval& restricted_tdomain)
  {
    m_restricted_tdomain = restricted_tdomain;
  }
  
  void VIBesFigMap::enable_tubes_backgrounds(bool enable)
  {
    m_draw_tubes_backgrounds = enable;
  }

  void VIBesFigMap::show()
  {
    typename map<const TubeVector*,FigMapTubeParams>::const_iterator it_tubes;
    for(it_tubes = m_map_tubes.begin(); it_tubes != m_map_tubes.end(); it_tubes++)
      m_view_box |= draw_tube(it_tubes->first);

    // Trajectories are drawn on top of the tubes
    typename map<const TrajectoryVector*,FigMapTrajParams>::const_iterator it_trajs;
    for(it_trajs = m_map_trajs.begin(); it_trajs != m_map_trajs.end(); it_trajs++)
      m_view_box |= draw_trajectory(it_trajs->first);

    axis_limits(m_view_box, true, 0.02);
  }

  void VIBesFigMap::show(float robot_size)
  {
    assert(robot_size >= 0.);
    m_robot_size = robot_size;
    show();
  }

  void VIBesFigMap::add_tube(const TubeVector *tube, const string& name, int index_x, int index_y)
  {
    assert(tube != NULL);
    assert(index_x != index_y);
    assert(index_x >= 0 && index_x < tube->size());
    assert(index_y >= 0 && index_y < tube->size());
    assert(m_map_tubes.find(tube) == m_map_tubes.end()
      && "tube must not have been previously added");

    m_map_tubes[tube];
    m_map_tubes[tube].index_x = index_x;
    m_map_tubes[tube].index_y = index_y;
    set_tube_color(tube, ColorMap::BLUE_TUBE);
    set_tube_name(tube, name);

    vibes::newGroup("tube_" + name + "_bckgrnd", "lightgray[lightgray]", vibesParams("figure", this->name()));
    vibes::newGroup("tube_" + name, "gray[gray]", vibesParams("figure", this->name()));
  }

  void VIBesFigMap::set_tube_name(const TubeVector *tube, const string& name)
  {
    assert(tube != NULL);
    assert(m_map_tubes.find(tube) != m_map_tubes.end()
      && "unknown tube, must be added beforehand");

    m_map_tubes[tube].name = name;
  }
  
  void VIBesFigMap::set_tube_color(const TubeVector *tube, const string& color)
  {
    assert(tube != NULL);
    assert(color != "");
    assert(m_map_tubes.find(tube) != m_map_tubes.end()
      && "unknown tube, must be added beforehand");

    m_map_tubes[tube].color = color;
  }
  
  void VIBesFigMap::set_tube_color(const TubeVector *tube, const ColorMap& colormap, const Trajectory *traj_colormap)
  {
    assert(tube != NULL);
    assert(m_map_tubes.find(tube) != m_map_tubes.end()
      && "unknown tube, must be added beforehand");

    m_map_tubes[tube].color = ""; // removing constant color
    m_map_tubes[tube].color_map = make_pair(colormap, traj_colormap);
  }
  
  void VIBesFigMap::remove_tube(const TubeVector *tube)
  {
    assert(tube != NULL);
    assert(m_map_tubes.find(tube) != m_map_tubes.end()
      && "unable to remove, unknown tube");

    if(m_map_tubes[tube].tube_x_copy != NULL)
    {
      delete m_map_tubes[tube].tube_x_copy;
      delete m_map_tubes[tube].tube_y_copy;
    }

    m_map_tubes.erase(tube);
  }

  void VIBesFigMap::add_trajectory(const TrajectoryVector *traj, const string& name, int index_x, int index_y, const string& color)
  {
    assert(traj != NULL);
    assert(m_map_trajs.find(traj) == m_map_trajs.end()
      && "traj must not have been previously added");
    assert(index_x != index_y);
    assert(index_x >= 0 && index_x < traj->size());
    assert(index_y >= 0 && index_y < traj->size());

    add_trajectory(traj, name, index_x, index_y, -1, color);
  }

  void VIBesFigMap::add_trajectory(const TrajectoryVector *traj, const string& name, int index_x, int index_y, int index_heading, const string& color)
  {
    assert(traj != NULL);
    assert(m_map_trajs.find(traj) == m_map_trajs.end()
      && "traj must not have been previously added");
    assert(index_x != index_y && index_x != index_heading && index_y != index_heading);
    assert(index_x >= 0 && index_x < traj->size());
    assert(index_y >= 0 && index_y < traj->size());
    assert(index_heading == -1 || (index_heading >= 0 && index_heading < traj->size()));

    m_map_trajs[traj];
    m_map_trajs[traj].index_x = index_x;
    m_map_trajs[traj].index_y = index_y;
    m_map_trajs[traj].index_heading = index_heading;

    set_trajectory_name(traj, name);
    set_trajectory_color(traj, color);
  }

  void VIBesFigMap::set_trajectory_name(const TrajectoryVector *traj, const string& name)
  {
    assert(traj != NULL);
    assert(m_map_trajs.find(traj) != m_map_trajs.end()
      && "unknown traj, must be added beforehand");

    m_map_trajs[traj].name = name;
  }
  
  void VIBesFigMap::set_trajectory_color(const TrajectoryVector *traj, const string& color)
  {
    assert(traj != NULL);
    assert(color != "");
    assert(m_map_trajs.find(traj) != m_map_trajs.end()
      && "unable to remove, unknown traj");

    m_map_trajs[traj].color = color;

    // Related groups are created during the display procedure
    // so that trajectories stay on top of the tubes.
  }
  
  void VIBesFigMap::set_trajectory_color(const TrajectoryVector *traj, const ColorMap& colormap, const Trajectory *traj_colormap)
  {
    assert(traj != NULL);
    assert(m_map_trajs.find(traj) != m_map_trajs.end()
      && "unable to remove, unknown traj");

    m_map_trajs[traj].color = ""; // removing constant color
    m_map_trajs[traj].color_map = make_pair(colormap, traj_colormap);

    // Related groups are created during the display procedure
    // so that trajectories stay on top of the tubes.
  }
  
  void VIBesFigMap::remove_trajectory(const TrajectoryVector *traj)
  {
    assert(traj != NULL);
    assert(m_map_trajs.find(traj) != m_map_trajs.end()
      && "unable to remove, unknown traj");

    m_map_trajs.erase(traj);
  }

  void VIBesFigMap::add_beacon(const Beacon& beacon, double width, const string& color)
  {
    // Simply directly drawn
    draw_beacon(beacon, width, color, vibesParams("figure", name(), "group", "beacons"));
  }

  void VIBesFigMap::add_beacons(const vector<Beacon>& v_beacons, double width, const string& color)
  {
    // Simply directly drawn
    for(int i = 0 ; i < v_beacons.size() ; i++)
      add_beacon(v_beacons[i], width, color);
  }
  
  void VIBesFigMap::add_observation(const IntervalVector& obs, const TrajectoryVector *traj, const string& color)
  {
    assert(traj != NULL);
    assert(m_map_trajs.find(traj) != m_map_trajs.end()
      && "unknown traj, must be added beforehand");

    // Simply directly drawn
    draw_observation(obs, traj, color, vibesParams("figure", name(), "group", "obs"));
  }
  
  void VIBesFigMap::add_observations(const vector<IntervalVector>& v_obs, const TrajectoryVector *traj, const string& color)
  {
    assert(traj != NULL);
    assert(m_map_trajs.find(traj) != m_map_trajs.end()
      && "unknown traj, must be added beforehand");

    // Simply directly drawn
    for(int i = 0 ; i < v_obs.size() ; i++)
      add_observation(v_obs[i], traj, color);
  }

  const IntervalVector VIBesFigMap::draw_tube(const TubeVector *tube)
  {
    assert(tube != NULL);
    assert(m_map_tubes.find(tube) != m_map_tubes.end()
      && "unknown tube, must be added beforehand");

    IntervalVector viewbox(2, Interval::EMPTY_SET);

    // Computing viewbox

      Vector pos_lb(2), pos_ub(2);
      for(int i = 0 ; i < 2 ; i++)
      {
        int index = (i == 0) ? m_map_tubes[tube].index_x : m_map_tubes[tube].index_y;

        if(!(*tube)[index].codomain().is_unbounded())
        {
          pos_lb[i] = (*tube)[index].codomain().lb();
          pos_ub[i] = (*tube)[index].codomain().ub();
        }
        
        else // some slices can be [-oo,+oo], maybe not all of them
        {
          pos_lb[i] = NAN;
          pos_ub[i] = NAN;

          for(const Slice *s = (*tube)[index].first_slice() ; s != NULL ; s = s->next_slice())
          {
            Interval codomain = s->codomain();
            if(!codomain.is_unbounded())
            {
              pos_lb[i] = std::isnan(pos_lb[i]) || pos_lb[i] > codomain.lb() ? codomain.lb() : pos_lb[i];
              pos_ub[i] = std::isnan(pos_ub[i]) || pos_ub[i] < codomain.ub() ? codomain.ub() : pos_ub[i];
            }
          }
        }
      }
     
      viewbox = IntervalVector(pos_lb) | pos_ub;

    // Displaying tube

      draw_slices(tube);

      // The background is the previous version of the tube (before contraction).
      // If a copy of the tube has not been done,
      // we make one and no display is done.
      if(m_map_tubes[tube].tube_x_copy == NULL)
      {
        m_map_tubes[tube].tube_x_copy = new Tube((*tube)[m_map_tubes[tube].index_x]);
        m_map_tubes[tube].tube_y_copy = new Tube((*tube)[m_map_tubes[tube].index_y]);
      }

    return viewbox;
  }

  const IntervalVector VIBesFigMap::draw_trajectory(const TrajectoryVector *traj, float points_size)
  {
    assert(traj != NULL);
    assert(m_map_trajs.find(traj) != m_map_trajs.end()
      && "unknown traj, must be added beforehand");
    assert(points_size >= 0.);

    std::ostringstream o;
    o << "traj_" << m_map_trajs[traj].name;
    string group_name = o.str();
    vibes::clearGroup(name(), group_name);
    vibes::newGroup(group_name, m_map_trajs[traj].color, vibesParams("figure", name()));

    IntervalVector viewbox(2, Interval::EMPTY_SET);
    if(traj->domain().is_unbounded() || traj->domain().is_empty())
      return viewbox;

    vector<double> v_x, v_y;
    int index_x = m_map_trajs[traj].index_x;
    int index_y = m_map_trajs[traj].index_y;

    // Color maps

      vector<string> v_colors;
      Trajectory identity_traj;
      identity_traj.set(traj->domain().lb(), traj->domain().lb());
      identity_traj.set(traj->domain().ub(), traj->domain().ub());

      const Trajectory *traj_colormap = &identity_traj;
      if(m_map_trajs[traj].color_map.second != NULL)
        traj_colormap = m_map_trajs[traj].color_map.second;

    if((*traj)[index_x].sampled_map().size() != 0)
    {
      typename map<double,double>::const_iterator it_scalar_values_x, it_scalar_values_y;
      it_scalar_values_x = (*traj)[index_x].sampled_map().begin();
      it_scalar_values_y = (*traj)[index_y].sampled_map().begin();

      while(it_scalar_values_x != (*traj)[index_x].sampled_map().end())
      {
        if(m_restricted_tdomain.contains(it_scalar_values_x->first))
        {
          if(points_size != 0.)
            vibes::drawPoint(it_scalar_values_x->second, it_scalar_values_y->second,
                             points_size,
                             vibesParams("figure", name(), "group", group_name));

          else
          {
            v_x.push_back(it_scalar_values_x->second);
            v_y.push_back(it_scalar_values_y->second);
            if(m_map_trajs[traj].color == "")
              v_colors.push_back(rgb2hex(m_map_trajs[traj].color_map.first.color(it_scalar_values_x->first, *traj_colormap)));
          }
        }

        viewbox[0] |= Interval(it_scalar_values_x->second);
        viewbox[1] |= Interval(it_scalar_values_y->second);

        it_scalar_values_x++;
        it_scalar_values_y++;
      }
    }

    else
    {
      for(double t = traj->domain().lb() ; t <= traj->domain().ub() ; t+=traj->domain().diam()/TRAJMAP_NB_DISPLAYED_POINTS)
      {
        double x = (*traj)[index_x](t);
        double y = (*traj)[index_y](t);

        viewbox[0] |= x;
        viewbox[1] |= y;

        if(!m_restricted_tdomain.contains(t))
          continue;

        if(points_size != 0.)
          vibes::drawPoint(x, y, points_size, vibesParams("figure", name(), "group", group_name));

        else
        {
          v_x.push_back(x);
          v_y.push_back(y);
          if(m_map_trajs[traj].color == "")
            v_colors.push_back(rgb2hex(m_map_trajs[traj].color_map.first.color(t, *traj_colormap)));
        }
      }
    }

    vibes::Params params = vibesParams("figure", name(), "group", group_name);

    if(m_map_trajs[traj].color == "")
      for(int i = 0 ; i < v_x.size()-1 ; i++) // shaded lines
      {
        vector<double> v_local_x, v_local_y;
        v_local_x.push_back(v_x[i]); v_local_x.push_back(v_x[i+1]);
        v_local_y.push_back(v_y[i]); v_local_y.push_back(v_y[i+1]);

        vibes::drawLine(v_local_x, v_local_y, v_colors[i], params);
      }

    else
      vibes::drawLine(v_x, v_y, params);

    draw_vehicle((traj->domain() & m_restricted_tdomain).ub(), traj, params);

    return viewbox;
  }

  void VIBesFigMap::draw_slices(const TubeVector *tube)
  {
    assert(tube != NULL);
    assert(m_map_tubes.find(tube) != m_map_tubes.end()
      && "unknown tube, must be added beforehand");

    // Reduced number of slices:
    int step = max((int)(tube->nb_slices() / MAP_SLICES_NUMBER_TO_DISPLAY), 1);

    // 1. Background:
    if(m_draw_tubes_backgrounds)
    {
      ostringstream o;
      o << "tube_" << m_map_tubes[tube].name << "_bckgrnd";
      string group_name = o.str();
      vibes::clearGroup(name(), group_name);
      vibes::Params params = vibesParams("figure", name(), "group", group_name);

      // if available, the copy of the tube (old version) is displayed
      if(m_map_tubes[tube].tube_x_copy != NULL)
      {
        string color = DEFAULT_MAPBCKGRND_COLOR;

        for(int k = 0 ; k < m_map_tubes[tube].tube_x_copy->nb_slices() ;
            k += step * 2) // less slices for the background
        {
          IntervalVector box(2);
          box[0] = (*m_map_tubes[tube].tube_x_copy)(k);
          box[1] = (*m_map_tubes[tube].tube_y_copy)(k);
          draw_box(box, color, params);
        }
      }
    }

    // 2. Foreground
    {
      if((*tube)[m_map_tubes[tube].index_x].is_empty() || (*tube)[m_map_tubes[tube].index_y].is_empty())
        cout << "VIBesFigMap: warning, empty tube " << m_map_tubes[tube].name << endl;

      ostringstream o;
      o << "tube_" << m_map_tubes[tube].name;
      string group_name = o.str();
      vibes::clearGroup(name(), group_name);
      vibes::Params params = vibesParams("figure", name(), "group", group_name);

      // Color map and related trajectory

        const ColorMap *color_map = &m_map_tubes[tube].color_map.first;

        Trajectory identity_traj;
        identity_traj.set(tube->domain().lb(), tube->domain().lb());
        identity_traj.set(tube->domain().ub(), tube->domain().ub());

        const Trajectory *traj_colormap = &identity_traj;
        if(m_map_tubes[tube].color_map.second != NULL)
          traj_colormap = m_map_tubes[tube].color_map.second;

      int k0, kf, kstep;
      bool from_first_to_last = m_map_tubes[tube].from_first_to_last;
      IntervalVector prev_box(2); // used for diff display

      if(from_first_to_last) // Drawing from last to first box
      {
        k0 = 0;
        kf = tube->nb_slices()-1;
      }

      else
      {
        k0 = tube->nb_slices()-1;
        kf = 0;
      }

      for(int k = k0 ;
          (from_first_to_last && k <= kf) || (!from_first_to_last && k >= kf) ;
          k+= from_first_to_last ? max(1,min(step,kf-k)) : -max(1,min(step,k)))
      {
        IntervalVector box(2);
        box[0] = (*tube)[m_map_tubes[tube].index_x](k);
        box[1] = (*tube)[m_map_tubes[tube].index_y](k);

        string color = m_map_tubes[tube].color;
        if(color == "") // then defined by a color map
        {
          color = rgb2hex(color_map->color((*tube)[0].slice(k)->domain().mid(), *traj_colormap));
          color = color + "[" + color + "]";
        }

        if(!color_map->is_opaque() && k != k0)
        {
          IntervalVector* diff_list;
          int nb_box = box.diff(prev_box, diff_list);

          for (int i = 0; i < nb_box; i++ )
            draw_box(*(diff_list+i), color, params);

          delete[] diff_list;
        }

        else
          draw_box(box, color, params);

        prev_box = box;
      }
      
      IntervalVector first_box(2);
      double tlb = (m_restricted_tdomain & tube->domain()).lb();
      first_box[0] = (*tube)[m_map_tubes[tube].index_x](tlb);
      first_box[1] = (*tube)[m_map_tubes[tube].index_y](tlb);
      draw_box(first_box, "#C02600[]", params); // red
      
      IntervalVector last_box(2);
      double tub = (m_restricted_tdomain & tube->domain()).ub();
      last_box[0] = (*tube)[m_map_tubes[tube].index_x](tub);
      last_box[1] = (*tube)[m_map_tubes[tube].index_y](tub);
      draw_box(last_box, "#47A040[]", params); // green
    }
  }

  void VIBesFigMap::draw_vehicle(double t, const TrajectoryVector *traj, const vibes::Params& params)
  {
    assert(traj != NULL);
    assert(m_map_trajs.find(traj) != m_map_trajs.end()
      && "unknown traj, must be added beforehand");
    assert(traj->domain().contains(t));

    double robot_x = (*traj)[m_map_trajs[traj].index_x](t);
    double robot_y = (*traj)[m_map_trajs[traj].index_y](t);
    double robot_heading;

    if(m_map_trajs[traj].index_heading == -1) // heading traj not available
    {
      double next_t;
      float delta_t = traj->domain().diam() / 10000.;
      if(t >= traj->domain().lb() + delta_t) next_t = t - delta_t;
      else next_t = t + delta_t;

      double robot_next_x = (*traj)[m_map_trajs[traj].index_x](next_t);
      double robot_next_y = (*traj)[m_map_trajs[traj].index_y](next_t);
      robot_heading = std::atan2(robot_y - robot_next_y, robot_x - robot_next_x);
      if(next_t > t) robot_heading += M_PI;
    }

    else
    {
      assert((*traj)[m_map_trajs[traj].index_heading].domain() == (*traj)[m_map_trajs[traj].index_x].domain());
      assert((*traj)[m_map_trajs[traj].index_heading].domain().contains(t));
      robot_heading = (*traj)[m_map_trajs[traj].index_heading](t);
    }

    vibes::drawAUV(robot_x, robot_y, robot_heading * 180. / M_PI, m_robot_size, "gray[yellow]", params);
  }

  void VIBesFigMap::draw_beacon(const Beacon& beacon, double width, const string& color, const vibes::Params& params)
  {
    vibes::newGroup("beacons", DEFAULT_BEACON_COLOR, vibesParams("figure", name()));
    IntervalVector drawn_box = beacon.pos().subvector(0,1);
    draw_box(drawn_box.inflate(width/2.), color, params);
  }

  void VIBesFigMap::draw_observation(const IntervalVector& obs, const TrajectoryVector *traj, const string& color, const vibes::Params& params)
  {
    assert(obs.size() == 3);
    assert(traj != NULL);
    assert(m_map_trajs.find(traj) != m_map_trajs.end()
      && "unknown traj, must be added beforehand");

    vibes::newGroup("obs", DEFAULT_OBS_COLOR, vibesParams("figure", name()));
    vector<double> v_x, v_y;
    v_x.push_back((*traj)[m_map_trajs[traj].index_x](obs[0].mid()));
    v_y.push_back((*traj)[m_map_trajs[traj].index_y](obs[0].mid()));
    v_x.push_back(v_x[0] + std::cos(obs[2].mid()) * obs[1].mid());
    v_y.push_back(v_y[0] + std::sin(obs[2].mid()) * obs[1].mid());
    draw_line(v_x, v_y, color, params);
  }
}