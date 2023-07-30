// Brian Goldman
// This file doesn't actually impelement a GA. I just reused
// the file name to fit into the framework. Instead it implements
// a deterministic hill climber.

#include <limits>
#include <algorithm>
#include <utility>
using std::pair;
#include <unordered_set>
#include <deque>
#include <iostream>
using std::cout;
using std::endl;
#include <fstream>

#include "GA.h"
#include "Matrix.hpp"
#include <numeric>

GA::GA(KusiakLayoutEvaluator& evaluator, int index)
    : wfle(evaluator) {
  best_so_far = std::numeric_limits<double>::max();
  output_file = "competition_" + to_string(index) + ".tsv";
  stop_after = wfle.getNumberOfEvaluation() + eval_limit;
}

bool GA::check_obstacles(const Vector2d & point) const {
  bool valid = true;
  for (unsigned int o = 0; o < wfle.scenario.obstacles.rows and valid; o++) {
    // Ensure this turbine won't overlap the obstacles
    double xmin = wfle.scenario.obstacles.get(o, 0);
    double ymin = wfle.scenario.obstacles.get(o, 1);
    double xmax = wfle.scenario.obstacles.get(o, 2);
    double ymax = wfle.scenario.obstacles.get(o, 3);
    if (point.x > xmin && point.y > ymin && point.x < xmax && point.y < ymax) {
      valid = false;
    }
  }
  return valid;
}

bool GA::verify_point_with_grid(const Vector2d& new_point,
                                const vector<Vector2d>& existing_points,
                                const double interval) const {
  for (const auto& existing_point : existing_points) {
    if ((existing_point - new_point).magnitude() < interval) {
      return false;
    }
  }
  return true;
}

// Given two vectors, creates a legal tessellation
vector<Vector2d> GA::build_grid(const Vector2d row, const Vector2d column,
                                const double min_interval) const {
  vector<Vector2d> result;
  unordered_map<int, std::unordered_set<int>> seen_coord;
  // Set (0, 0) to be the middle of the scenario
  Vector2d origin(wfle.scenario.width / 2, wfle.scenario.height / 2);
  deque<pair<int, int>> openned(1, { 0, 0 });
  // unit directions
  const vector<pair<int, int>> offsets { { 0, 1 }, { 1, 0 }, { -1, 0 }, { 0, -1 } };
  // While there are unvisited points
  while (openned.size()) {
    // Get the oldest point, and see if you've been there before
    auto grid_point = openned.front();
    openned.pop_front();
    auto never_seen_before = seen_coord[grid_point.first].insert(grid_point.second);
    if (not never_seen_before.second) {
      continue;
    }
    // create the new point
    auto working = origin + (column * grid_point.first) + (row * grid_point.second);
    // if its in bounds
    if (0 <= working.x and working.x <= wfle.scenario.width and 0 <= working.y
        and working.y <= wfle.scenario.height) {
      if (check_obstacles(working)) {
        if (verify_point_with_grid(working, result, min_interval)) {
          result.push_back(working);
        } else {
          // The tessellation is illegal as two grid points are too close together
          return vector<Vector2d>();
        }
      }
      // Open all adjacent locations
      for (const auto & offset : offsets) {
        openned.emplace_back(grid_point.first + offset.first,
                             grid_point.second + offset.second);
      }
    }
  }
  return result;
}

double GA::points_to_coe(const vector<Vector2d> & points) {
  // Ensure no extra evaluations are performed
  if (stop_after <= wfle.getNumberOfEvaluation()) {
    cout << "Out of evaluations" << endl;
    return std::numeric_limits<double>::max();
  }
  // Shows progress
  if ((wfle.getNumberOfEvaluation() + 1) % 200 == 0) {
    cout << "Completed: " << wfle.getNumberOfEvaluation() + 1 << endl;
  }
  // Convert the points into a matrix
  Matrix<double>* layout = new Matrix<double>(points.size(), 2);
  for (size_t i = 0; i < points.size(); i++) {
    layout->set(i, 0, points[i].x);
    layout->set(i, 1, points[i].y);
  }
  // Evaluate the layout
  auto cost = wfle.evaluate(layout);
  // Record the best when found
  if (best_so_far > cost) {
    best_so_far = cost;
    cout << best_so_far << "\t" << wfle.getNumberOfEvaluation() << "\t"
         << points.size() << endl;
    std::ofstream out(output_file);
    out << "# " << cost << " " << wfle.getNumberOfEvaluation() << " "
        << points.size() << endl;
    out << "x\ty\tfit" << endl;
    auto turbine_fit = wfle.getTurbineFitnesses();
    for (size_t i = 0; i < points.size(); i++) {
      out << points[i].x << '\t' << points[i].y << '\t'
          << turbine_fit->get(i, 0) << endl;
    }
  }

  // clean up
  delete layout;
  return cost;
}

