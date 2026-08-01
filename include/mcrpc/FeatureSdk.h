#pragma once

/**
 * @file FeatureSdk.h
 * @brief Stable Feature SDK surface for embedded/desktop feature modules.
 *
 * Prefer this over including individual headers when writing features.
 * Application hosts and language bindings should include <mcrpc/mcrpc.h>.
 *
 * Do not include transport or board SDKs from feature code.
 */

#include <mcrpc/Version.h>
#include <mcrpc/Feature.h>
#include <mcrpc/FeatureManager.h>
#include <mcrpc/CommandRegistry.h>
#include <mcrpc/CapabilityRegistry.h>
#include <mcrpc/EventBus.h>
#include <mcrpc/StatusBuilder.h>
#include <mcrpc/DiscoverBuilder.h>
#include <mcrpc/OutboundBuilder.h>
#include <mcrpc/HostServices.h>
#include <mcrpc/InboundMessage.h>
#include <mcrpc/Parser.h>
