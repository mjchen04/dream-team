//
// Created by Usama Riaz on 1/24/17.
// Copyright © 2017 Riaz. All rights reserved.
//

#ifndef BasicServerConfig_h
#define BasicServerConfig_h

#include <map>
#include "parsed_config.h"
#include "traffic_monitor.h"
#include "request_handler.h"
#include "status_handler.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "not_found_handler.h"

class BasicServerConfig : public ParsedConfig {
 public:
  BasicServerConfig(NginxConfig* config);
  BasicServerConfig() {} // constructor for mocking purposes

  bool Init();
  void RegisterTrafficMonitor(TrafficMonitor* monitor);
  void UpdateStatusHandlers(const Request& req, const Response& resp);

  unsigned GetPortNumber();
  RequestHandler* GetRequestHandlerFromUri(std::string uri);
 private:
  unsigned port_number_;

  std::map<std::string, std::unique_ptr<RequestHandler>> uri_to_request_handler_;
  std::unique_ptr<RequestHandler> default_handler_;
  TrafficMonitor* traffic_monitor_ = nullptr;

  bool InitPortNumber(NginxConfig* config);
  bool InitRequestHandlers(NginxConfig* config);

  std::string GetLongestMatchingUri(std::string client_uri);
  std::unique_ptr<RequestHandler> BuildHandlerForUri(std::string uri, 
						     std::string handler_id, 
						     NginxConfig* child_block);
};

#endif
