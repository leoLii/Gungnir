//#include "platform/window.h"
//
//#include "platform/platform.h"
//
//GUNGNIR_NAMESPACE_OPEN_SCOPE
//
//Window::Window(const Properties &properties) :
//    properties{properties}
//{
//}
//
//void Window::process_events()
//{
//}
//
//Window::Extent Window::resize(const Extent &new_extent)
//{
//	if (properties.resizable)
//	{
//		properties.extent.width  = new_extent.width;
//		properties.extent.height = new_extent.height;
//	}
//
//	return properties.extent;
//}
//
//const Window::Extent &Window::get_extent() const
//{
//	return properties.extent;
//}
//
//float Window::get_content_scale_factor() const
//{
//	return 1.0f;
//}
//
//Window::Mode Window::get_window_mode() const
//{
//	return properties.mode;
//}
//
//GUNGNIR_NAMESPACE_CLOSE_SCOPE
