// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright(c) 2021 Intel Corporation. All rights reserved.
/*! \file system_service.h */


#ifndef _ADSP_SYSTEM_SERVICE_H_
#define _ADSP_SYSTEM_SERVICE_H_

#include "core/kernel/logger/log.h"
#include "adsp_stddef.h"
#include "adsp_error_code.h"

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


struct AdspLogHandle;
typedef struct AdspLogHandle AdspLogHandle;

/*! \brief This struct defines the obfuscating type for notifications. */
typedef struct _AdspNotificationHandle
{} *AdspNotificationHandle;

/*! \brief Defines parameters used by ADSP system during notification creation. */
typedef struct _NotificationParams
{
   uint32_t type;              /*!< Notification type */
   uint16_t user_val_1;        /*!< 16 bits user value available directly in IPC header for some notifications */
   uint32_t user_val_2;        /*!< 30 bits user value available directly in IPC header for some notifications */
   uint32_t max_payload_size;  /*!< Data size of payload (NotificationCreate updates this value to max possible payload size) */
   uint8_t* payload;           /*!< Pointer on the payload */
} NotificationParams;

/*! \brief Defines parameters used by ADSP system during Module Event notification creation. */
typedef struct _ModuleEventNotification
{
    uint32_t module_instance_id; ///< Module ID (MS word) + Module Instance ID (LS word)
    uint32_t event_id;           ///< Module specific event ID.
    uint32_t event_data_size;    ///< Size of event_data array in bytes. May be set to 0 in case there is no data.
    uint32_t event_data[0];      ///< Optional event data (size set to 0 as it is optional data)
}ModuleEventNotification;

/*! \brief Defines notification targets supported by ADSP system */
typedef enum _NotificationTarget
{
    DSP_TO_HOST = 1,  /*!< Notification target is HOST */
    DSP_TO_ISH = 2    /*!< Notification target is ISH */
} NotificationTarget;

/*! \brief Defines size of notification header used by ADSP system */
#define OUTMESSAGE_HEADER_SIZE              28
#define NOTIFICATION_HEADER_SIZE            48
#define IXC_MAX_CLIENT_PAYLOAD_SIZE         256
#define NOTIFICATION_CLIENT_HEADER          sizeof(ModuleEventNotification)

/*! \brief Defines size of notifications used by ADSP system (including 28 bytes for notification header)*/
#define NOTIFICATION_BUFFER_SIZE            (IXC_MAX_CLIENT_PAYLOAD_SIZE + NOTIFICATION_HEADER_SIZE + NOTIFICATION_CLIENT_HEADER)


/*! \brief Defines notification types supported by ADSP system */
typedef enum _NotificationType
{
    VOICE_COMMAND_NOTIFICATION = 4, /*!< intel_adsp define corresponding to PHRASE_DETECTED notification */
    AUDIO_CLASSIFIER_RESULTS = 9,   /*!< intel_adsp define corresponding to FW_AUD_CLASS_RESULT notification */
    MODULE_EVENT_NOTIFICATION = 12,  /*!< intel_adsp define corresponding to MODULE_NOTIFICATION notification */
} NotificationType;

/*! \brief Defines size of HungryRTSink used by ADSP system */
#define HUNGRY_RT_SINK_SIZE            64

/*! \brief Defines prototype of the "LogMessage" function
 *
 * \param log_priority define the log priority for the message to be sent.
 *        The ADSP System may have been configured by the host to filter log message below a given priority.
 * \param log_entry provides information on log sender and log point location.
 * \param log_handle \copybrief AdspLogHandle
 * \param param1 some uint32_t value to include in the log message.
 * \param param2 some uint32_t value to include in the log message.
 * \param param3 some uint32_t value to include in the log message.
 * \param param4 some uint32_t value to include in the log message.
 *
 * \see LOG_MESSAGE
 */
typedef void (*SystemServiceLogMessageFct) (
    AdspLogPriority log_priority, uint32_t log_entry, AdspLogHandle const* log_handle, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4);

/*! \brief Defines prototype of the "SafeMemcpy" function
 *
 * \param dst define the address of destination buffer
 * \param maxlen define the size of destination buffer
 * \param src define the address of source buffer
 * \param len define the number of bytes
 * \return zero if success, error code otherwise
 */
typedef AdspErrorCode (*SystemServiceSafeMemcpyFct) (
    void *RESTRICT dst, size_t maxlen, const void *RESTRICT src, size_t len);

/*! \brief Defines prototype of the "SafeMemmove" function
 *
 * \param dst define the address of destination buffer
 * \param maxlen define the size of destination buffer
 * \param src define the address of source buffer
 * \param len define the number of bytes
 * \return zero if success, error code otherwise
 */
typedef AdspErrorCode (*SystemServiceSafeMemmoveFct) (
    void* dst, size_t maxlen, const void* src, size_t len);

/*! \brief Defines prototype of the "VecMemset" function
 *
 * \param dst define the address of destination buffer
 * \param c define the fill byte
 * \param len define the number of bytes
 * \return pointer to dst
 */
