#pragma once

class aircraft_t
{
public:
    enum aircraft_class
    {
        LIGHT, MEDIUM, HEAVY
    };
    enum aircraft_priority
    {
        RED, YELLOW, GREEN
    };
public:
    
    moment_t span(const aircraft_t &follower) const
    {
        switch (class_)
        {
        case LIGHT:
            return moment_t(1);
        case HEAVY:
            return moment_t(3);
        case MEDIUM:
            return moment_t(follower.class_ == HEAVY ? 2 : 3);
        }
        return moment_t(1000);
    }

    cost_t eweight() const
    {
        switch (priority_)
        {
        case RED:
            return cost_t(1);
        case YELLOW:
            return cost_t(0.5);
        case GREEN:
            return cost_t(0.1);
        }
        return cost_t(1000);
    }
    cost_t tweight() const
    {
        return eweight();
    }
public:
    string name;
    moment_t due;
    moment_t min_bound, max_bound;
    aircraft_class class_;
    aircraft_priority priority_;
};
