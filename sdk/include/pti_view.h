//==============================================================
// Copyright (C) Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================
#ifndef INCLUDE_PTI_VIEW_H_
#define INCLUDE_PTI_VIEW_H_

#include <stddef.h>
#include <stdint.h>

#include "pti_export.h"
#include <level_zero/layers/zel_tracing_api.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Return/Error codes
 */
typedef enum {
  PTI_SUCCESS = 0,                        //!< success
  PTI_STATUS_END_OF_BUFFER = 1,           //!< end of buffer reached, e.g., in ptiViewGetNextRecord
  PTI_ERROR_NOT_IMPLEMENTED = 2,          //!< functionality not implemented
  PTI_ERROR_BAD_ARGUMENT = 3,             //!< error code for invalid arguments
  PTI_ERROR_NO_CALLBACKS_SET = 4,         //!< error due to no callbacks set via ptiViewSetCallbacks
  PTI_ERROR_EXTERNAL_ID_QUEUE_EMPTY = 5,  //!< empty external ID-queue while working with
                                          //!< PTI_VIEW_EXTERNAL_CORRELATION

  PTI_ERROR_INTERNAL = 200                //!< internal error
} pti_result;

/**
 * @brief Kind of software and hardware operations to be tracked and viewed,
 * passed to ptiViewEnable/ptiViewDisable
 */
typedef enum _pti_view_kind {
  PTI_VIEW_INVALID = 0,
  PTI_VIEW_DEVICE_GPU_KERNEL = 1,         //!< Device kernels
  PTI_VIEW_DEVICE_CPU_KERNEL = 2,         //!< Host (CPU) kernels
  PTI_VIEW_LEVEL_ZERO_CALLS = 3,          //!< Level-Zero APIs tracing
  PTI_VIEW_OPENCL_CALLS = 4,              //!< OpenCL APIs tracing
  PTI_VIEW_COLLECTION_OVERHEAD = 5,       //!< Collection overhead
  PTI_VIEW_SYCL_RUNTIME_CALLS = 6,        //!< SYCL runtime API tracing
  PTI_VIEW_EXTERNAL_CORRELATION = 7,      //!< Correlation of external operations
  PTI_VIEW_DEVICE_GPU_MEM_COPY = 8,       //!< Memory copies between Host and Device
  PTI_VIEW_DEVICE_GPU_MEM_FILL = 9,       //!< Device memory fills
} pti_view_kind;

/**
 * @brief Memory type
 */
typedef enum _pti_view_memory_type {
  PTI_VIEW_MEMORY_TYPE_MEMORY = 0,  //!< Unknown memory type
  PTI_VIEW_MEMORY_TYPE_HOST = 1,    //!< Host memory
  PTI_VIEW_MEMORY_TYPE_DEVICE = 2,  //!< Device memory
  PTI_VIEW_MEMORY_TYPE_SHARED= 3,   //!< Shared memory
} pti_view_memory_type;

/**
 * @brief Memory copy type
 * where M=Memory(unknown), D=Device, H=Host
 */
typedef enum _pti_view_memcpy_type {
  PTI_VIEW_MEMCPY_TYPE_M2M = 0,
  PTI_VIEW_MEMCPY_TYPE_M2H = 1,
  PTI_VIEW_MEMCPY_TYPE_M2D = 2,
  PTI_VIEW_MEMCPY_TYPE_M2S = 3,

  PTI_VIEW_MEMCPY_TYPE_H2M = 4,
  PTI_VIEW_MEMCPY_TYPE_H2H = 5,
  PTI_VIEW_MEMCPY_TYPE_H2D = 6,
  PTI_VIEW_MEMCPY_TYPE_H2S = 7,

  PTI_VIEW_MEMCPY_TYPE_D2M = 8,
  PTI_VIEW_MEMCPY_TYPE_D2H = 9,
  PTI_VIEW_MEMCPY_TYPE_D2D = 10,
  PTI_VIEW_MEMCPY_TYPE_D2S = 11,

  PTI_VIEW_MEMCPY_TYPE_S2M = 12,
  PTI_VIEW_MEMCPY_TYPE_S2H = 13,
  PTI_VIEW_MEMCPY_TYPE_S2D = 14,
  PTI_VIEW_MEMCPY_TYPE_S2S = 15,
} pti_view_memcpy_type;

/**
 *  @brief External correlation kind
 */
typedef enum _pti_view_external_kind {
  PTI_VIEW_EXTERNAL_KIND_INVALID = 0,
  PTI_VIEW_EXTERNAL_KIND_UNKNOWN = 1,
  PTI_VIEW_EXTERNAL_KIND_CUSTOM_0 = 2,
  PTI_VIEW_EXTERNAL_KIND_CUSTOM_1 = 3,
  PTI_VIEW_EXTERNAL_KIND_CUSTOM_2 = 4,
  PTI_VIEW_EXTERNAL_KIND_CUSTOM_3 = 5,
} pti_view_external_kind;

