#include "project3/Planner.hpp"

Planner::Planner(std::vector<std::vector<int>> Cf, Map_manager manager)
{
  Cfree = Cf;

  branch_length = 5;
  region_radius = 8;

  map = manager;
}

// Return a random point from the Cspace
std::vector<int> Planner::get_random_point()
{
  return Cfree[randNum(0, Cfree.size())];
}

bool Planner::hasObstacle(std::vector<float> Xnear, std::vector<float> Xnew)
{
  bool result = false;

  float diff1 = (Xnew[1] - Xnear[1]);
  float diff2 = (Xnew[2] - Xnear[2]);

  int decimated_index;
  float diff;

  std::vector<float> point;

  // take the greater difference
  if (fabs(diff1) > fabs(diff2))
  {
    diff = diff1;
    decimated_index = 1;
  }
  else
  {
    diff = diff2;
    decimated_index = 0;
  }

  // Creates set of points between two points
  std::vector<vector<float>> points_to_check;
  points_to_check.push_back(Xnear);

  for (int ii = 1; ii <= fabs(diff); ii++)
  {
    point.push_back(Xnear[0] + ii * diff1 / fabs(diff));
    point.push_back(Xnear[1] + ii * diff2 / fabs(diff));

    point[decimated_index] = floor(point[decimated_index]);
    points_to_check.push_back(point);

    if (floor(point[decimated_index]) != point[decimated_index])
    {
      point[decimated_index]++;
      points_to_check.push_back(point);
    }
  }

  // returns true if one of the point in between is an obstacle
  for (int jj = 0; jj < points_to_check.size(); jj++)
    if (map.checkObstacle(points_to_check[jj]))
      result = true;

  return result;
}

// returns the nearest node in the tree
std::vector<float> Planner::find_nearest(vector<int> Xrand)
{
  std::vector<float> Xnear;
  long min_distance = 1000;
  long distance;

  for (int ii = 0; ii < tree.size(); i++)
  {
    distance = calculateDistance(Xrand, tree[ii].node);
    if (distance < min_distance)
    {
      min_distance = distance;
      Xnear = Tree[ii].node;
    }
  }
  return Xnear;
}

// returns the new node displaced a particular branch length in the random node's direction
std::vector<float> Planner::new_node(std::vector<float> Xnear, std::vector<int> Xrand)
{
  std::vector<float> Xnew;
  float slope = (Xrand[1] - Xnear[1]) / (Xrand[0] - Xnear[0]);
  float adjuster = branch_length * sqrt(1 / (1 + (slope ^ 2)));

  std::vector<float> point1, point2;

  point1.push_back(Xnear[0] + adjuster);
  point1.push_back(Xnear[1] + slope * adjuster);

  point2.push_back(Xnear[0] - adjuster);
  point2.push_back(Xnear[1] - slope * adjuster);

  int distance1 = calculateDistance(Xrand, point1);
  int distance2 = calculateDistance(Xrand, point2);
  if (distance1 < distance2)
    Xnew = point1;
  else
    Xnew = point2;

  return Xnew;
}

// returns the index of nodes in neighbourhood
std::vector<int> Planner::get_neighbourhood(std::vector<float> Xnew)
{
  std::vector<int> neighbourhood;
  for (int i = 0; i < tree.size(); i++)
  {
    if (calculateDistance(tree(i).node, Xnew) < region_radius)
      neighbourhood.push_back(i);
  }
  return neighbourhood;
}

// returns the parent with least cost to come
std::vector<float> Planner::get_best_parent(std::vector<int> neighbourhood)
{
  int min = tree[neighbourhood[0]].costToCome;
  std::vector<float> Xnear = tree[neighbourhood[0]].node;
  int position = neighbourhood[0];
  for (int i = 1; i < neighbourhood.size(); i++)
  {
    if (min > tree[neighbourhood[i]].costToCome)
    {
      min = tree[neighbourhood[i]].costToCome;
      Xnear = tree[neighbourhood[i]].node;
      position = neighbourhood[i];
    }
  }
  Xnear.push_back(position);
  // The third index is the position in the tree
  return Xnear;
}

