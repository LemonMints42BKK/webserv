#include <gtest/gtest.h>
#include "../Configs.hpp"
// Demonstrate some basic assertions.
TEST(HttpBlock, CompleteBrackets) {

    EXPECT_NO_THROW(cfg::Configs configs("../FileConfTest/TestHttpBlock/CompleteBracket.txt"));

}

TEST(HttpBlock, NotCompleteBrackets) {

    try{
        cfg::Configs configs("../FileConfTest/TestHttpBlock/NotCompleteBracket.txt");
    }
    catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "httpError: unknow directive ");
    }
    
}

TEST(HttpBlock, test_1_error) {

    try{
        cfg::Configs configs("../FileConfTest/TestHttpBlock/test_1_error.txt");
    }
    catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "validate http mush have server");
    }
    
}

TEST(HttpBlock, test_2_error) {

    try{
        cfg::Configs configs("../FileConfTest/TestHttpBlock/test_2_error.txt");
    }
    catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "Error: http '{' not found");
    }
    
}

TEST(HttpBlock, test_3_error) {

    try{
        cfg::Configs configs("../FileConfTest/TestHttpBlock/NotCompleteBracket.txt");
    }
    catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "httpError: unknow directive ");
    }
    
}

TEST(HttpBlock, emtry) {

    try{
        cfg::Configs configs("../FileConfTest/TestHttpBlock/emtry.txt");
    }
    catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "validate Config no http or more http");
    }
    
}

TEST(HttpBlock, error_httpd) {

    try{
        cfg::Configs configs("../FileConfTest/TestHttpBlock/error_httpd.txt");
    }
    catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "validate Config no http or more http");
    }
    
}

TEST(HttpBlock, UnknowToken) {

    try{
        cfg::Configs configs("../FileConfTest/TestHttpBlock/unknowToken.txt");
    }
    catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "httpError: unknow directive A");
    }
    
}