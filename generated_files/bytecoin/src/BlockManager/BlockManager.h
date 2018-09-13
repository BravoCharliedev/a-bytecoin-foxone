#pragma once

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>

#include <boost/program_options/variables_map.hpp>

#include "Common/ConsoleHandler.h"
#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "CryptoNoteCore/Currency.h"
#include "NodeRpcProxy/NodeRpcProxy.h"
#include <Logging/LoggerRef.h>
#include <Logging/LoggerManager.h>

#include <System/Dispatcher.h>
#include <System/Ipv4Address.h>

namespace CryptoNote {
	class BlockManager : public CryptoNote::INodeObserver, public CryptoNote::INodeRpcProxyObserver{
	public:

		bool init(const boost::program_options::variables_map& vm);
		bool deinit();
		bool run();
		void stop();

		BlockManager(System::Dispatcher& dispatcher, Logging::LoggerManager& log);
		std::string get_commands_str();
		bool process_command(const std::vector<std::string> &args);

	private:
		std::string accountNumber;

		std::string m_daemon_address;
		std::string m_daemon_host;
		uint16_t m_daemon_port;
		uint64_t m_default_fee;

		Common::ConsoleHandler m_consoleHandler;
		Logging::LoggerManager& logManager;
		System::Dispatcher& m_dispatcher;
		Logging::LoggerRef logger;

		std::unique_ptr<CryptoNote::NodeRpcProxy> m_node;

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////

		void handle_command_line(const boost::program_options::variables_map& vm);
		bool help(const std::vector<std::string> &args = std::vector<std::string>());
		bool exit(const std::vector<std::string> &args);
		bool push_block(const std::vector<std::string> &args);
		bool find_address(const std::vector<std::string> &args);
		bool print_list(const std::vector<std::string> &args);
		bool show_blockchain_height(const std::vector<std::string> &args);
		bool set_log(const std::vector<std::string> &args);
		bool print_hash(const std::vector<std::string> &args);

		Logging::LoggerMessage success_msg_writer(bool color = false) {
			return logger(Logging::INFO, color ? Logging::GREEN : Logging::DEFAULT);
		}

		Logging::LoggerMessage fail_msg_writer() const {
			auto msg = logger(Logging::ERROR, Logging::BRIGHT_RED);
			msg << "Error: ";
			return msg;
		}

		//----------------- INodeRpcProxyObserver --------------------------
		virtual void connectionStatusUpdated(bool connected) override;
		//----------------------------------------------------------

		void printConnectionError() const;
	};

}