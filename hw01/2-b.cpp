#include <iostream>
#include <vector>

enum class BallStatus 
{
    Normal,
    Lighter,
    Heavier
};


struct Ball 
{
    int id;
    BallStatus status;  // "normal", "lighter", or "heavier"
};


void printResult(Ball b) 
{
    std::cout << "The ball with different weight is Ball #" << b.id;
    if (b.status == BallStatus::Lighter) 
    {
        std::cout << " and it is lighter." << std::endl;
    } 
    else if (b.status == BallStatus::Heavier) 
    {
        std::cout << " and it is heavier." << std::endl;
    }
}


int getWeight(std::vector<Ball> ball_vec_1, std::vector<Ball> ball_vec_2)
{
    // return -1: the right is heavier; return 1: the left is heavier
    int weight = 0;
    for (int i = 0; i < ball_vec_1.size(); i++)
    {
        if (ball_vec_1[i].status == BallStatus::Lighter)
        {
            weight -= 1;
        }
        else if (ball_vec_1[i].status == BallStatus::Heavier)
        {
            weight += 1;
        }
    }

    for (int i = 0; i < ball_vec_2.size(); i++)
    {
        if (ball_vec_2[i].status == BallStatus::Lighter)
        {
            weight += 1;
        }
        else if (ball_vec_2[i].status == BallStatus::Heavier)
        {
            weight -= 1;
        }
    }
    if (weight < 0)
    {
        std::cout << "[result] The right is heavier." << std::endl;
    }
    else if (weight > 0)
    {
        std::cout << "[result] The left is heavier." << std::endl;
    }
    else
    {
        std::cout << "[result] The two groups have the same weight." << std::endl;
    }
    return weight;
}


int getWeight(Ball ball_1, Ball ball_2)
{
    int weight = 0;
    if (ball_1.status == BallStatus::Lighter)
    {
        weight -= 1;
    } 
    else if (ball_1.status == BallStatus::Heavier)
    {
        weight += 1;
    }

    if (ball_2.status == BallStatus::Lighter)
    {
        weight += 1;
    } 
    else if (ball_2.status == BallStatus::Heavier)
    {
        weight -= 1;
    }
    if (weight < 0)
    {
        std::cout << "[result] The right is heavier." << std::endl;
    }
    else if (weight > 0)
    {
        std::cout << "[result] The left is heavier." << std::endl;
    }
    else
    {
        std::cout << "[result] The two groups have the same weight." << std::endl;
    }
    return weight;
}