typedef void* (*SystemServiceVecMemsetFct) (
    void *dst, int c, size_t len);

/*! \brief Defines prototype of the "NotificationCreate" function
 *
 * \param params pointer on NotificationParams input structure
 * \param notification_buffer pointer on the notification buffer declared in module
 * \param notification_buffer_size size of the notification buffer declared in module
 * \param handle pointer on AdspNotificationHandle structure
 * \return error if notification_buffer is too small or NULL
 */
typedef AdspErrorCode (*SystemServiceCreateNotificationFct) (
                NotificationParams *params, uint8_t* notification_buffer, uint32_t notification_buffer_size, AdspNotificationHandle* handle);

/*! \brief Defines prototype of the "NotificationSend" function
 *
 * \param notification_target notification target is HOST or ISH
 * \param message the AdspNotificationHandle structure used for notification
 * \param actual_payload_size size of the notification data (excluding notification header)
 * \return error if invalid target
 */
typedef AdspErrorCode (*SystemServiceSendNotificationMessageFct) (
                NotificationTarget notification_target ,AdspNotificationHandle message, uint32_t actual_payload_size);

/*! \brief The AdspSystemService is actually a set of C-function pointers gathered in a C-struct
 * which brings some basic functionalities to module in runtime.
 *
 * The system service can be retrieved with help of either the intel_adsp::ProcessingModuleFactory::GetSystemService() method
 * or the intel_adsp::ProcessingModule::GetSysstemService() method.
 */
typedef struct AdspSystemService
{
    /*! The SystemService::LogMessage function provides capability to send some log message to the host for debugging purposes.
     * This log can be catched by the FDK Tools and displayed in the Debug window.
     * The prototype of this function is given by the \ref SystemServiceLogMessageFct typedef.
     *
     * \remarks This service function is not expected to be called directly by the user code.
     * Instead, the \ref LOG_MESSAGE should be invoked for this purpose.
     * \see LOG_MESSAGE
     */
    const SystemServiceLogMessageFct LogMessage;

    /*! The SystemService::SafeMemcpy function provides capability to use SafeMemcpy function provided by ADSP System.
     * The prototype of this function is given by the \ref SystemServiceSafeMemcpyFct typedef.
     */
    const SystemServiceSafeMemcpyFct SafeMemcpy;

    /*! The SystemService::SafeMemmove function provides capability to use SafeMemmove function provided by ADSP System.
     * The prototype of this function is given by the \ref SystemServiceSafeMemmoveFct typedef.
     */
    const SystemServiceSafeMemmoveFct SafeMemmove;

    /*! The SystemService::VecMemset function provides capability to use VecMemset function provided by ADSP System.
     * The prototype of this function is given by the \ref SystemServiceVecMemsetFct typedef.
     */
    const SystemServiceVecMemsetFct VecMemset;

    /*! The SystemService::NotificationCreate function provides capability to use NotificationCreate function provided by ADSP System.
     * The prototype of this function is given by the \ref SystemServiceCreateNotificationFct typedef.
     *
     * \remarks This service function is not expected to be called directly by the user code.
     * Instead, the \ref intel_adsp::ModuleNotificationMessage::GetNotification() should be invoked for this purpose.
     *
     * \see intel_adsp::ModuleNotificationMessage
     * \see intel_adsp::ModuleNotificationMessage::GetNotification()
     */
    const SystemServiceCreateNotificationFct NotificationCreate;

    /*! The SystemService::NotificationSend function provides capability to use NotificationSend function provided by ADSP System.
     * The prototype of this function is given by the \ref SystemServiceSendNotificationMessageFct typedef.
     *
     * \remarks This service function is not expected to be called directly by the user code.
     * Instead, the \ref intel_adsp::NotificationMessage::Send() should be invoked for this purpose.
     *
     * \see intel_adsp::NotificationMessage
     * \see intel_adsp::NotificationMessage::Send()
     */
    const SystemServiceSendNotificationMessageFct NotificationSend;

} AdspSystemService;


#ifdef __cplusplus
namespace intel_adsp
{
/*! \brief Alias type of AdspSystemService which can be used in C++.
 */
struct SystemService : public AdspSystemService {};
}
#endif


#ifdef __cplusplus
}
#endif


#endif // _ADSP_SYSTEM_SERVICE_H_

/*!

 \page systemservice_page System Service
 \tableofcontents

   The ADSP System provides some system service at each module creation.
 \copydoc AdspSystemService
 \copybrief AdspSystemService

 \section LogMessage
 \copydoc AdspSystemService::LogMessage

 \section SafeMemcpy
 \copydoc AdspSystemService::SafeMemcpy

 \section SafeMemmove
 \copydoc AdspSystemService::SafeMemmove

 \section VecMemset
 \copydoc AdspSystemService::VecMemset

 \section NotificationCreate
 \copydoc AdspSystemService::NotificationCreate

 \section NotificationSend
 \copydoc AdspSystemService::NotificationSend
*/
