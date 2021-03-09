/*
 * Copyright (c) 2021 Sylvestre Gallon
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef __CORE_H__
#define __CORE_H__

#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>
#include <thread>

#include <boost/asio.hpp>

#include "buffer_splitter.h"
#include "config.h"
#include "data.h"
#include "file_watcher.h"
#include "parser.h"
#include "ui.h"

using Timepoint = std::chrono::time_point<std::chrono::system_clock>;

class Core : public boost::enable_shared_from_this<Core> {
public:
  explicit Core(std::shared_ptr<clf::Config> cfg,
                std::filesystem::path const &path);
  Core() = delete;
  ~Core();
  Core(Core const &) = delete;
  Core &operator=(Core const &) = delete;

  void run();

protected:
  void
  onNewBuffer(std::vector<std::pair<clf::Timepoint, std::string>> &&buffers);

private:
  void refreshDisplayCallback();
  void checkAlarmCallback();
  void getDataFromSplitter(std::pair<Timepoint, std::string> &&line);

  // Timers
  boost::asio::io_context _ioCtx;
  boost::asio::steady_timer _refreshTimer;
  boost::asio::steady_timer _alertOffTimer;
  std::thread _asioThread;

  // Buffer mgmt
  clf::BufferSplitter _splitter;
  clf::FileWatcher _fileWatcher;

  // Config
  std::shared_ptr<clf::Config> _config;

  // data
  clf::Data _data;

  // Ui
  clf::Ui _ui;

  // handle data
  boost::asio::thread_pool _handleDataPool;
};

#endif /* __CORE_H__ */