void solution(int chosen_id, char chosen_weight_status) 
{
    std::vector<Ball> balls(12);
    for (int i = 0; i < 12; ++i) 
    {
        balls[i].id = i;
        balls[i].status = BallStatus::Normal;
    }

    std::cout << "The chosen ball is Ball #" << chosen_id << " and it is " 
              << (chosen_weight_status == 'H' ? "heavier." : "lighter.") << std::endl;
    if (chosen_weight_status == 'L')
    {
        balls[chosen_id].status = BallStatus::Lighter;
    }
    if (chosen_weight_status == 'H')
    {
        balls[chosen_id].status = BallStatus::Heavier;
    }

    // measured result
    int measured_id = -1;
    char measured_weight_status = 'X';
    
    // divide the ball into 3 groups
    std::vector<Ball> group_a(balls.begin(), balls.begin() + 4);
    std::vector<Ball> group_b(balls.begin() + 4, balls.begin() + 8);
    std::vector<Ball> group_c(balls.begin() + 8, balls.begin() + 12);

    // time 1: a vs b
    std::cout << "[1st time] a vs b" << std::endl;
    int weight_1st_diff = getWeight(group_a, group_b);

    if (weight_1st_diff == 0)
    {
        // time 2: select 3 in c, and compare with a
        std::cout << "[2nd time] a[:3] vs c[:3]" << std::endl;
        std::vector<Ball> group_a_0_3(balls.begin(), balls.begin() + 3);
        std::vector<Ball> group_c_0_3(balls.begin() + 8, balls.begin() + 11);
        int weight_2nd_diff = getWeight(group_a_0_3, group_c_0_3);

        if (weight_2nd_diff == 0) 
        {
            // the different ball is the last ball in c
            std::cout << "[3rd time] a[3] vs c[3]" << std::endl;
            measured_id = group_c[3].id;
            int weight_3rd_diff = getWeight(group_a[3], group_c[3]);
            if (weight_3rd_diff > 0)
            {
                measured_weight_status = 'L';
            }
            else
            {
                measured_weight_status = 'H';
            }
        } 
        else 
        {
            std::cout << "[3rd time] c[0] vs c[1]" << std::endl;
            int weight_3rd_diff = getWeight(group_c[0], group_c[1]);
            if (weight_2nd_diff < 0) 
            {
                measured_weight_status = 'H';
                if (weight_3rd_diff == 0)
                {
                    measured_id = group_c[2].id;
                }
                else if (weight_3rd_diff > 0)
                {
                    measured_id = group_c[0].id;
                }
                else if (weight_3rd_diff < 0)
                {
                    measured_id = group_c[1].id;
                }
            } 
            else 
            {
                measured_weight_status = 'L';
                if (weight_3rd_diff == 0)
                {
                    measured_id = group_c[2].id;
                }
                else if (weight_3rd_diff < 0)
                {
                    measured_id = group_c[0].id;
                }
                else if (weight_3rd_diff > 0)
                {
                    measured_id = group_c[1].id;
                }
            }
        }
    } 
    else 
    {
        // time 2: a0 + c0c1c2 vs b0 + a1a2a3, leave b1b2b3
        std::cout << "[2nd time] a[0] + c[0:3] vs b[0] + a[1:4]" << std::endl;
        std::vector<Ball> group_a_0_c_0_3;
        group_a_0_c_0_3.push_back(group_a[0]);
        group_a_0_c_0_3.insert(group_a_0_c_0_3.end(), group_c.begin(), group_c.end() - 1);

        std::vector<Ball> group_b_0_a_1_4;
        group_b_0_a_1_4.push_back(group_b[0]);
        group_b_0_a_1_4.insert(group_b_0_a_1_4.end(), group_a.begin() + 1, group_a.end());

        int weight_2nd_diff = getWeight(group_a_0_c_0_3, group_b_0_a_1_4);

        if (weight_2nd_diff == 0)
        {
            // the different ball is in b1b2b3
            std::cout << "[3rd time] b[1] vs b[2]" << std::endl;
            int weight_3rd_diff = getWeight(group_b[1], group_b[2]);
            if (weight_1st_diff > 0)
            {
                measured_weight_status = 'L';
                if (weight_3rd_diff == 0)
                {
                    measured_id = group_b[3].id;
                }
                else if (weight_3rd_diff > 0)
                {
                    measured_id = group_b[2].id;
                }
                else if (weight_3rd_diff < 0)
                {
                    measured_id = group_b[1].id;
                }
            }
            else if (weight_1st_diff < 0)
            {
                measured_weight_status = 'H';
                if (weight_3rd_diff == 0)
                {
                    measured_id = group_b[3].id;
                }
                else if (weight_3rd_diff > 0)
                {
                    measured_id = group_b[1].id;
                }
                else if (weight_3rd_diff < 0)
                {
                    measured_id = group_b[2].id;
                }
            }
        }
        else if (weight_2nd_diff > 0)
        {
            if (weight_1st_diff > 0)
            {
                // the different ball is in a0 or b0
                std::cout << "[3rd time] a[0] vs c[0]" << std::endl;
                int weight_3rd_diff = getWeight(group_a[0], group_c[0]);

                if (weight_3rd_diff > 0)
                {
                    measured_id = group_a[0].id;
                    measured_weight_status = 'H';
                }
                else if (weight_3rd_diff == 0)
                {
                    measured_id = group_b[0].id;
                    measured_weight_status = 'L';
                }
            }
            else if (weight_1st_diff < 0)
            {
                // the different ball is in a1a2a3
                std::cout << "[3rd time] a[1] vs a[2]" << std::endl;
                int weight_3rd_diff = getWeight(group_a[1], group_a[2]);
                measured_weight_status = 'L';

                if (weight_3rd_diff == 0)
                {
                    measured_id = group_a[3].id;
                }
                else if (weight_3rd_diff > 0)
                {
                    measured_id = group_a[2].id;
                }
                else if (weight_3rd_diff < 0)
                {
                    measured_id = group_a[1].id;
                }
            }
        }
        else if (weight_2nd_diff < 0)
        {
            if (weight_1st_diff > 0)
            {
                // the different ball is in a1a2a3
                std::cout << "[3rd time] a[1] vs a[2]" << std::endl;
                int weight_3rd_diff = getWeight(group_a[1], group_a[2]);
                measured_weight_status = 'H';

                if (weight_3rd_diff == 0)
                {
                    measured_id = group_a[3].id;
                }
                else if (weight_3rd_diff > 0)
                {
                    measured_id = group_a[1].id;
                }
                else if (weight_3rd_diff < 0)
                {
                    measured_id = group_a[2].id;
                }
            }
            else if (weight_1st_diff < 0)
            {
                // the different ball is in a0 or b0
                std::cout << "[3rd time] a[0] vs c[0]" << std::endl;
                int weight_3rd_diff = getWeight(group_a[0], group_c[0]);

                if (weight_3rd_diff == 0)
                {
                    measured_weight_status = 'H';
                    measured_id = group_b[0].id;
                }
                else if (weight_3rd_diff < 0)
                {
                    measured_weight_status = 'L';
                    measured_id = group_a[0].id;
                }
            }
        }
    }
    // Output result
    std::cout << "The ball with different weight is Ball #" << measured_id << " and it is " 
              << (measured_weight_status == 'H' ? "heavier." : "lighter.") << std::endl;

    // right or wrong, print right with green color, wrong with red color
    if (measured_id == chosen_id && measured_weight_status == chosen_weight_status)
    {
        std::cout << "\033[1;32m[Correct]\033[0m" << std::endl;
    }
    else
    {
        std::cout << "\033[1;31m[Wrong]\033[0m" << std::endl;
    }
}


int main()
{
    for (int i = 0; i < 12; i++)
    {
        solution(i, 'L');
        solution(i, 'H');
    }
}