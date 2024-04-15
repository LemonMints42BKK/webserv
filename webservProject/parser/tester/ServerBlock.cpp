

#include <gtest/gtest.h>
#include "../Configs.hpp"
// Demonstrate some basic assertions.
TEST(SeverBlock, test_1_error) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_1_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "validate server_name on server");
    }
    

}

TEST(SeverBlock, test_3_error) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_2_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "Error: server '{' not found");
    }

}

TEST(SeverBlock, test_5_error) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_5_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "validate server not have location");
    }

}

TEST(SeverBlock, test_2_error) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_2_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "Error: server '{' not found");
    }

}

TEST(SeverBlock, test_4_error) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_4_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "Error: server '{' not found");
    }

}