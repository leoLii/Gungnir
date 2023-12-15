#pragma once

#include "common/macros.h"
#include "common/math/math.h"

#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/**
 * A class to extract information about displays
 */
class GUNGNIR_API MonitorInfo
{
public:
    /**
     * Description data structure
     */
    struct MonitorDesc
    {
        std::string identifier; ///< Monitor identifier.
        math::Vector2i resolution;       ///< Resolution in pixels.
        math::Vector2f physicalSize;    ///< Physical size in inches.
        float ppi;              ///< Pixel density (points per inch).
        bool isPrimary;         ///< True if primary monitor.
    };

    /**
     * Get a list of all monitors.
     * This function is _not_ thread-safe
     * @return Returns a list of monitor configurations.
     */
    static std::vector<MonitorDesc> getMonitorDescs();

    /**
     * Display information on currently connected monitors.
     */
    static void displayMonitorInfo();
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE // namespace Gungnir
