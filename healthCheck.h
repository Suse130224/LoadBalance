#ifndef HEALTHCHECK_H
#define HEALTHCHECK_H

#include <vector>
#include "host.h"

class HealthCheck{
private:
    std::vector<Host*> m_servers;
    int m_inter;
    int m_rise;
    int m_fall;
public:
    HealthCheck(std::vector<Host*> servers, int inter, int rise, int fall);
    void check();
};

#endif
