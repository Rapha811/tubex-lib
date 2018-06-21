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

#ifndef VIBESFIGURETUBE_HEADER
#define VIBESFIGURETUBE_HEADER

#include "tubex_VibesFigure.h"
#include "tubex_TubeVector.h"
#include "tubex_TubeSlice.h"
#include "tubex_Trajectory.h"

namespace tubex
{
  #define DEFAULT_TUBE_NAME         "[x](·)"
  #define DEFAULT_TRAJ_NAME         "x(·)"
  #define TRAJ_NB_DISPLAYED_POINTS  10000
  // HTML color codes:
  #define DEFAULT_TRAJ_COLOR        "#276279"
  #define DEFAULT_FRGRND_COLOR      "#a2a2a2[#a2a2a2]"
  #define DEFAULT_BCKGRND_COLOR     "#d2d2d2[#d2d2d2]"
  #define DEFAULT_SLICES_COLOR      "lightGray[#a2a2a2]"
  #define DEFAULT_GATES_COLOR       "#004668[#004668]"
  #define DEFAULT_POLYGONS_COLOR    "#00536E[#2696BA]"

  class VibesFigure_Tube : public VibesFigure
  {
    public:

      VibesFigure_Tube(const std::string& fig_name, const TubeVector *tube = NULL, const Trajectory *traj = NULL);
      ~VibesFigure_Tube();

      void addTube(const TubeVector *tube, const std::string& name, const std::string& color_frgrnd = DEFAULT_FRGRND_COLOR, const std::string& color_bckgrnd = DEFAULT_BCKGRND_COLOR);
      void setTubeName(const TubeVector *tube, const std::string& name);
      void setTubeDerivative(const TubeVector *tube, const TubeVector *derivative);
      void setTubeColor(const TubeVector *tube, const std::string& color_frgrnd, const std::string& color_bckgrnd);
      void setTubeColor(const TubeVector *tube, int color_type, const std::string& color);
      void removeTube(const TubeVector *tube);

      void addTrajectory(const Trajectory *traj, const std::string& name, const std::string& color = DEFAULT_TRAJ_COLOR);
      void setTrajectoryName(const Trajectory *traj, const std::string& name);
      void setTrajectoryColor(const Trajectory *traj, const std::string& color);
      void removeTrajectory(const Trajectory *traj);

      virtual void show();
      void show(bool detail_slices);

      // Static methods (shortcuts for fast and simple use)
      static void draw(const std::string& fig_name, int x = 0, int y = 0);
      static void draw(const std::string& fig_name, const TubeVector *tube, int x = 0, int y = 0);
      static void draw(const std::string& fig_name, const Trajectory *traj, int x = 0, int y = 0);
      static void draw(const std::string& fig_name, const TubeVector *tube, const Trajectory *traj, int x = 0, int y = 0);
      static void endDrawing();

    protected:

      const ibex::IntervalVector drawTube(const TubeVector *tube, bool detail_slices = false);
      void computePolygonEnvelope(const TubeVector *tube, std::vector<double>& v_x, std::vector<double>& v_y);
      void drawSlice(const TubeSlice& slice, const vibes::Params& params) const;
      void drawSlice(const TubeSlice& slice, const TubeSlice& deriv_slice, const vibes::Params& params_slice, const vibes::Params& params_polygon) const;
      void drawGate(const ibex::Interval& gate, double t, const vibes::Params& params) const;
      const ibex::IntervalVector drawTrajectory(const Trajectory *traj, float points_size = 0.);

    protected:

      enum TubeColorType { FOREGROUND, BACKGROUND, SLICES, GATES, POLYGONS };

      struct FigTubeParams
      {
        std::map<int,std::string> m_colors;
        TubeVector *tube_copy = NULL; // to display previous values in background
        const TubeVector *tube_derivative = NULL; // to display polygons enclosed by slices
        std::string name;
      };

      struct FigTrajParams
      {
        std::string color;
        std::string name;
      };

      std::map<const TubeVector*,FigTubeParams> m_map_tubes;
      std::map<const Trajectory*,FigTrajParams> m_map_trajs;

      static std::vector<VibesFigure_Tube*> v_vibesfig_tube;
  };
}

#endif