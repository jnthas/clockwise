#pragma once

#include <stdint.h>

namespace cw
{

  /**
   * Pure C++ (no Arduino dependency) decision logic for Wi-Fi provisioning.
   *
   * It is pumped from loop() and never blocks, so Improv Serial keeps being
   * serviced in every state. This is what allows ESP-Web-Tools to detect the
   * device and show the "Configure Wi-Fi" step right after flashing.
   *
   *   begin(hasCreds) ── yes ──> Connecting ──timeout──> ImprovWait
   *          └──────── no ─────────────────────────────> ImprovWait
   *   ImprovWait ──grace/session expired──> ApPortal ──timeout──> Restarting
   *   Connecting | ImprovWait | ApPortal ──wifi connected──> Connected
   *
   * The caller owns all side effects (WiFi, WiFiManager, display, NVS) and
   * performs them when the returned action is not None.
   */
  enum class WiFiSetupState : uint8_t
  {
    Idle,
    Connecting,  // trying the credentials stored in NVS
    ImprovWait,  // listening for Improv Serial provisioning (web installer)
    ApPortal,    // WiFiManager captive portal running (non-blocking)
    Connected,   // network is up; terminal state for this state machine
    Restarting,  // portal timed out; device should restart
  };

  enum class WiFiSetupAction : uint8_t
  {
    None,
    StartStationConnect, // WiFi.begin() with stored credentials
    EnterImprovWait,     // stop station attempts, wait for Improv
    StartApPortal,       // start WiFiManager portal in non-blocking mode
    Connected,           // emitted exactly once: run network-ready setup
    Restart,             // emitted exactly once: restart the device
  };

  class WiFiSetupStateMachine
  {
  public:
    struct Config
    {
      // Same budget as Improv's tryConnectToWifi (20 x 500 ms).
      uint32_t staConnectTimeoutMs = 10000;
      // Minimum time to listen for Improv before starting the AP portal.
      // ESP-Web-Tools waits ~10 s after reset for an Improv answer.
      uint32_t improvGraceMs = 15000;
      // Once a browser talks Improv, keep waiting this long after the last
      // received byte so the user can choose an SSID and type the password.
      uint32_t improvSessionMs = 120000;
      // How long the AP portal stays up before the device restarts.
      uint32_t portalTimeoutMs = 300000;
    };

    WiFiSetupStateMachine() {}
    explicit WiFiSetupStateMachine(const Config &config) : config_(config) {}

    WiFiSetupAction begin(bool hasStoredCredentials, uint32_t nowMs)
    {
      improvActive_ = false;
      if (hasStoredCredentials)
      {
        enter(WiFiSetupState::Connecting, nowMs);
        return WiFiSetupAction::StartStationConnect;
      }
      enter(WiFiSetupState::ImprovWait, nowMs);
      return WiFiSetupAction::EnterImprovWait;
    }

    /**
     * @param nowMs          current millis(); wraparound safe
     * @param wifiConnected  station is associated and has an IP
     * @param improvActivity bytes were received on the Improv serial port
     *                       since the previous call
     */
    WiFiSetupAction update(uint32_t nowMs, bool wifiConnected, bool improvActivity)
    {
      if (improvActivity)
      {
        improvActive_ = true;
        lastImprovActivityMs_ = nowMs;
      }

      switch (state_)
      {
      case WiFiSetupState::Connecting:
        if (wifiConnected)
          return connected(nowMs);
        if (elapsed(nowMs, stateEnteredMs_) >= config_.staConnectTimeoutMs)
        {
          enter(WiFiSetupState::ImprovWait, nowMs);
          return WiFiSetupAction::EnterImprovWait;
        }
        return WiFiSetupAction::None;

      case WiFiSetupState::ImprovWait:
        if (wifiConnected)
          return connected(nowMs);
        if (improvWindowExpired(nowMs))
        {
          enter(WiFiSetupState::ApPortal, nowMs);
          return WiFiSetupAction::StartApPortal;
        }
        return WiFiSetupAction::None;

      case WiFiSetupState::ApPortal:
        if (wifiConnected)
          return connected(nowMs);
        if (elapsed(nowMs, stateEnteredMs_) >= config_.portalTimeoutMs)
        {
          enter(WiFiSetupState::Restarting, nowMs);
          return WiFiSetupAction::Restart;
        }
        return WiFiSetupAction::None;

      case WiFiSetupState::Idle:
      case WiFiSetupState::Connected:
      case WiFiSetupState::Restarting:
      default:
        return WiFiSetupAction::None;
      }
    }

    WiFiSetupState state() const { return state_; }
    bool isSettingUp() const
    {
      return state_ == WiFiSetupState::Connecting ||
             state_ == WiFiSetupState::ImprovWait ||
             state_ == WiFiSetupState::ApPortal;
    }

  private:
    // Unsigned subtraction keeps working across the millis() overflow.
    static uint32_t elapsed(uint32_t now, uint32_t since) { return now - since; }

    void enter(WiFiSetupState next, uint32_t nowMs)
    {
      state_ = next;
      stateEnteredMs_ = nowMs;
    }

    WiFiSetupAction connected(uint32_t nowMs)
    {
      enter(WiFiSetupState::Connected, nowMs);
      return WiFiSetupAction::Connected;
    }

    bool improvWindowExpired(uint32_t nowMs) const
    {
      if (elapsed(nowMs, stateEnteredMs_) < config_.improvGraceMs)
        return false;
      if (improvActive_ && elapsed(nowMs, lastImprovActivityMs_) < config_.improvSessionMs)
        return false;
      return true;
    }

    Config config_;
    WiFiSetupState state_ = WiFiSetupState::Idle;
    uint32_t stateEnteredMs_ = 0;
    uint32_t lastImprovActivityMs_ = 0;
    bool improvActive_ = false;
  };

} // namespace cw
