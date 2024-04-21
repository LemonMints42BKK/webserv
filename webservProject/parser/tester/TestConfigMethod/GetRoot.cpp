#include <gtest/gtest.h>
#include "../../Configs.hpp"


TEST(MethodgetRoot, ErrorNotFoundRoot_1) {

    EXPECT_NO_THROW(cfg::Configs configs("../FileConfTest/TestHttpBlock/CompleteBracket.txt"));

}