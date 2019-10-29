#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace vuh {
	class Instance;

	/// Logical device packed with associated command pools and buffers.
	/// Holds the pool(s) for transfer and compute operations as well as command
	/// buffers for sync operations.
	/// When the copy of the Device object is made it recreates all underlying
	/// structures access to which needs to be synchrnized, while still referring
	/// to the same physical device. Such that copying the Device object might be
	/// a convenient (although somewhat resource consuming) way to use device from
	/// different threads.
	class Device: public VULKAN_HPP_NAMESPACE::Device {
	public:
		explicit Device(const vuh::Instance& instance, const VULKAN_HPP_NAMESPACE::PhysicalDevice& phys_dev);
		~Device() noexcept;

		Device(const Device&);
		auto operator=(Device)-> Device&;
		Device(Device&&) noexcept;
		auto operator=(Device&&) noexcept-> Device&;
		friend auto swap(Device& d1, Device& d2)-> void;

		auto properties() const-> VULKAN_HPP_NAMESPACE::PhysicalDeviceProperties;
		auto numComputeQueues() const-> uint32_t { return 1u;}
		auto numTransferQueues() const-> uint32_t { return 1u;}
		auto memoryProperties(uint32_t id) const-> VULKAN_HPP_NAMESPACE::MemoryPropertyFlags;
		auto selectMemory(const VULKAN_HPP_NAMESPACE::Buffer& buffer, VULKAN_HPP_NAMESPACE::MemoryPropertyFlags properties) const-> uint32_t;
		auto selectMemory(const VULKAN_HPP_NAMESPACE::Image& image, VULKAN_HPP_NAMESPACE::MemoryPropertyFlags properties) const-> uint32_t;
		auto instance() const-> const vuh::Instance& {return _instance;}
		auto hasSeparateQueues() const-> bool;

		auto computeQueue(uint32_t i = 0)-> VULKAN_HPP_NAMESPACE::Queue;
		auto transferQueue(uint32_t i = 0)-> VULKAN_HPP_NAMESPACE::Queue;
		auto alloc(VULKAN_HPP_NAMESPACE::Buffer buf, uint32_t memory_id, VULKAN_HPP_NAMESPACE::Result& result)-> VULKAN_HPP_NAMESPACE::DeviceMemory;
		auto computeCmdPool()-> VULKAN_HPP_NAMESPACE::CommandPool {return _cmdpool_compute;}
		auto computeCmdBuffer()-> VULKAN_HPP_NAMESPACE::CommandBuffer& {return _cmdbuf_compute;}
		auto transferCmdPool()-> VULKAN_HPP_NAMESPACE::CommandPool;
		auto transferCmdBuffer()-> VULKAN_HPP_NAMESPACE::CommandBuffer&;
		auto createPipeline(VULKAN_HPP_NAMESPACE::PipelineLayout pipe_layout
		                    , VULKAN_HPP_NAMESPACE::PipelineCache pipe_cache
		                    , const VULKAN_HPP_NAMESPACE::PipelineShaderStageCreateInfo& shader_stage_info
							, VULKAN_HPP_NAMESPACE::Result& result
		                    , VULKAN_HPP_NAMESPACE::PipelineCreateFlags flags={}
		                    )-> VULKAN_HPP_NAMESPACE::Pipeline;
		auto instance()-> const vuh::Instance& { return _instance; }
		auto releaseComputeCmdBuffer(VULKAN_HPP_NAMESPACE::Result& result)-> VULKAN_HPP_NAMESPACE::CommandBuffer;

		// if fenceFd is support, we can use epoll or select wait for fence complete
		auto supportFenceFd()-> bool;
		
	private: // helpers
		explicit Device(const vuh::Instance& instance, const VULKAN_HPP_NAMESPACE::PhysicalDevice& phy_dev
		                , const std::vector<VULKAN_HPP_NAMESPACE::QueueFamilyProperties>& families);
		explicit Device(const vuh::Instance& instance, const VULKAN_HPP_NAMESPACE::PhysicalDevice& phy_dev
	                   , uint32_t computeFamilyId, uint32_t transferFamilyId);
		auto release() noexcept-> void;

		auto fenceFdSupported() noexcept-> bool;
	private: // data
		const vuh::Instance&     			_instance;           ///< refer to Instance object used to create device
        VULKAN_HPP_NAMESPACE::PhysicalDevice _phy_dev;            ///< handle to associated physical device
        VULKAN_HPP_NAMESPACE::CommandPool    _cmdpool_compute;    ///< handle to command pool for compute commands
        VULKAN_HPP_NAMESPACE::CommandBuffer  _cmdbuf_compute;     ///< primary command buffer associated with the compute command pool
        VULKAN_HPP_NAMESPACE::CommandPool    _cmdpool_transfer;   ///< handle to command pool for transfer instructions. Initialized on first trasnfer request.
        VULKAN_HPP_NAMESPACE::CommandBuffer  _cmdbuf_transfer;    ///< primary command buffer associated with transfer command pool. Initialized on first transfer request.
		uint32_t _cmp_family_id = uint32_t(-1); ///< compute queue family id. -1 if device does not have compute-capable queues.
		uint32_t _tfr_family_id = uint32_t(-1); ///< transfer queue family id, maybe the same as compute queue id.
        VULKAN_HPP_NAMESPACE::Result	_result;					///< result of vulkan's api
		bool		_support_fence_fd;			///< if fenceFd is support, we can use epoll or select wait for fence complete
	}; // class Device
}
