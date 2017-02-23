//
// Created by Usama Riaz on 1/24/17.
// Copyright © 2017 Riaz. All rights reserved.
//

#include "basic_server_config.h"
#include "utils.h"
#include "echo_handler.h"

#define PATH_NUM_TOKENS 3
#define DEFAULT_NUM_TOKENS 2

const std::string PORT_KEY = "port";
const std::string LOCATION_OBJ = "path";
const std::string DEFAULT_OBJ = "default";
const std::string ROOT_KEY = "root";

const std::string HANDLER_ECHO_ID = "EchoHandler";
const std::string HANDLER_STATIC_ID = "StaticHandler";
const std::string HANDLER_NOT_FOUND_ID = "NotFoundHandler";

BasicServerConfig::BasicServerConfig(NginxConfig* config) : ParsedConfig(config) {}

bool BasicServerConfig::Init() {
  NginxConfig* root_config = GetConfig();
  bool init_status = 
    InitPortNumber(root_config) &&
    InitRequestHandlers(root_config);

  return init_status;
}

bool BasicServerConfig::InitPortNumber(NginxConfig* config) {
  std::string port_num = GetStatementValue(config, PORT_KEY);
  if (port_num == "") {
    return false;
  }
  
  port_number_ = std::stoi(port_num);
  return true;
}

bool BasicServerConfig::InitRequestHandlers(NginxConfig* config) {
  std::vector<std::shared_ptr<NginxConfigStatement>> location_matches = 
    FilterStatements(config, LOCATION_OBJ);
  
  for (const auto& statement : location_matches) {
    if (statement->tokens_.size() >= PATH_NUM_TOKENS) {
      NginxConfig* path_child_block = statement->child_block_.get();
      std::string uri = statement->tokens_[1];
      std::string handler_id = statement->tokens_[2];
      
      uri_to_request_handler_[uri] = BuildHandlerForUri(uri, handler_id, path_child_block);
    }    
  }

  std::vector<std::shared_ptr<NginxConfigStatement>> default_matches =
    FilterStatements(config, DEFAULT_OBJ);

  for (const auto& statement : default_matches) {
    if (statement->tokens_.size() == DEFAULT_NUM_TOKENS) {
      NginxConfig* default_child_block = statement->child_block_.get();
      std::string handler_id = statement->tokens_[1];
      default_handler_ = BuildHandlerForUri("", handler_id, default_child_block);
    }    
  }
  
  return true;
}

std::string BasicServerConfig::GetLongestMatchingUri(std::string client_uri) {
  std::vector<std::string> host_url_keys;
  for (auto const& map: uri_to_request_handler_) {
    host_url_keys.push_back(map.first);
  }
  
  unsigned max_matches = 0;
  std::string max_matches_path = "";
  for (std::string host_path : host_url_keys) {
    unsigned matches = NumberMatches(host_path, client_uri);
    if (matches > max_matches) {
      max_matches = matches;
      max_matches_path = host_path;
    }
  }
  
  return max_matches_path;
}


RequestHandler* BasicServerConfig::GetRequestHandlerFromUri(std::string uri) {
  std::string host_uri = GetLongestMatchingUri(uri);
  if (uri_to_request_handler_.count(host_uri) > 0) {
    return uri_to_request_handler_[host_uri].get();
  }

  return nullptr;
}

std::unique_ptr<RequestHandler> BasicServerConfig::BuildHandlerForUri(std::string uri, 
								      std::string handler_id, 
								      NginxConfig* child_block) {
  RequestHandler* handler = nullptr;
  if (handler_id == HANDLER_ECHO_ID) {
    handler = new EchoHandler();    
  }
  
  if (handler != nullptr) {
    handler->Init(uri, *child_block);
  }

  return std::unique_ptr<RequestHandler>(handler);
}

unsigned BasicServerConfig::GetPortNumber() {
  return port_number_;
}
