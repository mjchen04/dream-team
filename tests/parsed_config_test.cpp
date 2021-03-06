#include <sstream>
#include <string>
#include "gtest/gtest.h"
#include "../src/parsed_config.h"

class ParsedConfigTest : public ::testing::Test {
protected:
  NginxConfigParser parser_;
  NginxConfig out_config_; 
  ParsedConfig* parsed_config_;

  void CreateParsedConfig(const std::string config_string) {
    std::stringstream config_stream(config_string);
    parser_.Parse(&config_stream, &out_config_);
    parsed_config_ = new ParsedConfig(&out_config_);
  }
};

TEST_F(ParsedConfigTest, BasicConfigTest) {
  CreateParsedConfig("server {\n\tlisten 2020;\n}\n");
  NginxConfig* server_block = (parsed_config_->FilterStatements(&out_config_, "server")[0])->child_block_.get();
  EXPECT_EQ("2020", parsed_config_->GetStatementValue(server_block, "listen"));
}

TEST_F(ParsedConfigTest, InvalidConfigTest) {
  CreateParsedConfig("server 2020; }");
  NginxConfig* server_block = (parsed_config_->FilterStatements(&out_config_, "server")[0])->child_block_.get();
  EXPECT_EQ("", parsed_config_->GetStatementValue(server_block, "listen"));
}

TEST_F(ParsedConfigTest, MultipleLineConfigTest) {
  CreateParsedConfig("server {\n\tlisten 2020;\n\troot ../;\n}\n");
  NginxConfig* server_block = (parsed_config_->FilterStatements(&out_config_, "server")[0])->child_block_.get();
  EXPECT_EQ("2020", parsed_config_->GetStatementValue(server_block, "listen"));
  EXPECT_EQ("../", parsed_config_->GetStatementValue(server_block, "root"));
}
