#include <map>
#include <ctime>
#include <cmath>
#include <vector>
#include <iomanip>
#include <iostream>


struct Location
{
    std::string buildingUnit;
    double latitude;
    double longitude;
};


struct Person
{
    std::string id;
    Location location;
    std::vector<time_t> activityTimestamps; // 存储活动时间
};


class CovidMonitor
{
public:
    void addPerson(const Person &person)
    {
        people[person.id] = person;
    }

    void recordActivity(const std::string &personId, const time_t &timestamp)
    {
        people[personId].activityTimestamps.push_back(timestamp);
    }

    std::vector<time_t> getRecentActivities(const std::string &personId, int days)
    {
        std::vector<time_t> recentActivities;
        time_t now = time(nullptr);
        time_t cutoff = now - (days * 24 * 60 * 60); // 计算时间戳

        for (const auto &timestamp : people[personId].activityTimestamps)
        {
            if (timestamp >= cutoff)
            {
                recentActivities.push_back(timestamp);
            }
        }

        return recentActivities;
    }

    int getInfectedCountInRange(const Location &location, double range)
    {
        int count = 0;

        // 简化：假设有一些感染者数据
        std::vector<Location> infectedLocations = {
            {"Unit A", 30.0, 120.0}, // 假设感染者位置
            {"Unit B", 30.005, 120.001},
            {"Unit C", 30.01, 120.01}};

        for (const auto &infected : infectedLocations)
        {
            double distance = calculateDistance(location, infected);
            if (distance <= range)
            {
                count++;
            }
        }

        return count;
    }

private:
    std::map<std::string, Person> people;

    // 计算两点之间的距离（简化版，假设使用直线距离）
    double calculateDistance(const Location &loc1, const Location &loc2)
    {
        // 实际应用中应使用地理坐标计算方法（如 Haversine 公式）
        return std::sqrt(std::pow(loc1.latitude - loc2.latitude, 2) + std::pow(loc1.longitude - loc2.longitude, 2)) * 100; // 简化为实际距离
    }
};

int main()
{
    CovidMonitor monitor;

    // 添加人员
    Person person1 = {"P1", {"Unit 101", 30.0, 120.0}};
    Person person2 = {"P2", {"Unit 102", 30.005, 120.001}};
    monitor.addPerson(person1);
    monitor.addPerson(person2);

    // 记录活动
    time_t now = time(nullptr);
    monitor.recordActivity("P1", now - (5 * 24 * 60 * 60)); // 5天前
    monitor.recordActivity("P1", now - (1 * 24 * 60 * 60)); // 1天前
    monitor.recordActivity("P2", now - (2 * 24 * 60 * 60)); // 2天前

    // 查询最近活动
    auto recentActivities = monitor.getRecentActivities("P1", 14);
    std::cout << "Person P1 Recent Activities (last 14 days):\n";
    for (const auto &activity : recentActivities)
    {
        std::cout << std::put_time(std::localtime(&activity), "%Y-%m-%d %H:%M:%S") << std::endl;
    }

    // 查询指定位置感染人数
    Location queryLocation = {"Unit 101", 30.0, 120.0};
    int infectedCount = monitor.getInfectedCountInRange(queryLocation, 1.0); // 1km范围
    std::cout << "Infected count within 1km of Unit 101: " << infectedCount << std::endl;

    return 0;
}
