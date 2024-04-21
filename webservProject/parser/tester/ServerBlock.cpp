

#include <gtest/gtest.h>
#include "../Configs.hpp"
// Demonstrate some basic assertions.
TEST(SeverBlock, not_have_servername_in_server_block) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_1_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "validate server_name on server");
    }
    

}

TEST(SeverBlock, not_have_bracket_open_in_server_block) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_2_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "Error: server '{' not found");
    }

}


TEST(SeverBlock, not_have_bracket_open_in_server_block_2) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_2_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "Error: server '{' not found");
    }

}

TEST(SeverBlock, not_have_bracket_open_in_server_block_3) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_4_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "Error: server '{' not found");
    }

}

TEST(SeverBlock, not_have_listen_on_server) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_5_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "validate listen on server");
    }

}

TEST(SeverBlock, server_block_not_have_location) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_6_error.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "validate server not have location");
    }

}

TEST(SeverBlock, duplicate_root_in_server) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/test_duplicate_root_in_server.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "duplicate root in server");
    }

}

TEST(SeverBlock, not_have_root_in_server_and_location) {

    try{
        cfg::Configs configs("../FileConfTest/TestServerBlock/not_have_root.txt");
    }catch (std::exception const &e) {
        EXPECT_STREQ(e.what(), "validate server not have root");
    }

}