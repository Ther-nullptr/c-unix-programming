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
    std::vector<time_t> activityTimestamps;
    std::vector<Location> visitedLocations;
};


struct CovidStatus
{
    Location location;
    time_t timestamp;
};


class CovidMonitor
{
public:
    void addPerson(const Person &person)
    {
        people[person.id] = person;
    }

    void addInfectedLocation(const Location &location)
    {
        infectedLocations.push_back(location);
    }

    void recordActivity(const std::string &personId, const time_t &timestamp, const Location &location)
    {
        people[personId].activityTimestamps.push_back(timestamp);
        people[personId].visitedLocations.push_back(location);
    }

    std::vector<CovidStatus> getRecentActivities(const std::string &personId, int days)
    {
        std::vector<CovidStatus> recentActivities;
        time_t now = time(nullptr);
        time_t cutoff = now - (days * 24 * 60 * 60);

        for (int i = 0; i < people[personId].activityTimestamps.size(); i++)
        {
            if (people[personId].activityTimestamps[i] >= cutoff)
            {
                CovidStatus status = {people[personId].visitedLocations[i], people[personId].activityTimestamps[i]};
                recentActivities.push_back(status);
            }
        }

        return recentActivities;
    }

    int getInfectedCountInRange(const Location &location, double range)
    {
        int count = 0;

        for (const auto &infectedLocation : infectedLocations)
        {
            double distance = calculateDistance(location, infectedLocation);
            if (distance <= range)
            {
                count++;
            }
        }

        return count;
    }

private:
    std::map<std::string, Person> people;
    std::vector<Location> infectedLocations;

    double calculateDistance(const Location &loc1, const Location &loc2)
    {
        return std::sqrt(std::pow(loc1.latitude - loc2.latitude, 2) + std::pow(loc1.longitude - loc2.longitude, 2));
    }
};

int main()
{
    CovidMonitor monitor;

    // add some people
    Person person1 = {"P1", {}, {}};
    Person person2 = {"P2", {}, {}};
    monitor.addPerson(person1);
    monitor.addPerson(person2);

    // add some infected locations
    Location location1 = {"Unit 201", 32, 121};
    Location location2 = {"Unit 202", 33, 122};
    Location location3 = {"Unit 101", 30, 120};
    Location location4 = {"Unit 102", 31, 121};
    Location location5 = {"Unit 103", 34, 123};
    Location location6 = {"Unit 104", 35, 129};
    Location location7 = {"Unit 105", 30, 120.5};
    Location location8 = {"Unit 106", 30.25, 120.25};

    monitor.addInfectedLocation(location1);
    monitor.addInfectedLocation(location2);
    monitor.addInfectedLocation(location7);
    monitor.addInfectedLocation(location8);

    // add some activities
    time_t now = time(nullptr);
    monitor.recordActivity("P1", now - (15.0 * 24 * 60 * 60), location2); // 15 days ago
    monitor.recordActivity("P1", now - (10.0 * 24 * 60 * 60), location1); // 10 days ago
    monitor.recordActivity("P1", now - (5.0 * 24 * 60 * 60), location3); // 5 days ago
    monitor.recordActivity("P1", now - (1.0 * 24 * 60 * 60), location4); // 1 day ago
    monitor.recordActivity("P1", now - (0.5 * 24 * 60 * 60), location1); // 12 hours ago
    monitor.recordActivity("P1", now - (0.25 * 24 * 60 * 60), location5); // 6 hours ago
    monitor.recordActivity("P1", now - (0.1 * 24 * 60 * 60), location6); // 2 hours ago
    monitor.recordActivity("P2", now - (2.0 * 24 * 60 * 60), location2); // 2 days ago

    // task 1: get recent activities(14 days)
    auto recentActivities = monitor.getRecentActivities("P1", 14);
    std::cout << "Person P1 Recent Activities (last 14 days):\n";
    for (const auto &activity : recentActivities)
    {
        std::cout << "Building Unit: " << activity.location.buildingUnit << ", Timestamp: " << std::put_time(std::localtime(&activity.timestamp), "%F %T") << std::endl;
    }

    // task 2: get infected count within range
    Location queryLocation = location3;
    int infectedCount = monitor.getInfectedCountInRange(queryLocation, 1.0); // 1km range
    std::cout << "Infected count within 1km of Unit 101: " << infectedCount << std::endl;

    return 0;
}
