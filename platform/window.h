#pragma once

#include "common/base.h"

#include "bgiVulkan/bgi.h"

#include <optional>
#include <string>
#include <vulkan/vulkan.hpp>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/**
 * @brief An interface class, declaring the behaviour of a Window
 */
class Window
{
public:
    struct Extent
    {
        uint32_t width;
        uint32_t height;
    };

    struct OptionalExtent
	{
		std::optional<uint32_t> width;
		std::optional<uint32_t> height;
	};

    enum class Mode
	{
		Headless,
		Fullscreen,
		FullscreenBorderless,
		Default
	};

	enum class Vsync
	{
		OFF,
		ON,
		Default
	};

    struct OptionalProperties
	{
		std::optional<std::string> title;
		std::optional<Mode>        mode;
		std::optional<bool>        resizable;
		std::optional<Vsync>       vsync;
		OptionalExtent        extent;
	};

	struct Properties
	{
		std::string title     = "";
		Mode        mode      = Mode::Default;
		bool        resizable = true;
		Vsync       vsync     = Vsync::Default;
		Extent      extent    = {1920, 1080};
	};

   /**
	 * @brief Constructs a Window
	 * @param properties The preferred configuration of the window
	 */
	Window(const Properties &properties);

	virtual ~Window() = default;

    /**
	 * @brief Gets a handle from the platform's Vulkan surface 
	 * @param instance A Vulkan instance
	 * @returns A VkSurfaceKHR handle, for use by the application
	 */
    virtual VkSurfaceKHR create_surface(driver::BgiVulkanInstance &instance) = 0;

    /**
	 * @brief Gets a handle from the platform's Vulkan surface 
	 * @param instance A Vulkan instance
	 * @param physical_device A Vulkan PhysicalDevice
	 * @returns A vk::SurfaceKHR handle, for use by the application
	 */
	virtual vk::SurfaceKHR create_surface(driver::BgiVulkanInstance instance, driver::BgiVulkanDevice device) = 0;

	/**
	 * @brief Checks if the window should be closed
	 */
	virtual bool should_close() = 0;

	/**
	 * @brief Handles the processing of all underlying window events
	 */
	virtual void process_events();

	/**
	 * @brief Requests to close the window
	 */
	virtual void close() = 0;

	/**
	 * @return The dot-per-inch scale factor
	 */
	virtual float get_dpi_factor() const = 0;

	/**
     * @return The scale factor for systems with heterogeneous window and pixel coordinates
     */
	virtual float get_content_scale_factor() const;

	/**
	 * @brief Attempt to resize the window - not gauranteed to change
	 * 
	 * @param extent The preferred window extent
	 * @return Extent The new window extent
	 */
	Extent resize(const Extent &extent);

	const Extent &get_extent() const;

	Mode get_window_mode() const;

protected:
	Properties _properties;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE
