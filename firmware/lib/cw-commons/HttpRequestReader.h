#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <ctype.h>

#ifndef MCP_MAX_BODY
#define MCP_MAX_BODY 2048
#endif

// Incremental HTTP framing. A poll consumes at most byteBudget bytes and never
// waits for the network, so idle keep-alive sockets cannot stop the clock loop.
class HttpRequestReader
{
public:
  enum Result { Waiting, Complete, TooLarge, Invalid, TimedOut };

  void reset(uint32_t now, bool keepAlive = false)
  {
    request_.clear();
    headerEnd_ = 0;
    contentLength_ = 0;
    startedAt_ = now;
    keepAlive_ = keepAlive;
  }

  template <typename Client>
  Result poll(Client& client, uint32_t now, size_t byteBudget = 256)
  {
    const uint32_t timeout = request_.empty() && keepAlive_ ? 15000U : 1500U;
    if ((uint32_t)(now - startedAt_) >= timeout)
      return TimedOut;

    while (byteBudget-- > 0 && client.available() > 0)
    {
      const int next = client.read();
      if (next < 0)
        break;
      if (request_.empty())
        startedAt_ = now;
      request_ += (char)next;
      if (request_.size() > 4096)
        return TooLarge;
      if (headerEnd_ == 0 && request_.size() >= 4 &&
          request_.compare(request_.size() - 4, 4, "\r\n\r\n") == 0)
      {
        headerEnd_ = request_.size();
        const Result parsed = parseHeaders();
        if (parsed != Waiting)
          return parsed;
      }
      if (headerEnd_ && request_.size() - headerEnd_ == contentLength_)
        return Complete;
    }
    return Waiting;
  }

  const std::string& request() const { return request_; }
  size_t headerEnd() const { return headerEnd_; }
  size_t contentLength() const { return contentLength_; }

private:
  std::string request_;
  size_t headerEnd_ = 0;
  size_t contentLength_ = 0;
  uint32_t startedAt_ = 0;
  bool keepAlive_ = false;

  Result parseHeaders()
  {
    bool hasLength = false;
    size_t start = request_.find("\r\n") + 2;
    while (start + 2 < headerEnd_)
    {
      const size_t end = request_.find("\r\n", start);
      const size_t colon = request_.find(':', start);
      if (end == std::string::npos || colon == std::string::npos || colon >= end)
        return Invalid;
      std::string key = request_.substr(start, colon - start);
      for (size_t i = 0; i < key.size(); ++i)
        key[i] = (char)tolower((unsigned char)key[i]);
      size_t value = colon + 1;
      while (value < end && (request_[value] == ' ' || request_[value] == '\t'))
        ++value;
      size_t valueEnd = end;
      while (valueEnd > value && (request_[valueEnd - 1] == ' ' || request_[valueEnd - 1] == '\t'))
        --valueEnd;
      if (key == "transfer-encoding")
        return Invalid; // Chunked requests are not supported.
      if (key == "content-length")
      {
        if (hasLength || value == valueEnd)
          return Invalid;
        hasLength = true;
        for (; value < valueEnd; ++value)
        {
          if (!isdigit((unsigned char)request_[value]))
            return Invalid;
          contentLength_ = contentLength_ * 10 + (request_[value] - '0');
          if (contentLength_ > MCP_MAX_BODY)
            return TooLarge;
        }
        if (headerEnd_ + contentLength_ > 4096)
          return TooLarge;
      }
      start = end + 2;
    }
    return Waiting;
  }
};

// Owns a single HTTP connection across loop() calls. The dispatch callback
// writes one response and returns whether the connection should remain open.
// Restart is deliberately armed by dispatch only AFTER writing its response.
template <typename Client>
class HttpRequestSession
{
public:
  bool active() const { return active_; }
  bool restarting() const { return restartPending_; }
  Client& client() { return client_; }

  void accept(const Client& client, uint32_t now)
  {
    client_ = client;
    active_ = true;
    closing_ = false;
    reader_.reset(now);
  }

  void close()
  {
    if (active_)
      client_.stop();
    active_ = false;
    closing_ = false;
  }

  void requestRestart(uint32_t now)
  {
    restartPending_ = true;
    restartAt_ = now;
  }

  bool restartReady(uint32_t now) const
  {
    return restartPending_ && (uint32_t)(now - restartAt_) >= 250U;
  }

  template <typename Dispatch>
  void poll(uint32_t now, Dispatch dispatch)
  {
    if (!active_ || restartPending_)
      return;
    if (!client_.connected())
    {
      close();
      return;
    }
    if (closing_)
    {
      if ((uint32_t)(now - closeAt_) >= 300U)
        close();
      return;
    }

    const HttpRequestReader::Result result = reader_.poll(client_, now);
    if (result == HttpRequestReader::Waiting)
      return;
    if (result != HttpRequestReader::Complete)
    {
      if (result == HttpRequestReader::TooLarge)
        client_.print("HTTP/1.1 413 Request Entity Too Large\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
      else if (result == HttpRequestReader::Invalid)
        client_.print("HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
      else
      {
        close();
        return;
      }
      closing_ = true;
      closeAt_ = now;
      return;
    }

    const bool keepAlive = dispatch(client_, reader_);
    if (keepAlive)
      reader_.reset(now, true);
    else
    {
      closing_ = true;
      closeAt_ = now;
    }
    // Always return after one response, even with pipelined requests queued.
  }

private:
  Client client_;
  HttpRequestReader reader_;
  bool active_ = false;
  bool closing_ = false;
  bool restartPending_ = false;
  uint32_t closeAt_ = 0;
  uint32_t restartAt_ = 0;
};
