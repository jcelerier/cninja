#include "graph.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/graph/directed_graph.hpp>
#include <boost/graph/topological_sort.hpp>

#include <cn/builtins.hpp>
#include <cn/config.hpp>
#include <cn/fmt.hpp>
#if defined(CNINJA_DEBUG_GRAPH)
#include <boost/graph/graphviz.hpp>
#endif

#include <functional>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

namespace cn
{

namespace
{
// cninja_require(this)
static inline const std::regex require_regex{R"_(cninja_require\(([a-zA-Z0-9_=-]+)\))_"};

// cninja_optional(that)
static inline const std::regex optional_regex{R"_(cninja_optional\(([a-zA-Z0-9_=-]+)\))_"};

// foo_bar = 10.14
static inline const std::regex option_regex{R"_(([a-zA-Z0-9_-]+)\s*=\s*([a-zA-Z0-9_.-]+))_"};
}

graph::graph(std::vector<std::string_view> options)
{
  // Add the user options
  for (const auto& opt : options)
  {
    add_option(opt);
  }

  // Add the always-here things :
  m_startStage = add_option("start");
  m_preStage = add_option("pre");
  m_defaultStage = add_option("default");
  m_postStage = add_option("post");
  m_finishStage = add_option("finish");
}

graph::node* graph::add_option(const std::string_view& opt)
{
  auto [name, argument] = split_name_and_argument(opt);

  // Do nothing if we already found that option earlier.
  // This is in particular done so that options passed on the command line
  // override those in the built-ins.
  if(auto it = m_handledOptions.find(name); it != m_handledOptions.end())
    return it->second;

  // If we have e.g. default=plain, the content of "default" will be what's in plain.cmake
  // If plain.cmake is nowhere to be found, then "plain" will be passed as the %default% variable.

  if(!argument.empty())
  {
    // Look for the argument in the filesystem
    if (auto file = read_config_file(argument))
    {
      auto node = insert_content(name, argument, *std::move(file));
      m_handledOptions[name] = node;
      m_handledOptions[argument] = node;
      return node;
    }

    // Look for the argument into the cninja builtins
    const auto& bts = cn::builtins();
    if (auto it = bts.find(argument); it != bts.end())
    {
      auto node = insert_content(name, argument, it->second);
      m_handledOptions[name] = node;
      m_handledOptions[argument] = node;
      return node;
    }
  }

  // Look in the filesystem
  if (auto file = read_config_file(name))
  {
    auto node = insert_content(name, argument, *std::move(file));
    m_handledOptions[name] = node;
    return node;
  }

  // Look into the cninja builtins
  const auto& bts = cn::builtins();
  if (auto it = bts.find(name); it != bts.end())
  {
    auto node = insert_content(name, argument, it->second);
    m_handledOptions[name] = node;
    return node;
  }

  // Fail
  throw std::runtime_error(fmt::format("option {} not found.\n", opt));
}

void graph::add_requirements(const std::string& name, const std::string& content)
{
  auto for_all_matches = [&](const std::regex& rx, auto func) {
    using iterator = std::cregex_iterator;

    for(iterator match_it{content.data(), content.data() + content.size(), rx}; match_it != iterator{}; ++match_it) {
      const auto& match = *match_it;

      auto it = match.begin();
      // Check if we aren't in a comment
      if (it->first != content.data())
      {
        const char* cur_char_it = it->first;

        bool in_a_comment = false;
        while(*cur_char_it != '\n' && cur_char_it != content.data())
        {
          if(*cur_char_it == '#')
          {
            in_a_comment = true;
            break;
          }
          cur_char_it--;
        }

        if(in_a_comment)
          break;
      }

      // Skip the first match - we only want the groups
      ++it;
      for (auto end = match.end(); it != end; ++it)
      {
        func(it->str());
      }
    }
  };

  for_all_matches(require_regex, [&](const std::string& str) {
    add_option(str);
    add_dependency(name, str);
  });

  for_all_matches(optional_regex, [&](const std::string& str) {
    add_dependency(name, str);
  });
}

void graph::add_dependency(const std::string& from, const std::string& to)
{
  node* from_node{};
  if (auto it = m_content.find(from); it != m_content.end())
  {
    from_node = (*it).get();
  }
  else
  {
    // Create a placeholder
    auto n = std::make_unique<node>(node{from, std::string{}});
    from_node = n.get();
    m_index[from_node] = boost::add_vertex(from_node, m_graph);
    m_content.emplace(std::move(n));
  }

  node* to_node{};
  if (auto it = m_content.find(to); it != m_content.end())
  {
    to_node = (*it).get();
  }
  else
  {
    // Create a placeholder
    auto n = std::make_unique<node>(node{to, std::string{}});
    to_node = n.get();
    m_index[to_node] = boost::add_vertex(to_node, m_graph);
    m_content.emplace(std::move(n));
  }

  boost::add_edge(m_index[from_node], m_index[to_node], m_graph);
}

std::pair<std::string, std::string>
graph::split_name_and_argument(const std::string_view& input) noexcept
{
  std::smatch match;

  // yay for regex not supporting string_view...
  std::string input_str{input};

  if (std::regex_match(input_str, match, option_regex))
  {
    if (match.size() == 3)
    {
      return {match[1].str(), match[2].str()};
    }
  }

  return {input_str, {}};
}

graph::node* graph::insert_content(std::string_view name, std::string_view argument, std::string content)
{
  node* inserted{};

  // Handle the foo=bar case
  if (!argument.empty())
  {
    boost::replace_all(content, "%" + std::string{name} + "%", argument);
  }

  // Note that we do not replace existing things, only placeholders.
  auto it = m_content.find(name);
  if (it != m_content.end())
  {
    if ((*it)->content.empty())
    {
      inserted = (*it).get();
      (*it)->content = std::move(content);
    }
    else
    {
      return inserted;
    }
  }
  else
  {
    auto [it, ok] = m_content.emplace(std::make_unique<node>(node{std::string{name}, std::move(content)}));
    inserted = (*it).get();
    m_index[it->get()] = boost::add_vertex(it->get(), m_graph);
  }

  assert(inserted);
  add_requirements(std::string{name}, inserted->content);

  // Cleanup the content string a bit
  inserted->content = std::regex_replace(inserted->content, require_regex, "");
  inserted->content = std::regex_replace(inserted->content, optional_regex, "");
  boost::trim(inserted->content);

  return inserted;
}

bool graph::is_fixed_stage(std::string_view name) noexcept
{
  static const constexpr std::array<std::string_view, 5> names{
      "start", "pre", "default", "post", "finish"};
  return std::find(names.begin(), names.end(), name) != names.end();
}

std::string graph::generate()
{
  std::stringstream result;

  // Clean the unused nodes
  for (auto it = m_index.begin(); it != m_index.end();)
  {
    const auto& node = *it->first;
    if (is_fixed_stage(node.name))
    {
      ++it;
      continue;
    }

    if (node.content.empty())
    {
      // Cleanup
      boost::clear_vertex(it->second, m_graph);
      boost::remove_vertex(it->second, m_graph);

      auto node_it = m_content.find(&node);
      if (node_it != m_content.end())
        m_content.erase(node_it);

      it = m_index.erase(it);
    }
    else
    {
      ++it;
    }
  }

  auto default_index = m_index[m_defaultStage];
  auto post_index = m_index[m_postStage];
  for (const auto& [node, index] : m_index)
  {
    if (node->name == "start")
      continue;

    // If no edges : put it after default
    auto [in_begin, in_end] = boost::in_edges(index, m_graph);
    auto [out_begin, out_end] = boost::out_edges(index, m_graph);
    if (in_begin == in_end && out_begin == out_end)
    {
      boost::add_edge(index, default_index, m_graph);
      boost::add_edge(post_index, index, m_graph);
    }
  }
  using namespace boost;
  auto index_map = get(vertex_index, m_graph);
  std::size_t current_index = 0;

  for(auto [vbegin, vend] = boost::vertices(m_graph); vbegin != vend; ++vbegin)
      index_map[*vbegin]=current_index++;

  std::vector<vertex_index_t> c;
  boost::topological_sort(m_graph, std::back_inserter(c));

  for (auto vtx : c)
  {
    node& node = *m_graph[vtx];
    if (!node.content.empty())
    {
      result << "# cninja:  " << node.name << "\n";
      result << "list(APPEND CNINJA_FEATURES \"" << node.name << "\")\n";
      result << node.content << "\n\n";
    }
  }

  return result.str();
}

#if defined(CNINJA_DEBUG_GRAPH)
void graph::debug_graph()
{
  std::stringstream s;
  boost::write_graphviz(
      s,
      m_graph,
      [&](auto& out, const auto& v) { // vertices
        out << "[label=\"" << this->m_graph[v]->name << "\"]";
      },
      [&](auto& out, const auto& v) { // edges
        out << "[]";
      });
  std::cout << s.str() << std::endl << std::flush;
}
#endif

}
