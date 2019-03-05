#ifndef HEALTHCHECK_H
#define HEALTHCHECK_H

#include <vector>
#include "host.h"

static const int SEND_TIME_INTERVAL = 5; //Time interval of health check
static const int THRESHOLD = 3; // State switching threshold


class HealthCheck{
private:
    std::vector<Host> m_servers;
    void check();
public:
    HealthCheck(std::vector<Host> servers);
    void start();
};

#endif
