/*===============================================================================
Copyright (c) 2024 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_LOGHANDLERCONFIG_H_
#define _VU_LOGHANDLERCONFIG_H_

/**
 * \file LogHandlerConfig.h
 * \brief Configuration to enable Apps to receive log events from Vuforia Engine.
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup EngineLogHandlerConfigGroup
 * \{
 */

/// \brief Vuforia Engine log level values
VU_ENUM(VuLogLevel)
{
    VU_LOG_LEVEL_ERROR = 0x0,   ///< Error log message. Logged in situations that caused an operation to fail or abort.
    VU_LOG_LEVEL_WARNING = 0x1, ///< Warning log message. Logged in situations where the operation continued
                                ///< despite an error or unexpected condition.
    VU_LOG_LEVEL_INFO = 0x2,    ///< Info log message. Information that might be useful to the user or developer but does
                                ///< not indicate any error or problem.
    VU_LOG_LEVEL_VERBOSE = 0x3  ///< Verbose log message. Used for very detailed information or very frequently logged information.
};

/// \brief A Vuforia Engine log event
/**
 * A log event contains information about Vuforia Engine log messages logged to the platform logging system.
 */
typedef struct VuLogEvent
{
    /// \brief Log level of the message.
    VuLogLevel logLevel;

    /// \brief The message string logged by Vuforia Engine
    /**
     * \brief The lifetime of the string is bound to the scope of the log callback.
     *
     * \brief String data is UTF-8 encoded.
     */
    const char* logMessage;
} VuLogEvent;


/// \brief Handler for receiving Engine log events
/**
 * \param logEvent The log event
 * \param clientData Custom data provided by the client.
 */
typedef void(VU_API_CALL VuLogHandler)(VuLogEvent logEvent, void* clientData);

/// \brief Data structure to configure the handling of Engine log messages
typedef struct VuLogHandlerConfig
{
    /// \brief Log handler function to report Engine log events
    /**
     * \note The parameter is ignored if set to NULL. In this case Engine does not have a way to notify
     * its client about log events. The default value is NULL.
     *
     * \note The client has to ensure that the handler function is valid for the life-time of the Engine instance.
     *
     * \note The log handler will be called on a dedicated Engine thread. The client must make sure to properly synchronize the thread.
     */
    VuLogHandler* logHandler;

    /// \brief Client data to pass back when the log handler function is called
    /**
     * \note Default value is NULL.
     */
    void* clientData;
} VuLogHandlerConfig;

/// \brief Default error handler configuration
/**
 * \note Use this function to initialize the VuLogHandlerConfig data structure with default values.
 */
VU_API VuLogHandlerConfig VU_API_CALL vuLogHandlerConfigDefault();

/// \brief Add log handler configuration to the engine configuration to handle log events from Engine.
/**
 * The registered callback handler function will be invoked for every message that is logged internally by Vuforia Engine.
 *
 * \note Messages might be truncated if they exceed the internal length limits.
 *
 * \note The handler function will be invoked on a dedicated internal Vuforia Engine thread. Clients are responsible for
 * synchronizing their callback implementation with other threads in the client code.
 *
 * \note Logging to the standard platform logging system will be done directly from the respective thread on which the
 * logging calls are invoked, e.g. logging for an API call on a client thread or logging from an internal Vuforia Engine thread.
 * As a consequence, there might be a small delay for the events delivered on the dedicated log callback thread compared
 * to the messages logged to the standard platform logging system.
 *
 * \note Clients should only do the minimum amount of work in the callback handler and return control as soon as
 * possible back to Vuforia Engine. Blocking the callback thread might lead to increased memory usage and to log messages
 * being dropped and not delivered to the callback handler.
 *
 * \note Clients should NOT do any reentrant calls to Vuforia Engine API functions from the callback handler. Calling Vuforia
 * APIs from the callback handler might lead to undefined behavior including crashes, instability and deadlocks. In particular,
 * these function calls might trigger additional log events which could potentially lead to an infinite cycle.
 *
 * \note See also the general Vuforia Engine API documentation on "Multi-threading and thread safety" as well as
 * "Callbacks and reentrancy".
 *
 * \note As the log messages contain largely the same information as is logged by Vuforia Engine to the platform logging system
 * the same considerations apply regarding any sensitive information contained in the logs.
 */
VU_API VuResult VU_API_CALL vuEngineConfigSetAddLogHandlerConfig(VuEngineConfigSet* configSet, const VuLogHandlerConfig* config);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_LOGHANDLERCONFIG_H_
