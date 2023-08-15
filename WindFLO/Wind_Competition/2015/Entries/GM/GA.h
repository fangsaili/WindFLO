// Brian Goldman
// This file doesn't actually impelement a GA. I just reused
// the file name to fit into the framework. Instead it implements
// a deterministic hill climber.
#ifndef GA_H
#define GA_H

#include <vector>
#include <unordered_map>
using std::vector;

#include "KusiakLayoutEvaluator.h"

// Maximum number of evaluations allowed per instance
const int eval_limit = 2000;

const int turbines_per_substation = 30;

// Some magic numbers that control the precision of the hill climber
// const int angle_slice = 36;
const int angle_slice = 36*2;

const int mag_slice = 16*2;
const int mag_step = 4 * mag_slice;

// Utility class designed to make 2d vector math easier
struct Vector2d {
  double x, y;
  Vector2d(double _x = 0, double _y = 0)
      : x(_x),
        y(_y) {
  }
  Vector2d operator+(const Vector2d& rhs) const {
    return Vector2d(x + rhs.x, y + rhs.y);
  }
  Vector2d operator-(const Vector2d& rhs) const {
    return Vector2d(x - rhs.x, y - rhs.y);
  }
  Vector2d operator*(const int scalar) const {
    return Vector2d(x * scalar, y * scalar);
  }
  double magnitude() {
    return sqrt(x * x + y * y);
  }
  static Vector2d make(double angle, double magnitude) {
    return Vector2d(magnitude * cos(angle), magnitude * sin(angle));
  }
};

// Taken from Boost: http://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
template<class T>
inline void hash_combine(std::size_t& seed, const T& v) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// My own creation, used to hash vectors which contain any hashable type.
template<class T>
struct vector_hash {
  std::size_t operator()(std::vector<T> const& vec) const {
    std::size_t current_hash = 0;
    for (const auto& i : vec) {
      hash_combine(current_hash, i);
    }
    return current_hash;
  }
};

// Meat of the implementation. Performs determinisitic local search
class GA {
 public:
  GA(KusiakLayoutEvaluator& evaluator, int index);
  // Perform one complete optimization of a given layout
  void run();
 private:
  bool check_obstacles(const Vector2d & point) const;
  bool verify_point_with_grid(const Vector2d& new_point,
                              const vector<Vector2d>& existing_points,
                              const double interval) const;
  vector<Vector2d> build_grid(const Vector2d row, const Vector2d column,
                              const double min_interval) const;
  // Converts a list of turbine locations into a cost of energy
  double points_to_coe(const vector<Vector2d> & points);
  double test_grid(const vector<Vector2d> & points);

  enum Arguments {
    Magnitude_One,
    Magnitude_Two,
    Angle_One,
    Angle_Two
  };
  double run_combination(const vector<int> & args);

  void try_all(int arg, int max_value, vector<int> & args, double & cost);

  // Hold onto the competition evaluator
  KusiakLayoutEvaluator& wfle;
  int stop_after;
  std::unordered_map<vector<int>, double, vector_hash<int>> seen;
  double best_so_far;
  string output_file;
};

#endif
