
#include <gtest/gtest.h>
#include "../Configs.hpp"
// Demonstrate some basic assertions.
TEST(LocationBlock, NotCompleteBrackets) {

    try{
        cfg::Configs configs("../FileConfTest/TestLocationBlock/test_1_error.txt");
    }
    catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "validate root on location");
    }
    
}