/**
 *  @brief Collection Overhead kind
 */
typedef enum _pti_view_overhead_kind {
  PTI_VIEW_OVERHEAD_KIND_INVALID = 0,
  PTI_VIEW_OVERHEAD_KIND_UNKNOWN = 1,
  PTI_VIEW_OVERHEAD_KIND_RESOURCE = 2,
  PTI_VIEW_OVERHEAD_KIND_BUFFER_FLUSH = 3,
  PTI_VIEW_OVERHEAD_KIND_DRIVER = 4,
  PTI_VIEW_OVERHEAD_KIND_TIME = 5,
} pti_view_overhead_kind;

/**
 * @brief Base View record type
 */
typedef struct pti_view_record_base {
  pti_view_kind _view_kind;                   //!< Record View kind
} pti_view_record_base;

/**
 * @brief Device Compute kernel View record type
 */
typedef struct pti_view_record_kernel {
  pti_view_record_base _view_kind;            //!< Base record
  ze_command_queue_handle_t _queue_handle;    //!< Device back-end queue handle
  ze_device_handle_t  _device_handle;         //!< Device handle
  ze_context_handle_t _context_handle;        //!< Context handle
  const char* _name;                          //!< Kernel name
  const char* _source_file_name;              //!< Kernel source file,
                                              //!< null if no information
  uint64_t _source_line_number;               //!< Kernel beginning source line number,
                                              //!< 0 if no information
  uint64_t _kernel_id;                        //!< Kernel instance ID,
                                              //!< unique among all device kernel instances
  uint32_t _correlation_id;                   //!< ID that correlates this record with records
                                              //!< of other Views
  uint32_t _thread_id;                        //!< Thread ID of Function call
  char _pci_address[16];                      //!< Device pci_address
  uint64_t _append_timestamp;                 //!< Timestamp of kernel appending to
                                              //!< back-end command list, ns
  uint64_t _start_timestamp;                  //!< Timestamp of kernel start on device, ns
  uint64_t _end_timestamp;                    //!< Timestamp of kernel completion on device, ns
  uint64_t _submit_timestamp;                 //!< Timestamp of kernel command list submission
                                              //!< of device, ns
  uint64_t _sycl_task_begin_timestamp;        //!< Timestamp of kernel submission from SYCL layer,
                                              //!< ns
  uint64_t _sycl_enqk_begin_timestamp;
  uint64_t _sycl_node_id;
  uint32_t _sycl_invocation_id;
} pti_view_record_kernel;

/**
 * @brief SYCL runtime API View record type
 */
typedef struct pti_view_record_sycl_runtime{
  pti_view_record_base _view_kind;  //!< Base record
  const char* _name;                //!< SYCL runtime function name
  uint64_t _start_timestamp;        //!< Function enter timestamp, ns
  uint64_t _end_timestamp;          //!< Function exit timestamp, ns
  uint32_t _process_id;             //!< Process ID of function call
  uint32_t _thread_id;              //!< Thread ID of function call
  uint32_t _correlation_id;         //!< ID that correlates this record with records of other Views
} pti_view_record_sycl_runtime;

/**
 * @brief Memory Copy Operation View record type
 */
typedef struct pti_view_record_memory_copy {
  pti_view_record_base _view_kind;          //!< Base record
  pti_view_memcpy_type _memcpy_type;        //!< Memory copy type
  pti_view_memory_type _mem_src;            //!< Memory type
  pti_view_memory_type _mem_dst;            //!< Memory type
  ze_command_queue_handle_t _queue_handle;  //!< Device back-end queue handle
  ze_device_handle_t  _device_handle;       //!< Device handle
  ze_context_handle_t _context_handle;      //!< Context handle
  const char* _name;                        //!< Back-end API name making a memory copy
  char _pci_address[16];                    //!< Device pci_address
  uint64_t _mem_op_id;                      //!< Memory operation ID, unique among
                                            //!< all memory operations instances
  uint32_t _correlation_id;                 //!< ID that correlates this record with records
                                            //!< of other Views
  uint32_t _thread_id;                      //!< Thread ID from which operation submitted
  uint64_t _append_timestamp;               //!< Timestamp of memory copy appending to
                                            //!< back-end command list, ns
  uint64_t _start_timestamp;                //!< Timestamp of memory copy start on device, ns
  uint64_t _end_timestamp;                  //!< Timestamp of memory copy completion on device, ns
  uint64_t _submit_timestamp;               //!< Timestamp of memory copy command list submission
                                            //!< to device, ns
} pti_view_record_memory_copy;

/**
 * @brief Device Memory Fill operation View record type
 */
