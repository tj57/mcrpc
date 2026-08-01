#pragma once

/**
 * @file mcrpc.h
 * @brief Stable public C++ API for mcRPC (transport-independent).
 *
 * Include this header from applications, bindings, and tools.
 * Feature firmware modules may include FeatureSdk.h instead.
 *
 * API stability (1.x): breaking changes require a major version bump and
 * CHANGELOG entry. See docs/API.md.
 */

#include <mcrpc/Version.h>
#include <mcrpc/McRpcTypes.h>
#include <mcrpc/Parser.h>
#include <mcrpc/InboundMessage.h>
#include <mcrpc/OutboundBuilder.h>
#include <mcrpc/StatusBuilder.h>
#include <mcrpc/DiscoverBuilder.h>
#include <mcrpc/CommandRegistry.h>
#include <mcrpc/CapabilityRegistry.h>
#include <mcrpc/EventBus.h>
#include <mcrpc/Dispatcher.h>
#include <mcrpc/Config.h>
#include <mcrpc/McRpc.h>
#include <mcrpc/FeatureSdk.h>
