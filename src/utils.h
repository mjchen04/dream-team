//
//  Utils.h
//  codeForce practice
//
//  Created by Brian Cho on 1/22/17.
//  Copyright © 2017 Cho. All rights reserved.
//

#ifndef Utils_h
#define Utils_h

#include <string>
#include <vector>
#include "http_request.h"

// parses a string by delimeter
void tokenize(const std::string &in, std::vector<std::string> &tokens,
              const std::string &delimeter = " ");
HttpRequest parse_message(std::string raw_message);

#endif /* Utils_h */