double GA::test_grid(const vector<Vector2d>& points) {
  // Determine the initial cost, and set up wfle auxilary data
  auto cost = points_to_coe(points);

  // Turn off the worst performing turbines until you get to a full substation
  auto turbine_fit = wfle.getTurbineFitnesses();
  vector<int> options(points.size());
  iota(options.begin(), options.end(), 0);
  // sort in reverse quality order
  sort(options.begin(), options.end(),
       [turbine_fit](const int & i, const int& j) {
         return turbine_fit->get(i, 0) > turbine_fit->get(j, 0);
       });
  size_t remove = (points.size() + 1) % turbines_per_substation;

  // If you don't need to remove any, or you'd need to remove all the points
  if (remove == 0 or remove >= points.size()) {
    return cost;
  }

  // Construct the new set of points
  options.resize(points.size() - remove);
  vector<Vector2d> new_points;
  for (const auto & index : options) {
    new_points.push_back(points[index]);
  }
  auto new_cost = points_to_coe(new_points);
  // The grid is the better of the two
  return min(cost, new_cost);
}

double GA::run_combination(const vector<int>& args) {
  // Prevent duplicate combinations from being evaluated
  auto lookup = seen.find(args);
  if (lookup != seen.end()) {
    return lookup->second;
  }
  double padded_interval = 8.001 * wfle.scenario.R;
  double valid_interval = 8 * wfle.scenario.R;

  // Convert the arguments to row and column offset vectors
  auto column = Vector2d::make(
      args[Angle_One] * M_PI / angle_slice,
      padded_interval + args[Magnitude_One] * padded_interval / mag_slice);
  auto row = Vector2d::make(
      args[Angle_Two] * M_PI / angle_slice,
      padded_interval + args[Magnitude_Two] * padded_interval / mag_slice);
  auto cost = std::numeric_limits<double>::max();
  // If the rows are too close together, the combination is invalid
  if (row.magnitude() < valid_interval) {
    return cost;
  }
  // If the columns are too close together, the combination is invalid
  if (column.magnitude() < valid_interval) {
    return cost;
  }
  // If (0, 1) is too close to (1, 0), the combination is invalid
  if ((row - column).magnitude() < valid_interval) {
    return cost;
  }
  auto grid = build_grid(row, column, valid_interval);
  // If build_grid failed, the combination is invalid
  if (grid.size() == 0) {
    return cost;
  }
  // Evaluate the grid, and record its quality
  auto result = test_grid(grid);
  seen[args] = result;
  return result;
}

void GA::try_all(int arg, int max_value, vector<int> & args, double & cost) {
  // For all possible alternative values
  for (int i = 0; i <= max_value; i++) {
    // create a new argument list with that value changed
    auto new_args = args;
    new_args[arg] = i;
    auto new_cost = run_combination(new_args);
    // If it improved, save the change
    if (new_cost < cost) {
      swap(cost, new_cost);
      swap(args, new_args);
    }
  }
}

void GA::run() {
  vector<int> args(4);
  // Initialize the arguments to perpendicular, one being very long
  args[Magnitude_One] = 0;
  args[Magnitude_Two] = mag_step / 2;
  args[Angle_One] = 0;
  args[Angle_Two] = angle_slice / 2;
  auto cost = run_combination(args);
  vector<int> prev_args;
  do {
    prev_args = args;
    try_all(Angle_One, angle_slice - 1, args, cost);
    try_all(Angle_Two, angle_slice - 1, args, cost);
    cout << "Angles: " << args[Angle_One] << " " << args[Angle_Two] << endl;
    try_all(Magnitude_One, mag_step, args, cost);
    try_all(Magnitude_Two, mag_step, args, cost);
    cout << "Magnitudes: " << args[Magnitude_One] << " " << args[Magnitude_Two] << endl;

  } while (prev_args != args and wfle.getNumberOfEvaluation() < stop_after);
  cout << "Go the other way!" << endl;
  // Initialize the arguments to perpendicular, one being very long, opposite of before
  args[Magnitude_One] = mag_step / 2;
  args[Magnitude_Two] = 0;
  args[Angle_One] = 0;
  args[Angle_Two] = angle_slice / 2;
  cost = run_combination(args);
  do {
    prev_args = args;
    try_all(Angle_Two, angle_slice - 1, args, cost);
    try_all(Angle_One, angle_slice - 1, args, cost);
    cout << "Angles: " << args[Angle_One] << " " << args[Angle_Two] << endl;
    try_all(Magnitude_Two, mag_step, args, cost);
    try_all(Magnitude_One, mag_step, args, cost);
    cout << "Magnitudes: " << args[Magnitude_One] << " " << args[Magnitude_Two] << endl;

  } while (prev_args != args and wfle.getNumberOfEvaluation() < stop_after);
  cout << "Out of combinations to test: " << wfle.getNumberOfEvaluation() << endl;
}
