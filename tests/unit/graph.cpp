#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cn/options.hpp>
#include <cn/graph.hpp>

void check_phases_order(const std::string& res)
{
  auto start_pos = res.find("# cninja:  start");
  auto default_pos = res.find("# cninja:  default");
  auto post_pos = res.find("# cninja:  post");
  REQUIRE(start_pos < default_pos);
  REQUIRE(default_pos < post_pos);

}
TEST_CASE( "dependency check order 1", "[dependency]" ) {
    using namespace cn;
    Options opt;
    opt.options = {"compiler=clang", "lld"};
    Graph g{opt};

    auto res = g.generate();

    auto comp_pos = res.find("# cninja:  compiler");
    auto lld_pos = res.find("# cninja:  lld");

    REQUIRE(comp_pos < lld_pos);

    check_phases_order(res);
}

TEST_CASE( "dependency check order 2", "[dependency]" ) {
    using namespace cn;
    Options opt;
    opt.options = {"lld"};
    Graph g{opt};

    auto res = g.generate();

    auto comp_pos = res.find("# cninja:  compiler");
    auto lld_pos = res.find("# cninja:  lld");

    REQUIRE(comp_pos < lld_pos);

    check_phases_order(res);
}

TEST_CASE( "dependency check order 3", "[dependency]" ) {
    using namespace cn;
    Options opt;
    opt.options = {"lld", "compiler=clang"};
    Graph g{opt};

    auto res = g.generate();

    auto comp_pos = res.find("# cninja:  compiler");
    auto lld_pos = res.find("# cninja:  lld");

    REQUIRE(comp_pos < lld_pos);

    check_phases_order(res);
}

TEST_CASE( "plain", "[dependency]" ) {
    using namespace cn;
    Options opt;
    opt.options = {"fast", "default=plain"};
    Graph g{opt};

    std::string res;
    REQUIRE_NOTHROW(res = g.generate());

    REQUIRE(!res.empty());

    check_phases_order(res);
}

TEST_CASE( "plain post", "[dependency]" ) {
    using namespace cn;
    Options opt;
    opt.options = {"default=plain", "post=plain"};
    Graph g{opt};

    std::string res;
    REQUIRE_NOTHROW(res = g.generate());

    REQUIRE(!res.empty());

    check_phases_order(res);
}

TEST_CASE( "visibility post", "[dependency]" ) {
    using namespace cn;
    Options opt;
    opt.options = {"visibility", "default=plain"};
    Graph g{opt};

    std::string res;
    REQUIRE_NOTHROW(res = g.generate());

    REQUIRE(!res.empty());

    check_phases_order(res);
}


TEST_CASE( "default=plain lld", "[dependency]" ) {
    using namespace cn;
    Options opt;
    opt.options = {"default=plain", "lld"};
    Graph g{opt};

    std::string res;
    REQUIRE_NOTHROW(res = g.generate());

    REQUIRE(!res.empty());

    check_phases_order(res);
}
