#include <mcrpc/features/button/ButtonFeature.h>

#include <stdio.h>

namespace mcrpc {

static ButtonFeature* g_btn = nullptr;

void ButtonFeature::registerCommands(CommandRegistry& commands) {
  g_btn = this;
  commands.registerCommand("button", &ButtonFeature::cmdButton, "button info");
  commands.registerCommand("button_state", &ButtonFeature::cmdButtonState, "pressed?");
}

void ButtonFeature::registerCapabilities(CapabilityRegistry& caps) {
  caps.registerCapability("button");
}

void ButtonFeature::contributeStatus(StatusBuilder& status) {
  status.add("button_count", (unsigned long)_press_count);
}

void ButtonFeature::contributeDiscover(DiscoverBuilder& discover) {
  // RFC-0002: keep discovery slim — capability appears in caps= only.
  (void)discover;
}

void ButtonFeature::notifyPressed(uint8_t btn_id) {
  _pressed = true;
  _press_count++;
  char kv[48];
  if (btn_id <= 1) {
    snprintf(kv, sizeof(kv), "count=%lu", (unsigned long)_press_count);
  } else {
    snprintf(kv, sizeof(kv), "id=%u count=%lu", (unsigned)btn_id, (unsigned long)_press_count);
  }
  publishEvent("button.pressed", kv);
  _pressed = false;
}

void ButtonFeature::notifyDown(uint8_t btn_id) {
  _pressed = true;
  char kv[24];
  if (btn_id <= 1) {
    publishEvent("button.down", nullptr);
  } else {
    snprintf(kv, sizeof(kv), "id=%u", (unsigned)btn_id);
    publishEvent("button.down", kv);
  }
}

void ButtonFeature::notifyUp(uint8_t btn_id) {
  _pressed = false;
  _press_count++;
  char kv[48];
  if (btn_id <= 1) {
    snprintf(kv, sizeof(kv), "count=%lu", (unsigned long)_press_count);
  } else {
    snprintf(kv, sizeof(kv), "id=%u count=%lu", (unsigned)btn_id, (unsigned long)_press_count);
  }
  publishEvent("button.up", kv);
}

bool ButtonFeature::cmdButton(CommandContext& ctx) {
  if (!g_btn) return false;
  ctx.reply->clear();
  ctx.reply->printf("button count=%lu", (unsigned long)g_btn->_press_count);
  return true;
}

bool ButtonFeature::cmdButtonState(CommandContext& ctx) {
  if (!g_btn) return false;
  ctx.reply->clear();
  ctx.reply->printf("button_state value=%s", g_btn->_host.readButtonPressed() ? "1" : "0");
  return true;
}

}  // namespace mcrpc
