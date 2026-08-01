#pragma once

/**
 * @file HostServices.h
 * @brief Application-supplied callbacks used by reference features.
 *
 * Implement this interface in the host (firmware, simulator, desktop).
 * Features must not call platform SDKs directly.
 */
#include <stdint.h>

namespace mcrpc {

struct GpsFix {
  bool valid;
  float lat;
  float lon;
  float alt;
  int sats;
  float hdop;
};

struct HostServices {
  virtual ~HostServices() {}

  virtual const char* nodeName() = 0;
  virtual const char* profile() = 0;
  virtual const char* firmwareVersion() = 0;
  virtual uint32_t uptimeSeconds() = 0;
  virtual int rssi() { return 0; }

  virtual bool readBattery(float& volts, int& percent) {
    (void)volts;
    (void)percent;
    return false;
  }
  virtual bool isCharging() { return false; }

  virtual bool readGps(GpsFix& fix) {
    (void)fix;
    return false;
  }
  /** Request an on-demand GPS acquisition (may be async). */
  virtual bool requestGpsFix() { return false; }
  virtual bool gpsBusy() { return false; }

  virtual bool readButtonPressed() { return false; }
  virtual bool setRelay(bool on) {
    (void)on;
    return false;
  }
  virtual bool getRelay() { return false; }

  virtual bool displayText(const char* text) {
    (void)text;
    return false;
  }
  virtual bool displayClear() { return false; }

  /** Optional back-pointer set by the host for event publishing. */
  class McRpc* engine = nullptr;
};

}  // namespace mcrpc
