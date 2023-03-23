#include "mu_test.h"
#include "gameOfLife.hpp"

BEGIN_TEST(basic_test)
    GameOfLife game(400,400);
    game.run();
    ASSERT_PASS();
END_TEST

TEST_SUITE(Game of life)
    TEST(basic_test)

END_SUITE