// returns the position of the parent
long findParent(long position_of_child)
{
  for (long i = 0; i < tree.size(); i++)
  {
    for (int j = 0; j < tree[i].branches.size(); j++)
    {
      if (tree[i].branches[j] == position_of_child)
        return i
    }
  }
}

// returns random number between min and max
int Planner::randNum(int min, int max)
{
  return rand() % max + min;
}

// returns the euclidian distance between two points
float Planner::calculateDistance(std::vector<float> first_point, std::vector<float> second_point)
{
  return (float)sqrt((double)pow(first_point[0] - second_point[0], 2) + (double)pow(first_point[1] - second_point[1], 2));
}

std::vector<geometry_msgs::PoseStamped> Planner::makePlan(std::vector<int> root, std::vector<int> target)
{
  Tree twig;
  twig.node = root;
  twig.costToCome = 0;

  tree.push_back(twig);

  distance_to_target = branch_length + 1;

  int count = 0;

  vector<int> Xrand, neighbourhood;
  vector<float> Xnew, Xnear, Xnearest, parent;
  int position;

  while ((distance_to_target > branch_length) || hasObstacle(target, Xnew) || count < 2000)
  {
    count = count++;

    Xrand = get_random_point();
    Xnearest = find_nearest(Xrand);
    Xnew = new_node(Xnearest, Xrand);
    //if (sum(isnan(Xnew)))
    //  continue end
    neighbourhood = get_neighbourhood(Tree, Xnew, region_radius);
    parent = get_best_parent(Tree, neighbourhood);
    Xnew.push_back(parent[0]);
    Xnew.push_back(parent[1]);
    position = parent[2];

    // Add node if obstacle not in between
    std::vector<float> point1, point2;

    point1.push_back(floor(Xnear[0]));
    point1.push_back(floor(Xnear[1]));
    point2.push_back(floor(Xnew[0]));
    point2.push_back(floor(Xnew[1]));

    if (!hasObstacle(point1, point2))
    {
      long current_no_of_nodes = tree.size();

      // Add new node
      tree[current_no_of_nodes + 1].node = Xnew;
      tree[current_no_of_nodes + 1].costToCome = tree[position].costToCome + calculateDistance(Xnear, Xnew);

      // Add child location in parent node
      tree[position].branches.push_back(current_no_of_nodes + 1);

      // check if close to target
      distance_to_target = calculateDistance(Xnew, target);

      // Rewiring
      for (int cc = 0; cc < neighbourhood.size(); cc++)
      {
        if ((tree[current_no_of_nodes + 1].costToCome + calculateDistance(tree[neighbourhood[cc]].node, Xnew)) < tree[neighbourhood[cc]].costToCome)
        {
          // If cost from new node is cheaper switch parents
          long location = findParent(neighbourhood(cc));
          tree[location].branches.erase(std::remove(tree[location].branches.begin(), tree[location].branches.end(), neighbourhood[cc]), tree[location].branches.end());
          tree[current_no_of_nodes + 1].branches.push_back(neighbourhood[cc]);
        }
      }
    }
  }

  // Adding the target Node to the tree
  long current_no_of_nodes = tree.size();
  // Add new node
  tree[current_no_of_nodes + 1].node = target;
  tree[current_no_of_nodes + 1].costToCome = tree[current_no_of_nodes].costToCome + calculateDistance(target, Xnew);

  // Add child location in parent node
  tree[current_no_of_nodes].branches.push_bask(current_no_of_nodes + 1);

  // Track the optimal path
  long node_number = current_no_of_nodes + 1;
  std::vector<int> current_node = target;
  path_point1 = [];
  path_point2 = [];
  while (~isequal(root, current_node))
  {
    path_point1 = cat(2, path_point1, current_node(1));
    path_point2 = cat(2, path_point2, current_node(2));

    node_number = findParent(Tree, node_number);
    current_node = Tree(node_number).node;
  }
}