#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cn/options.hpp>
#include <cn/graph.hpp>

TEST_CASE( "dependency check order 1", "[dependency]" ) {
    using namespace cn;
    Options opt;
    opt.options = {"compiler=clang", "lld"};
    Graph g{opt};

    auto res = g.generate();

    auto comp_pos = res.find("# cninja:  compiler");
    auto lld_pos = res.find("# cninja:  lld");

    REQUIRE(comp_pos < lld_pos);
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
}

TEST_CASE( "plain", "[dependency]" ) {
    using namespace cn;
    Options opt;
    opt.options = {"fast", "default=plain"};
    Graph g{opt};

    std::string res;
    REQUIRE_NOTHROW(res = g.generate());

    REQUIRE(!res.empty());
}
