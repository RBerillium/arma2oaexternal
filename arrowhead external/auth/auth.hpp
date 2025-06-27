#pragma once

#include <string>
#include <vector>

#pragma comment (lib, "authlib.lib")

struct response_t
{
	std::string unix_expire_time;
	bool valid;
	std::string message;
};

class authlib
{
private:

	bool authorized = false;
	std::string link{};
	std::string application_name{};
	std::string api_public_key{};
	std::string cert_hash{};
	std::string ca_cert_path;

public:

	authlib();
	~authlib();

	bool init(std::string application_name, std::string api_public_key, std::string link);

	std::string get_hwid_string();
	std::string get_hwid_hash();

	bool auth(const std::string& key, response_t& result);

	std::vector<uint8_t> download_file(int file_id, const std::string& key);

	bool ban_key(const std::string& key);

	bool create_ca_cert();
	bool delete_ca_cert();

	bool is_authorized();
};