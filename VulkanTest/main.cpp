#include <cstdlib>
#include <iostream>
#include <string>
#include <bitset>

#define VKCPP_ENHANCED_MODE
#include "vk_cpp.h"

const std::string vkApiVersionToString(const std::uint32_t apiVersion) {
	std::string major = std::to_string(apiVersion >> 22);
	std::string minor = std::to_string((apiVersion >> 12) & 0x000003FF);
	std::string patch = std::to_string(apiVersion & 0x00000FFF);
	return major + "." + minor + "." + patch;
}

const std::string vkResultToString(const vk::Result & result) {
	switch (result)	{
		case vk::Result::eVkSuccess:
			return "Success";
			break;
		case vk::Result::eVkNotReady:
			return "Not ready";
			break;
		case vk::Result::eVkEventSet:
			return "Event set";
			break;
		case vk::Result::eVkEventReset:
			return "Event reset";
			break;
		case vk::Result::eVkIncomplete:
			return "Incomplete";
			break;
		case vk::Result::eVkErrorOutOfHostMemory:
			return "Error: Out of host memory";
			break;
		case vk::Result::eVkErrorOutOfDeviceMemory:
			return "Error: Out of device memory";
			break;
		case vk::Result::eVkErrorInitializationFailed:
			return "Error: Initialization failed";
			break;
		case vk::Result::eVkErrorDeviceLost:
			return "Error: Device lost";
			break;
		case vk::Result::eVkErrorMemoryMapFailed:
			return "Error: Memory map failed";
			break;
		case vk::Result::eVkErrorLayerNotPresent:
			return "Error: Layer not present";
			break;
		case vk::Result::eVkErrorExtensionNotPresent:
			return "Error: Extension not present";
			break;
		case vk::Result::eVkErrorFeatureNotPresent:
			return "Error: Feature not present";
			break;
		case vk::Result::eVkErrorIncompatibleDriver:
			return "Error: Incompatible driver";
			break;
		case vk::Result::eVkErrorTooManyObjects:
			return "Error: Too many objects";
			break;
		case vk::Result::eVkErrorFormatNotSupported:
			return "Error: Format not supported";
			break;
		case vk::Result::eVkTimeout:
			return "Timeout";
			break;
		default:
			return "NOT_DEFINED";
	}
}

const std::string vkLayerInfoToString(const vk::LayerProperties & prop) {
	std::string str;
	std::string name = prop.layerName();
	str += "Layer name: " + name;
	return str;
}

const std::string vkPhysicalDeviceTypeToString(const vk::PhysicalDeviceType & type) {
	switch (type) {
		case vk::PhysicalDeviceType::eOther:
			return "Other";
			break;
		case vk::PhysicalDeviceType::eIntegratedGpu:
			return "Integrated GPU";
			break;
		case vk::PhysicalDeviceType::eDiscreteGpu:
			return "Discrete GPU";
			break;
		case vk::PhysicalDeviceType::eVirtualGpu:
			return "Virtual GPU";
			break;
		case vk::PhysicalDeviceType::eCpu:
			return "CPU";
			break;
		default:
			return "NOT_DEFINED";
	}
}

const std::string vKPhysicalDevicePropertiesToString(const vk::PhysicalDeviceProperties & prop) {
	std::string str;
	std::string name = prop.deviceName();
	str += "Device name: " + name + "\n";
	str += "API version: " + vkApiVersionToString(prop.apiVersion()) + "\n";
	str += "Driver version: " + vkApiVersionToString(prop.driverVersion()) + "\n";
	str += "Vendor ID: " + std::to_string(prop.vendorID()) + "\n";
	str += "Device ID: " + std::to_string(prop.deviceID()) + "\n";
	str += "Device type: " + vkPhysicalDeviceTypeToString(prop.deviceType());
	return str;
}

VKAPI_ATTR VkBool32 VKAPI_CALL dbgFunc(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char *pLayerPrefix, const char * pMsg, void * pUserData) {
	std::string messageString;
	std::string pLayerPrefixString(pLayerPrefix);

	if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		messageString += "ERROR: [" + pLayerPrefixString + "] Code " + std::to_string(msgCode) + " : " + pMsg;
	}
	else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
		messageString += "WARNING: [" + pLayerPrefixString + "] Code " + std::to_string(msgCode) + " : " + pMsg;
	}
	/*else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
		messageString += "INFORMATION: [" + pLayerPrefixString + "] Code " + std::to_string(msgCode) + " : " + pMsg;
	}*/
	else {
		return false;
	}

	std::cout << messageString << std::endl;

	return false;
}

PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
vk::DebugReportCallbackEXT msg_callback;

int main() {

	std::cout << "Hallo!\n";

	// layers
	std::vector<vk::LayerProperties> properties;
	auto result = enumerateInstanceLayerProperties(properties);
	std::cout << "Layer Properties result: " << vkResultToString(result) << std::endl;
	if (result == vk::Result::eVkSuccess) {
		std::cout << "Listing Layer properties:\n\n";
		for (const auto & prop : properties) {
			std::cout << vkLayerInfoToString(prop) << std::endl;
		}
		std::cout << std::endl;
	}

	std::uint32_t s = 10;
	vk::ExtensionProperties * exProps = new vk::ExtensionProperties[s];
	const std::string nullStr("NULL");
	result = vk::enumerateInstanceExtensionProperties(nullptr, &s, exProps);
	std::cout << "Instance extensions properties result: " << vkResultToString(result) << std::endl;
	for (std::uint32_t k = 0; k < s; ++k) {
		std::cout << "Instance Extension: " << exProps[k].extensionName() << "\n";
	}

	/*
		Instance
	*/
	std::bitset<32> apiVersion;
	apiVersion.set(22);
	apiVersion.set(1);
	apiVersion.set(0);
	vk::ApplicationInfo appInfo("Test", 1, nullptr, 0, apiVersion.to_ulong());
	std::string debugLayerName("VK_LAYER_LUNARG_standard_validation");
	const auto debugLayerNamePtr = debugLayerName.c_str();
	std::string debugExtensionName("VK_EXT_debug_report");
	const auto debugExtensionNamePtr = debugExtensionName.c_str();
	vk::InstanceCreateInfo info(0, &appInfo, 1, &debugLayerNamePtr, 1, &debugExtensionNamePtr);
	info.pApplicationInfo(&appInfo);
	vk::Instance instance;
	result = vk::createInstance(&info, nullptr, &instance);
	std::cout << "Create Instance result: " << vkResultToString(result) << std::endl;

	/*
		Debugging
	*/
	CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (!CreateDebugReportCallback) {
		std::cout << "GetProcAddr: Unable to find vkCreateDebugReportCallbackEXT\n";
	}
	DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (!DestroyDebugReportCallback) {
		std::cout << "GetProcAddr: Unable to find vkDestroyDebugReportCallbackEXT\n";
	}
	VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
	dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	dbgCreateInfo.flags =
		VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
	dbgCreateInfo.pfnCallback = dbgFunc;
	dbgCreateInfo.pUserData = NULL;
	dbgCreateInfo.pNext = NULL;
	
	CreateDebugReportCallback(instance, &dbgCreateInfo, nullptr, &msg_callback);

	std::vector<vk::PhysicalDevice> physicalDevices;
	result = vk::enumeratePhysicalDevices(instance, physicalDevices);
	std::cout << "Enumerate Physical Devices result: " << vkResultToString(result) << std::endl;
	if (result == vk::Result::eVkSuccess) {
		std::cout << "Listing device properties:\n\n";
		for (const auto & physicalDevice : physicalDevices) {
			vk::PhysicalDeviceProperties properties;
			vk::getPhysicalDeviceProperties(physicalDevice, properties);
			std::cout << vKPhysicalDevicePropertiesToString(properties) << std::endl;
		}
		std::cout << std::endl;
	}

	if (physicalDevices.empty()) {
		std::cout << "No physical devices detected!\n";
		return EXIT_FAILURE;
	}

	unsigned int i = 0;
	unsigned int queueFamilyIndex = 0;
	bool foundSuitableDevice = false;
	for (; i < physicalDevices.size(); ++i) {
		const auto v = vk::getPhysicalDeviceQueueFamilyProperties(physicalDevices.at(i));
		queueFamilyIndex = 0;
		for (const auto & q : v) {
			if (q.queueFlags() & (vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eSparseBinding | vk::QueueFlagBits::eTransfer)) {
				foundSuitableDevice = true;
				break;
			}
			++queueFamilyIndex;
		}
		if (foundSuitableDevice)
			break;
	}

	if (!foundSuitableDevice) {
		std::cout << "No physical device has suitable queues!\n";
		return EXIT_FAILURE;
	}

	vk::PhysicalDevice physicalDevice = physicalDevices.at(i);

	std::vector<vk::LayerProperties> properties2;
	result = enumerateDeviceLayerProperties(physicalDevice, properties2);
	std::cout << "Device layer properties result: " << vkResultToString(result) << std::endl;
	for (const auto & l : properties2) {
		std::cout << "Device Layer: " << l.layerName() << "\n";
	}

	s = 10;
	result = vk::enumerateDeviceExtensionProperties(physicalDevice, nullptr, &s, exProps);
	std::cout << "Device extensions properties result: " << vkResultToString(result) << std::endl;
	for (std::uint32_t k = 0; k < s; ++k) {
		std::cout << "Device Extension: " << exProps[k].extensionName() << "\n";
	}

	/*
		Device
	*/
	const auto queuePriority = 1.f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo(0, queueFamilyIndex, 1, &queuePriority);
	std::string glslExtensionName("VK_NV_glsl_shader");
	const auto glslExtensionNamePtr = glslExtensionName.c_str();
	vk::PhysicalDeviceFeatures physicalDeviceFeatures;
	physicalDeviceFeatures.logicOp(true);
	physicalDeviceFeatures.shaderStorageBufferArrayDynamicIndexing(true);
	vk::DeviceCreateInfo deviceCreateInfo(0, 1, &deviceQueueCreateInfo, 1, &debugLayerNamePtr, 1, &glslExtensionNamePtr, &physicalDeviceFeatures);
	vk::Device device;
	result = vk::createDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
	std::cout << "Create device result: " << vkResultToString(result) << std::endl;

	vk::Queue queue;
	vk::getDeviceQueue(device, queueFamilyIndex, 0, queue);

	vk::CommandPoolCreateFlags cpcf;
	vk::CommandPoolCreateInfo commandPoolCreateInfo(cpcf, queueFamilyIndex);
	vk::CommandPool commandPool;
	result = vk::createCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
	std::cout << "Create command pool result: " << vkResultToString(result) << std::endl;

	std::vector<vk::CommandBuffer> commandBufferVec;
	commandBufferVec.resize(1);
	vk::CommandBufferAllocateInfo commandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, static_cast<std::uint32_t>(commandBufferVec.size()));
	result = vk::allocateCommandBuffers(device, commandBufferAllocateInfo, commandBufferVec);
	std::cout << "Allocate command buffer result: " << vkResultToString(result) << std::endl;
	vk::CommandBuffer cmdBuffer = commandBufferVec.front();

	vk::AttachmentDescriptionFlags adf;
	vk::AttachmentDescription attachmentDescription(adf, vk::Format::eR32G32B32A32Sfloat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::ImageLayout::eGeneral, vk::ImageLayout::eGeneral);
	vk::AttachmentReference attachmentRef(0, vk::ImageLayout::eGeneral);
	vk::SubpassDescription subPassDescription(0, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &attachmentRef, nullptr, nullptr, 0, nullptr);
	vk::RenderPassCreateInfo renderPassCreateInfo(0, 1, &attachmentDescription, 1, &subPassDescription, 0, nullptr);
	vk::RenderPass renderPass;
	result = vk::createRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);
	std::cout << "Create render pass result: " << vkResultToString(result) << std::endl;

	vk::ImageCreateFlags icf;
	vk::Extent3D extent(800, 800, 1);
	vk::ImageCreateInfo imageCreateInfo(icf, vk::ImageType::e2D, vk::Format::eR32G32B32A32Sfloat, extent, 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 1, &queueFamilyIndex, vk::ImageLayout::ePreinitialized);
	vk::Image image;
	result = vk::createImage(device, &imageCreateInfo, nullptr, &image);
	std::cout << "Create image result: " << vkResultToString(result) << std::endl;

	vk::MemoryRequirements memreq;
	vk::getImageMemoryRequirements(device, image, &memreq);
	//std::cout << "MemReq: size: " << memreq.size() << ", alignment: " << memreq.alignment() << "\n";

	vk::PhysicalDeviceMemoryProperties memprops;
	vk::getPhysicalDeviceMemoryProperties(physicalDevice, &memprops);
	unsigned int bestMemoryEntry = 0;
	for (; bestMemoryEntry < memprops.memoryTypeCount(); ++bestMemoryEntry) {
		//std::cout << "memory type [" << bestMemoryEntry << "]: flags: " << memprops.memoryTypes()[bestMemoryEntry].propertyFlags() << ", heap index: " << memprops.memoryTypes()[bestMemoryEntry].heapIndex() << "\n";
		if ((memprops.memoryTypes()[bestMemoryEntry].propertyFlags() & vk::MemoryPropertyFlagBits::eDeviceLocal))
			break;
	}
	//std::cout << "best memory index: " << bestMemoryEntry << "\n";
	
	vk::MemoryAllocateInfo allocateInfo(memreq.size(), bestMemoryEntry);
	vk::DeviceMemory memory;
	result = vk::allocateMemory(device, &allocateInfo, nullptr, &memory);
	std::cout << "Allocate memory result: " << vkResultToString(result) << std::endl;

	result = vk::bindImageMemory(device, image, memory, 0);
	std::cout << "Bind image memory result: " << vkResultToString(result) << std::endl;

	vk::ComponentMapping componentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA);
	vk::ImageSubresourceRange imageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
	vk::ImageViewCreateInfo imageViewCreateInfo(0, image, vk::ImageViewType::e2D, vk::Format::eR32G32B32A32Sfloat, componentMapping, imageSubresourceRange);
	vk::ImageView imgView;
	result = vk::createImageView(device, &imageViewCreateInfo, nullptr, &imgView);
	std::cout << "Create image view result: " << vkResultToString(result) << std::endl;

	vk::FramebufferCreateInfo framebufferCreateInfo(0, renderPass, 1, &imgView, 800, 800, 1);
	vk::Framebuffer framebuffer;
	result = vk::createFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer);
	std::cout << "Create framebuffer result: " << vkResultToString(result) << std::endl;

	/*
		Shaders
	*/
	char const vss[] =
		"#version 450 core\n"
		"layout(location = 0) in vec2 aVertex;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"out vec3 vColor;\n"
		"void main() {\n"
		"    vColor = aColor;\n"
		"    gl_Position = vec4(aVertex, 0, 1);\n"
		"}\n"
		;
	char const fss[] =
		"#version 450 core\n"
		"layout(location = 0) out vec4 outColor;\n"
		"in vec3 vColor;\n"
		"void main() {\n"
		"	outColor = vec4(vColor, 1);\n"
		"}\n"
		;
	vk::ShaderModuleCreateInfo vertexShaderModuleCreateInfo(0, sizeof(vss), reinterpret_cast<const std::uint32_t *>(vss));
	vk::ShaderModule vertexShader;
	result = vk::createShaderModule(device, &vertexShaderModuleCreateInfo, nullptr, &vertexShader);
	std::cout << "Create vertex shader module result: " << vkResultToString(result) << std::endl;
	vk::ShaderModuleCreateInfo fragmentShaderModuleCreateInfo(0, sizeof(fss), reinterpret_cast<const std::uint32_t *>(fss));
	vk::ShaderModule fragmentShader;
	result = vk::createShaderModule(device, &fragmentShaderModuleCreateInfo, nullptr, &fragmentShader);
	std::cout << "Create fragment shader module result: " << vkResultToString(result) << std::endl;


	/*
		Graphics Pipeline
	*/
	vk::PipelineCreateFlags pcf;
	std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
	pipelineShaderStageCreateInfos.emplace_back(0, vk::ShaderStageFlagBits::eVertex, vertexShader, "main", nullptr);
	pipelineShaderStageCreateInfos.emplace_back(0, vk::ShaderStageFlagBits::eFragment, fragmentShader, "main", nullptr);
	// Binding Description
	vk::VertexInputBindingDescription vertexBindingDescription(0, sizeof(float) * 6, vk::VertexInputRate::eVertex);
	// Attribute Description
	std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescriptions;
	vertexAttributeDescriptions.emplace_back(0, 0, vk::Format(vk::Format::eR32G32Sfloat), 0);
	vertexAttributeDescriptions.emplace_back(1, 0, vk::Format(vk::Format::eR32G32B32Sfloat), static_cast<std::uint32_t>(sizeof(float)) * 2);
	vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(0, 1, &vertexBindingDescription, static_cast<std::uint32_t>(vertexAttributeDescriptions.size()), vertexAttributeDescriptions.data());
	// Assembly
	vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(0, vk::PrimitiveTopology::eTriangleList, false);
	// Viewport
	vk::Viewport viewport(0.f, 0.f, 800.f, 800.f, 0.f, 1.f);
	vk::Rect2D scissor(vk::Offset2D(0, 0), vk::Extent2D(800, 800));
	vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(0, 1, &viewport, 1, &scissor);
	// Rasterization
	vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(0, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, false, 0.f, 0.f, 0.f, 0.1f);
	// Multisample
	vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(0, vk::SampleCountFlagBits::e1, false, 0, nullptr, false, false);
	// Colorblend
	vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(false, vk::BlendFactor::eOne, vk::BlendFactor::eOne, vk::BlendOp::eMax, vk::BlendFactor::eOne, vk::BlendFactor::eOne, vk::BlendOp::eMax, (vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB, vk::ColorComponentFlagBits::eA));
	std::array<float, 4> blendConstants{ 0.f, 0.f, 0.f, 0.f };
	vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(0, false, vk::LogicOp::eAnd, 1, &pipelineColorBlendAttachmentState, blendConstants);
	// Descriptor set
	std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings;
	descriptorSetLayoutBindings.emplace_back(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr);
	descriptorSetLayoutBindings.emplace_back(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr);
	vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(0, static_cast<std::uint32_t>(descriptorSetLayoutBindings.size()), descriptorSetLayoutBindings.data());
	vk::DescriptorSetLayout descriptorSetLayout;
	result = vk::createDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
	std::cout << "Create descriptor set layout result: " << vkResultToString(result) << std::endl;
	// Layout
	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo(0, 1, &descriptorSetLayout, 0, nullptr);
	vk::PipelineLayout pipelineLayout;
	result = vk::createPipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	std::cout << "Create pipeline layout result: " << vkResultToString(result) << std::endl;
	vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo(pcf, static_cast<std::uint32_t>(pipelineShaderStageCreateInfos.size()), pipelineShaderStageCreateInfos.data(), &pipelineVertexInputStateCreateInfo,
																&pipelineInputAssemblyStateCreateInfo, nullptr, &pipelineViewportStateCreateInfo, &pipelineRasterizationStateCreateInfo,
																&pipelineMultisampleStateCreateInfo, nullptr, &pipelineColorBlendStateCreateInfo, nullptr, pipelineLayout, renderPass, 0, 0, 0);
	vk::Pipeline pipeline;
	result = vk::createGraphicsPipelines(device, 0, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);
	std::cout << "Create graphics pipeline result: " << vkResultToString(result) << std::endl;

	vk::CommandBufferUsageFlags cbuf;
	vk::CommandBufferBeginInfo cmdBufferBeginInfo(cbuf, nullptr);
	result = vk::beginCommandBuffer(cmdBuffer, cmdBufferBeginInfo);
	std::cout << "Begin command buffer result: " << vkResultToString(result) << std::endl;
	
	const std::array<float, 4> col { 1.f, 0.f, 0.f, 1.f };
	vk::ClearColorValue val(col);
	vk::ClearValue clearVal(val);
	vk::RenderPassBeginInfo renderPassBeginInfo(renderPass, framebuffer, vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(800, 800)), 1, &clearVal);
	vk::cmdBeginRenderPass(cmdBuffer, renderPassBeginInfo, vk::SubpassContents::eInline);
	// TODO
	vk::cmdEndRenderPass(cmdBuffer);

	result = vk::endCommandBuffer(cmdBuffer);
	std::cout << "End command buffer result: " << vkResultToString(result) << std::endl;

	vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &cmdBuffer, 0, nullptr);
	result = vk::queueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	std::cout << "Queue submit result: " << vkResultToString(result) << std::endl;

	// wait for device to finish everything before destroying everything
	vk::deviceWaitIdle(device);

	vk::destroyFramebuffer(device, framebuffer, nullptr);
	vk::destroyImageView(device, imgView, nullptr);
	vk::freeMemory(device, memory, nullptr);
	vk::destroyImage(device, image, nullptr);
	vk::destroyRenderPass(device, renderPass, nullptr);

	vk::freeCommandBuffers(device, commandPool, commandBufferVec);
	vk::destroyCommandPool(device, commandPool, nullptr);

	DestroyDebugReportCallback(instance, msg_callback, nullptr);

	vk::destroyDevice(device, nullptr);
	vk::destroyInstance(instance, nullptr);

	return EXIT_SUCCESS;
}