typedef struct pti_view_record_memory_fill {
  pti_view_record_base _view_kind;          //!< Base record
  pti_view_memory_type _mem_type;           //!< Type of memory filled
  ze_command_queue_handle_t _queue_handle;  //!< Device back-end queue handle
  ze_device_handle_t  _device_handle;       //!< Device handle
  ze_context_handle_t _context_handle;      //!< Context handle
  const char* _name;                        //!< Back-end API name making a memory fill
  char _pci_address[16];                    //!< Device pci_address
  uint64_t _mem_op_id;                      //!< Memory operation ID,
                                            //!< unique among all memory operations instances
  uint32_t _correlation_id;                 //!< ID provided by user, marking some external
                                            //!< to PTI operations
  uint32_t _thread_id;                      //!< Thread ID from which operation submitted
  uint64_t _append_timestamp;               //!< Timestamp of memory fill appending
                                            //!< to back-end command list, ns
  uint64_t _start_timestamp;                //!< Timestamp of memory fill start on device, ns
  uint64_t _end_timestamp;                  //!< Timestamp of memory fill completion on device, ns
  uint64_t _submit_timestamp;               //!< Timestamp of memory fill command list submission
                                            //!< to device, ns
  uint64_t _bytes;                          //!< number of bytes filled
  uint64_t _value_for_set;                  //!< value filled
} pti_view_record_memory_fill;

/**
 * @brief External Correlation View record type
 */
typedef struct pti_view_record_external_correlation {
  pti_view_record_base _view_kind;          //!< Base record
  uint32_t _correlation_id;                 //!< ID that correlates this record with records
                                            //!< of other Views
  uint64_t _external_id;                    //!< ID provided by user, marking an external
                                            //!< to PTI operation
  pti_view_external_kind _external_kind;
} pti_view_record_external_correlation;


/**
 * @brief Overhead View record type
 */
typedef struct pti_view_record_overhead {
  pti_view_record_base _view_kind;          //!< Base record
  uint64_t _overhead_start_timestamp_ns;    //!< Overhead observation start timestamp, ns
  uint64_t _overhead_end_timestamp_ns;      //!< Overhead observation end timestamp, ns
  uint64_t _overhead_thread_id;             //!< Thread ID of where the overhead observed
  uint64_t _overhead_count;                 //!< number of views in the overhead region
  uint64_t _overhead_duration_ns;           //!< Cumulative duration of the overhead over
                                            //!< the observation region, could be less than
                                            //!< interval between the observation region
                                            //!< start and the end
  pti_view_overhead_kind  _overhead_kind;   //!< Type of overhead
} pti_view_record_overhead;

typedef void (*pti_fptr_buffer_completed)(unsigned char* buffer,
                                             size_t buffer_size_in_bytes,
                                             size_t used_bytes);

typedef void (*pti_fptr_buffer_requested)(unsigned char** buffer_ptr,
                                             size_t* buffer_size_in_bytes);


/**
 * @brief Sets callback to user buffer managemnet functions, which implemeneted
 * by a user
 *
 * @return pti_result
 */
pti_result PTI_EXPORT
ptiViewSetCallbacks(pti_fptr_buffer_requested fptr_bufferRequested,
                       pti_fptr_buffer_completed fptr_bufferCompleted);

/**
 * @brief Enables View of specific group of operations
 *
 * @param view_kind
 * @return pti_result
 */
pti_result PTI_EXPORT ptiViewEnable(pti_view_kind view_kind);

/**
 * @brief Disables View of specific group of operations
 *
 * @param view_kind
 * @return pti_result
 */
pti_result PTI_EXPORT ptiViewDisable(pti_view_kind view_kind);

/**
 * @brief Flushes all view records by calling bufferCompleted callback
 *
 * @return pti_result
 */
pti_result PTI_EXPORT ptiFlushAllViews();

/**
 * @brief Gets next view record in buffer.
 *
 * @param buffer the buffer initially provided by pti_fptr_buffer_requested
 * user function and now passd to pti_fptr_buffer_completed
 * @param valid_bytes size of portion of the buffer filled with view records
 * @param record current view record
 * @return pti_result
 */
pti_result PTI_EXPORT
ptiViewGetNextRecord(uint8_t* buffer, size_t valid_bytes,
                       pti_view_record_base** record);

/**
 * @brief Pushes ExternelCorrelationId kind and id for generation of external correlation records
 *
 * @return pti_result
 */
pti_result PTI_EXPORT
ptiViewPushExternalCorrelationId(pti_view_external_kind external_kind, uint64_t external_id);

/**
 * @brief Pops ExternelCorrelationId kind and id for generation of external correlation records
 *
 * @return pti_result
 */
pti_result PTI_EXPORT
ptiViewPopExternalCorrelationId(pti_view_external_kind external_kind, uint64_t* p_external_id);


#if defined(__cplusplus)
}
#endif

#endif  // INCLUDE_PTI_VIEW_H_
