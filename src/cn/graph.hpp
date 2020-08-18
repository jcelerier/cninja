#pragma once
#include <boost/graph/directed_graph.hpp>

#include <set>
#include <vector>

#include <string_view>
#include <unordered_map>

namespace cn
{
struct Options;
class Graph
{
public:
  // Pass a list of options, {"asan", "era=10.14", ...}
  explicit Graph(const Options& options);

  // Get a toolchain file in return
  std::string generate();

private:
  struct node
  {
    std::string name;
    std::string content;

    struct comparator
    {
      using is_transparent = std::true_type;
      bool operator()(const std::unique_ptr<node>& lhs, const std::unique_ptr<node>& rhs) const
          noexcept
      {
        return std::less<>{}(lhs->name, rhs->name);
      }

      bool operator()(const node* lhs, const std::unique_ptr<node>& rhs) const noexcept
      {
        return std::less<>{}(lhs->name, rhs->name);
      }
      bool operator()(const std::unique_ptr<node>& lhs, const node* rhs) const noexcept
      {
        return std::less<>{}(lhs->name, rhs->name);
      }

      bool operator()(const std::unique_ptr<node>& lhs, const std::string& rhs) const noexcept
      {
        return std::less<>{}(lhs->name, rhs);
      }
      bool operator()(const std::string& lhs, const std::unique_ptr<node>& rhs) const noexcept
      {
        return std::less<>{}(lhs, rhs->name);
      }

      bool operator()(const std::unique_ptr<node>& lhs, std::string_view rhs) const noexcept
      {
        return std::less<>{}(lhs->name, rhs);
      }
      bool operator()(std::string_view lhs, const std::unique_ptr<node>& rhs) const noexcept
      {
        return std::less<>{}(lhs, rhs->name);
      }
    };
  };

  using vertex_t = node*;
  using vertex_index_t = void*;
  using graph_t = boost::directed_graph<vertex_t>;

#if defined(CNINJA_DEBUG_GRAPH)
  void debug_graph();
#endif

  node* add_option(const std::string_view& opt);
  void add_requirements(const std::string& name, const std::string& content);

  // "from" depends on "to"
  void add_dependency(const std::string& from, const std::string& to);

  static std::pair<std::string, std::string>
  split_name_and_argument(const std::string_view& input) noexcept;
  node* insert_content(std::string_view name, std::string_view argument, std::string content);
  static bool is_fixed_stage(std::string_view name) noexcept;

  const Options& m_options;

  node* m_startStage{};
  node* m_preStage{};
  node* m_defaultStage{};
  node* m_postStage{};
  node* m_finishStage{};

  graph_t m_graph;
  std::set<std::unique_ptr<node>, node::comparator> m_content;
  std::unordered_map<node*, void*> m_index;

  std::map<std::string, node*> m_handledOptions;
};
}
