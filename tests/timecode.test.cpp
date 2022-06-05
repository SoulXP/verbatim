#include <Catch2/catch_all.hpp>
#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "timecode.hpp"
#include <vector>
#include <string>
#include <string_view>

TEST_CASE("vtm::timecode Initialization", "[timecode][chrono][initialization]")
{
    // Regular ctors
    const vtm::timecode tc_1{};
    const vtm::timecode tc_2{3.33};

    INFO("tc_1 signed: " << tc_1.as_signed());
    INFO("tc_2 signed: " << tc_2.as_signed());
    INFO("tc_1 float: " << tc_1.as_float());
    INFO("tc_2 float: " << tc_2.as_float());

    REQUIRE(tc_1.as_signed() == 0);
    REQUIRE(tc_1.as_float() == 0.0);
    REQUIRE(tc_2.as_signed() == 3);
    REQUIRE(tc_2.as_float() == 3.33);
    REQUIRE_FALSE(tc_1.as_float() == 0.01);
    REQUIRE_FALSE(tc_2.as_float() == 3.34);
}

TEST_CASE("vtm::timecode Static Factory Functions", "[timecode][chrono][static][factory]")
{
    auto tc_1 = vtm::timecode::from_hmsf(0, 0, 0, 0, vtm::fps::fps_24);
    auto tc_2 = vtm::timecode::from_hmsf(0, 0, 0, 0, vtm::fps::fps_25);
    auto tc_3 = vtm::timecode::from_hmsf(0, 0, 0, 0, vtm::fps::fps_30);

    auto tc_4 = vtm::timecode::from_string("00:00:01:00");
    auto tc_5 = vtm::timecode::from_string(std::string("00:00:01:00"));
    auto tc_6 = vtm::timecode::from_string(std::string_view("00:00:01:00"));

    INFO("tc_1 string: " << tc_1.as_string());
    INFO("tc_2 string: " << tc_2.as_string());
    INFO("tc_3 string: " << tc_3.as_string());
    INFO("tc_4 string: " << tc_4.as_string());
    INFO("tc_5 string: " << tc_5.as_string());
    INFO("tc_6 string: " << tc_6.as_string());

    REQUIRE(tc_1.as_string() == "00:00:00:00");
    REQUIRE(tc_2.as_string() == "00:00:00:00");
    REQUIRE(tc_3.as_string() == "00:00:00:00");
    REQUIRE(tc_4.as_string() == "00:00:01:00");
    REQUIRE(tc_5.as_string() == "00:00:01:00");
    REQUIRE(tc_6.as_string() == "00:00:01:00");
}


TEST_CASE("vtm::timecode Assignment", "[timecode][chrono][operators]")
{
    vtm::timecode tc_1{100.3323};
    vtm::timecode tc_2{998};

    INFO("tc_1 signed: " << tc_1.as_signed());
    INFO("tc_2 signed: " << tc_2.as_signed());
    INFO("tc_1 float: " << tc_1.as_float());
    INFO("tc_2 float: " << tc_2.as_float());

    // Object assigment
    tc_1 = tc_2;

    REQUIRE(tc_1.as_signed() == 998);
    REQUIRE(tc_1.as_float() == 998.0);
    REQUIRE_FALSE(tc_1.as_float() == 998.01);

    // Number assignment
    tc_1 = 69420.69420;
    tc_2 = 420.420;

    REQUIRE(tc_1.as_signed() == 69421);
    REQUIRE(tc_1.as_float() == 69420.69420);
    REQUIRE_FALSE(tc_1.as_float() == 69420.69421);

    REQUIRE(tc_2.as_signed() == 420);
    REQUIRE(tc_2.as_float() == 420.420);
    REQUIRE_FALSE(tc_2.as_float() == 420.421);
}

TEST_CASE("vtm::timecode Comparison", "[timecode][chrono][operators]")
{
    vtm::timecode tc_1{690420ll};
    const vtm::timecode tc_2{690420l};

    INFO("Equality Section");
    INFO("tc_1 signed: " << tc_1.as_signed());
    INFO("tc_2 signed: " << tc_2.as_signed());
    INFO("tc_1 float: " << tc_1.as_float());
    INFO("tc_2 float: " << tc_2.as_float());

    REQUIRE(tc_1 == tc_2);
    REQUIRE(tc_2 == tc_1);

    tc_1 = 69.420;

    INFO("Inequality Section");
    INFO("tc_1 signed: " << tc_1.as_signed());
    INFO("tc_2 signed: " << tc_2.as_signed());
    INFO("tc_1 float: " << tc_1.as_float());
    INFO("tc_2 float: " << tc_2.as_float());

    REQUIRE(tc_1 != tc_2);
    REQUIRE_FALSE(tc_1 == tc_2);
    REQUIRE(tc_2 != tc_1);
    REQUIRE_FALSE(tc_2 == tc_1);

    REQUIRE(tc_1 < tc_2);
    REQUIRE(tc_1 <= tc_2);
    REQUIRE(tc_2 > tc_1);
    REQUIRE(tc_2 >= tc_1);
    REQUIRE_FALSE(tc_1 > tc_2);
    REQUIRE_FALSE(tc_1 >= tc_2);
    REQUIRE_FALSE(tc_2 < tc_1);
    REQUIRE_FALSE(tc_2 <= tc_1);
}

TEST_CASE("vtm::timecode String Representations", "[timecode][chrono][string][conversion]")
{
    vtm::timecode tc_1{0.0};
    const vtm::timecode tc_2{36.0};
    INFO("tc_1 string: " << tc_1.as_string());
    INFO("tc_2 string: " << tc_2.as_string());
    REQUIRE(tc_1.as_string() == "00:00:00:00");
    REQUIRE(tc_2.as_string() == "01:00:00:00");
}

TEST_CASE("vtm::timecode Drop Frame Representation", "[timecode][chrono]")
{
    const vtm::timecode tc_1{0.0};
    REQUIRE(true == true);
}









